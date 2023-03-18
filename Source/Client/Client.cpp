// Copyright 2023 Justus Zorn

#include <Anomaly.h>
#include <Client/Client.h>

#include <SDL.h>

Client::Client(const std::string& hostname, uint16_t port, Window& window) {
	if (enet_initialize() < 0) {
		window.error("Network initialization failed");
	}
	host = enet_host_create(nullptr, 1, NET_CHANNELS, 0, 0);
	if (host == nullptr) {
		window.error("Could not create network socket");
	}
	ENetAddress address = { 0 };
	if (enet_address_set_host(&address, hostname.c_str()) < 0) {
		window.error("Could not resolve hostname '" + hostname + "'");
	}
	address.port = port;
	peer = enet_host_connect(host, &address, NET_CHANNELS, 0);
	ENetEvent event;
	if (enet_host_service(host, &event, 5000) == 0 || event.type != ENET_EVENT_TYPE_CONNECT) {
		window.error("Could not connect to '" + hostname + ":[" + std::to_string(port) + "]'");
	}
}

Client::~Client() {
	enet_peer_disconnect_now(peer, 0);
	enet_host_destroy(host);
	enet_deinitialize();
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
	uint16_t id = read16(packet->data + 1);
	uint32_t length = read32(packet->data + 3);
	if (packet->data[0] == static_cast<uint8_t>(ContentType::IMAGE)) {
		SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Received content update (image ID %u)", id);
		renderer.load_image(id, packet->data + 7, length);
	}
	else if (packet->data[0] == static_cast<uint8_t>(ContentType::FONT)) {
		//SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Received content update (font ID %u)", id);
		//renderer.load_font(id, packet->data + 7, length);
	}
}

void Client::draw(Renderer& renderer, ENetPacket* packet) {
	renderer.clear(0, 0, 0);
	uint32_t length = read32(packet->data);
	for (uint32_t i = 0; i < length; ++i) {
		uint32_t offset = 4 + i * sizeof(Sprite);
		if ((i + 1) * sizeof(Sprite) > packet->dataLength) {
			return;
		}
		uint16_t texture = read16(packet->data + offset);
		int16_t x = static_cast<int16_t>(read16(packet->data + offset + 2));
		int16_t y = static_cast<int16_t>(read16(packet->data + offset + 4));
		uint16_t scale = read16(packet->data + offset + 6);
		renderer.draw_sprite(texture, x, y, scale);
	}
	//renderer.draw_text(0, 600, 400, "My text!");
	renderer.present();
}
