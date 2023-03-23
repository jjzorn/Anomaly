// Copyright 2023 Justus Zorn

#include <iostream>

#include <Server/ContentManager.h>
#include <Server/Server.h>

Server::Server(ContentManager& content, uint16_t port) : content{ &content } {
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

void Server::update(Script& script, double dt) {
	ENetEvent event;
	while (enet_host_service(host, &event, 0) > 0) {
		uint16_t peer_id = event.peer->incomingPeerID;
		switch (event.type) {
		case ENET_EVENT_TYPE_CONNECT:
			clients[peer_id].peer = event.peer;
			break;
		case ENET_EVENT_TYPE_DISCONNECT_TIMEOUT:
		case ENET_EVENT_TYPE_DISCONNECT:
			script.on_quit(peer_id);
			clients[peer_id].connected = false;
			break;
		case ENET_EVENT_TYPE_RECEIVE:
			if (clients[peer_id].connected) {
				client_input(peer_id, event.packet, script);
			}
			else {
				bool has_touch = event.packet->data[0];
				clients[peer_id].connected = true;
				clients[peer_id].has_touch = has_touch;
				content->init_client(*this, peer_id);
				script.on_join(peer_id, has_touch);
			}
			enet_packet_destroy(event.packet);
			break;
		}
	}
	script.on_tick(dt);
	for (Client& client : clients) {
		if (!client.connected) continue;
		ENetPacket* packet = create_sprite_packet(client);
		enet_peer_send(client.peer, SPRITE_CHANNEL, packet);
		client.sprites.clear();
		if (client.commands.size() > 0) {
			packet = create_command_packet(client);
			enet_peer_send(client.peer, COMMAND_CHANNEL, packet);
			client.commands.clear();
		}
		if (client.audio_commands.size() > 0) {
			packet = create_audio_packet(client);
			enet_peer_send(client.peer, AUDIO_CHANNEL, packet);
			client.audio_commands.clear();
		}
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

bool Server::start_text_input(uint16_t client) {
	if (client >= clients.size() || !clients[client].connected) {
		return false;
	}
	clients[client].commands.push_back({ Command::Type::START_TEXT_INPUT });
	return true;
}

bool Server::stop_text_input(uint16_t client) {
	if (client >= clients.size() || !clients[client].connected) {
		return false;
	}
	clients[client].commands.push_back({ Command::Type::STOP_TEXT_INPUT });
	return true;
}

const char* Server::get_composition(uint16_t client) {
	if (client >= clients.size() || !clients[client].connected) {
		return nullptr;
	}
	return clients[client].composition.c_str();
}

int Server::draw_sprite(uint16_t client, const std::string& path, float x, float y, float scale) {
	if (client >= clients.size() || !clients[client].connected) {
		return 1;
	}
	uint32_t id = content->get_image_id(path);
	if (id == 0) {
		return 2;
	}
	clients[client].sprites.push_back({ false, id, x, y, scale, 0, 0, 0, "" });
	return 0;
}

int Server::draw_text(uint16_t client, const std::string& path, float x, float y, float scale,
	uint8_t r, uint8_t g, uint8_t b, std::string text) {
	if (client >= clients.size() || !clients[client].connected) {
		return 1;
	}
	uint32_t id = content->get_font_id(path);
	if (id == 0) {
		return 2;
	}
	clients[client].sprites.push_back({ true, id, x, y, scale, r, g, b, text });
	return 0;
}

bool Server::kick(uint16_t client) {
	if (client >= clients.size() || !clients[client].connected) {
		return false;
	}
	enet_peer_disconnect(clients[client].peer, 0);
	return true;
}

int Server::play(uint16_t client, const std::string& path, uint16_t channel, uint8_t volume) {
	if (client >= clients.size() || !clients[client].connected) {
		return 1;
	}
	uint32_t id = content->get_sound_id(path);
	if (id == 0) {
		return 2;
	}
	clients[client].audio_commands.push_back({ id, channel, volume, AudioCommand::Type::PLAY });
	return 0;
}

int Server::play_any(uint16_t client, const std::string& path, uint8_t volume) {
	if (client >= clients.size() || !clients[client].connected) {
		return 1;
	}
	uint32_t id = content->get_sound_id(path);
	if (id == 0) {
		return 2;
	}
	clients[client].audio_commands.push_back({ id, 0, volume, AudioCommand::Type::PLAY_ANY });
	return 0;
}

bool Server::stop(uint16_t client, uint16_t channel) {
	if (client >= clients.size() || !clients[client].connected) {
		return false;
	}
	clients[client].audio_commands.push_back({ 0, channel, 0, AudioCommand::Type::STOP });
	return true;
}

bool Server::stop_all(uint16_t client) {
	if (client >= clients.size() || !clients[client].connected) {
		return false;
	}
	clients[client].audio_commands.push_back({ 0, 0, 0, AudioCommand::Type::STOP_ALL });
	return true;
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

ENetPacket* Server::create_command_packet(Client& client) {
	uint32_t size = 4 + client.commands.size();
	ENetPacket* packet = enet_packet_create(nullptr, size, 0);
	write32(packet->data, static_cast<uint32_t>(client.commands.size()));
	uint8_t* data = packet->data + 4;
	for (const Command& command : client.commands) {
		*(data++) = static_cast<uint8_t>(command.type);
	}
	return packet;
}

ENetPacket* Server::create_audio_packet(Client& client) {
	uint32_t size = 4 + 8 * client.audio_commands.size();
	ENetPacket* packet = enet_packet_create(nullptr, size, 0);
	write32(packet->data, static_cast<uint32_t>(client.audio_commands.size()));
	uint8_t* data = packet->data + 4;
	for (const AudioCommand& audio_command : client.audio_commands) {
		write32(data, audio_command.id);
		write16(data + 4, audio_command.channel);
		data[6] = audio_command.volume;
		data[7] = static_cast<uint8_t>(audio_command.type);
		data += 8;
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

void Server::client_input(uint16_t client, ENetPacket* input_packet, Script& script) {
	uint8_t* data = input_packet->data;
	uint32_t length = read32(data);
	data += 4;
	for (uint32_t i = 0; i < length; ++i) {
		int32_t key = read32(data);
		bool down = data[4];
		data += 5;
		if (key == 1073741886 && down) {
			script.reload();
			content->reload(*this);
		}
		script.on_key_event(client, key, down);
	}
	length = read32(data);
	data += 4;
	clients[client].composition.assign(reinterpret_cast<const char*>(data), length);
	data += length;
	length = read32(data);
	data += 4;
	for (uint32_t i = 0; i < length; ++i) {
		float x = read_float(data);
		float y = read_float(data + 4);
		uint8_t button = data[8];
		uint8_t type = data[9];
		data += 10;
		if (clients[client].has_touch) {
			script.on_finger_event(client, x, y, button, type);
		}
		else {
			if (type == static_cast<uint8_t>(InputEventType::MOTION)) {
				script.on_mouse_motion(client, x, y);
			}
			else {
				script.on_mouse_button(client, x, y, button,
					type == static_cast<uint8_t>(InputEventType::DOWN));
			}
		}
	}
}
