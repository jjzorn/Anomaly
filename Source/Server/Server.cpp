// Copyright 2023 Justus Zorn

#include <iostream>

#include <Anomaly.h>
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
			content.init_client(*this, peer_id);
			break;
		case ENET_EVENT_TYPE_DISCONNECT_TIMEOUT:
		case ENET_EVENT_TYPE_DISCONNECT:
			std::cout << "INFO: Client disconnected (ID " << peer_id << ")\n";
			clients[peer_id].connected = false;
			break;
		case ENET_EVENT_TYPE_RECEIVE:
			enet_packet_destroy(event.packet);
			break;
		}
	}
}

void Server::update_client_image(uint16_t client, uint32_t id, const std::vector<uint8_t>& data) {
	ENetPacket* packet = create_image_packet(id, data);
	enet_peer_send(clients[client].peer, CONTENT_CHANNEL, packet);
}

void Server::update_image(uint32_t id, const std::vector<uint8_t>& data) {
	ENetPacket* packet = create_image_packet(id, data);
	enet_host_broadcast(host, CONTENT_CHANNEL, packet);
}

ENetPacket* Server::create_image_packet(uint32_t id, const std::vector<uint8_t>& data) {
	ENetPacket* packet = enet_packet_create(nullptr, 9 + data.size(), ENET_PACKET_FLAG_RELIABLE | ENET_PACKET_FLAG_UNSEQUENCED);
	packet->data[0] = static_cast<uint8_t>(PacketType::IMAGE);
	write32(packet->data + 1, id);
	write32(packet->data + 5, data.size());
	memcpy(packet->data + 9, data.data(), data.size());
	return packet;
}
