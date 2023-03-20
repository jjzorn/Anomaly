// Copyright 2023 Justus Zorn

#include <iostream>

#include <Server/ContentManager.h>
#include <Server/Server.h>

Server::Server(uint16_t port) {
	clients.resize(MAX_CLIENTS);

	ENetAddress address = { 0 };
	address.host = ENET_HOST_ANY;
	address.port = port;

	host = enet_host_create(&address, MAX_CLIENTS, NET_CHANNELS, 0, 0);
	if (host == nullptr) {
		std::cerr << "ERROR: Could not connect to network\n";
	}
}

Server::~Server() {
	enet_host_destroy(host);
}

void Server::update(ContentManager& content) {
	ENetEvent event;
	while (enet_host_service(host, &event, 0) > 0) {
		uint16_t peer_id = event.peer->incomingPeerID;
		switch (event.type) {
		case ENET_EVENT_TYPE_CONNECT:
			std::cout << "INFO: Client connected (ID " << peer_id << ")\n";
			clients[peer_id].connected = true;
			clients[peer_id].peer = event.peer;
			clients[peer_id].sprites.push_back({ 0, 0, -1.0f, 0.0f, 1.0f, 0, 0, 0 });
			clients[peer_id].sprites.push_back({ 1, 0, -1.0f, 0.0f, 0.5f, 0, 255, 0, "Hello, world!" });
			content.init_client(*this, peer_id);
			break;
		case ENET_EVENT_TYPE_DISCONNECT_TIMEOUT:
		case ENET_EVENT_TYPE_DISCONNECT:
			std::cout << "INFO: Client disconnected (ID " << peer_id << ")\n";
			clients[peer_id].connected = false;
			break;
		case ENET_EVENT_TYPE_RECEIVE:
			client_input(clients[peer_id], event.packet);
			enet_packet_destroy(event.packet);
			break;
		}
	}

	for (Client& client : clients) {
		if (!client.connected) continue;
		ENetPacket* packet = create_sprite_packet(client);
		enet_peer_send(client.peer, SPRITE_CHANNEL, packet);
	}
}

void Server::update_client_content(uint16_t client, ContentType type, uint32_t id, const std::vector<uint8_t>& data) {
	ENetPacket* packet = create_content_packet(type, id, data);
	enet_peer_send(clients[client].peer, CONTENT_CHANNEL, packet);
}

void Server::update_content(ContentType type, uint32_t id, const std::vector<uint8_t>& data) {
	ENetPacket* packet = create_content_packet(type, id, data);
	enet_host_broadcast(host, CONTENT_CHANNEL, packet);
}

ENetPacket* Server::create_sprite_packet(Client& client) {
	uint32_t size = 4;
	for (const Sprite& sprite : client.sprites) {
		if (sprite.is_text) {
			size += 23;
			size += sprite.text.length();
		}
		else {
			size += 16;
		}
	}
	ENetPacket* packet = enet_packet_create(nullptr, size, 0);
	write32(packet->data, static_cast<uint32_t>(client.sprites.size()));
	uint8_t* data = packet->data + 4;
	for (const Sprite& sprite : client.sprites) {
		write_float(data + 4, sprite.x);
		write_float(data + 8, sprite.y);
		write_float(data + 12, sprite.scale);
		if (sprite.is_text) {
			write32(data, sprite.id | 0x80000000);
			data[16] = sprite.r;
			data[17] = sprite.g;
			data[18] = sprite.b;
			write32(data + 19, static_cast<uint32_t>(sprite.text.length()));
			memcpy(data + 23, sprite.text.data(), sprite.text.length());
			data += 23;
			data += sprite.text.length();
		}
		else {
			write32(data, sprite.id);
			data += 16;
		}
	}
	return packet;
}

ENetPacket* Server::create_content_packet(ContentType type, uint32_t id, const std::vector<uint8_t>& data) {
	ENetPacket* packet = enet_packet_create(nullptr, 9 + data.size(), ENET_PACKET_FLAG_RELIABLE | ENET_PACKET_FLAG_UNSEQUENCED);
	packet->data[0] = static_cast<uint8_t>(type);
	write32(packet->data + 1, id);
	write32(packet->data + 5, data.size());
	memcpy(packet->data + 9, data.data(), data.size());
	return packet;
}

void Server::client_input(Client& client, ENetPacket* input_packet) {
	uint8_t* data = input_packet->data;
	uint32_t length = read32(data);
	data += 4;
	for (uint32_t i = 0; i < length; ++i) {
		int32_t key = read32(data);
		bool down = data[4];
		data += 5;
		std::cout << "Key " << key << " down: " << down << '\n';
	}
}
