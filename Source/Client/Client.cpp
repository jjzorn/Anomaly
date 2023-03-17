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
			if (event.channelID == CONTENT_CHANNEL) {
				update_content(renderer, event.packet);
			}
			enet_packet_destroy(event.packet);
			break;
		}
	}
	return true;
}

void Client::update_content(Renderer& renderer, ENetPacket* packet) {
	if (packet->data[0] == static_cast<uint8_t>(PacketType::IMAGE)) {
		uint32_t id = read32(packet->data + 1);
		uint32_t length = read32(packet->data + 5);
		renderer.load_image(id, packet->data + 9, length);
	}
}
