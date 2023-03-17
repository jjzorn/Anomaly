// Copyright 2023 Justus Zorn

#include <Anomaly.h>
#include <Client/Client.h>

#include <SDL.h>

Client::Client(const std::string& hostname, uint16_t port) {
	host = enet_host_create(nullptr, 1, NET_CHANNELS, 0, 0);
	if (host == nullptr) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Could not create network socket");
		return;
	}
	ENetAddress address = { 0 };
	if (enet_address_set_host(&address, hostname.c_str()) < 0) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Could not resolve hostname %s", hostname.c_str());
		return;
	}
	address.port = port;
	peer = enet_host_connect(host, &address, NET_CHANNELS, 0);
	ENetEvent event;
	if (enet_host_service(host, &event, 5000) > 0 && event.type == ENET_EVENT_TYPE_CONNECT) {
		SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Connected");
	}
	else {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Could not connect to %s:[%u]", hostname.c_str(), (unsigned int)port);
		enet_peer_reset(peer);
	}
}

Client::~Client() {
	enet_peer_disconnect_now(peer, 0);
	enet_host_destroy(host);
}

bool Client::update(Renderer& renderer) {
	ENetEvent event;
	while (enet_host_service(host, &event, 0) > 0) {
		switch (event.type) {
		case ENET_EVENT_TYPE_DISCONNECT_TIMEOUT:
		case ENET_EVENT_TYPE_DISCONNECT:
			return false;
		case ENET_EVENT_TYPE_RECEIVE:
			if (event.channelID == SPRITE_CHANNEL) {
				draw(renderer, event.packet);
			} else if (event.channelID == CONTENT_CHANNEL) {
				update_content(renderer, event.packet);
			}
			enet_packet_destroy(event.packet);
			break;
		}
	}
	return true;
}

void Client::update_content(Renderer& renderer, ENetPacket* packet) {
	if (packet->data[0] == static_cast<uint8_t>(ContentType::IMAGE)) {
		uint16_t id = read16(packet->data + 1);
		uint32_t length = read32(packet->data + 3);
		SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Received content update (image ID %u)", id);
		renderer.load_image(id, packet->data + 7, length);
	}
}

void Client::draw(Renderer& renderer, ENetPacket* packet) {
	renderer.clear(0, 0, 0);
	uint32_t length = read32(packet->data);
	for (uint32_t i = 0; i < length; ++i) {
		uint32_t offset = 4 + i * 6;
		if (i * 6 + 6 > packet->dataLength) {
			return;
		}
		uint16_t sprite = read16(packet->data + offset);
		uint16_t x = read16(packet->data + offset + 2);
		uint16_t y = read16(packet->data + offset + 4);
		renderer.draw_sprite(sprite, x, y);
	}
	renderer.present();
}
