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
	ENetPacket* input_packet = renderer.get_window().create_input_packet();
	if (input_packet) {
		enet_peer_send(peer, INPUT_CHANNEL, input_packet);
	}
	ENetEvent event;
	while (enet_host_service(host, &event, 0) > 0) {
		switch (event.type) {
		case ENET_EVENT_TYPE_DISCONNECT_TIMEOUT:
		case ENET_EVENT_TYPE_DISCONNECT:
			return false;
		case ENET_EVENT_TYPE_RECEIVE:
			if (event.channelID == SPRITE_CHANNEL) {
				draw(renderer, event.packet);
			}
			else if (event.channelID == COMMAND_CHANNEL) {
				handle_commands(renderer, event.packet);
			}
			else if (event.channelID == CONTENT_CHANNEL) {
				update_content(renderer, event.packet);
			}
			enet_packet_destroy(event.packet);
			break;
		}
	}
	return true;
}

void Client::draw(Renderer& renderer, ENetPacket* packet) {
	renderer.clear(0.0f, 0.0f, 0.0f);
	uint32_t length = read32(packet->data);
	uint8_t* data = packet->data + 4;
	for (uint32_t i = 0; i < length; ++i) {
		uint32_t id = read32(data);
		float x = read_float(data + 4);
		float y = read_float(data + 8);
		float scale = read_float(data + 12);
		if (id & 0x80000000) {
			uint8_t r, g, b;
			r = data[16];
			g = data[17];
			b = data[18];
			uint32_t length = read32(data + 19);
			renderer.draw_text(id & ~0x80000000, x, y, scale, r, g, b, data + 23, length);
			data += 23;
			data += length;
		}
		else {
			renderer.draw_sprite(id, x, y, scale);
			data += 16;
		}
	}
	renderer.present();
}

void Client::handle_commands(Renderer& renderer, ENetPacket* packet) {
	uint32_t size = read32(packet->data);
	uint8_t* data = packet->data + 4;
	for (uint32_t i = 0; i < size; ++i) {
		uint8_t type = data[i];
		if (type == static_cast<uint8_t>(Command::Type::START_TEXT_INPUT)) {
			SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Started text input");
			renderer.get_window().start_text_input();
		}
		else if (type == static_cast<uint8_t>(Command::Type::STOP_TEXT_INPUT)) {
			SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Stopped text input");
			renderer.get_window().stop_text_input();
		}
	}
}

void Client::update_content(Renderer& renderer, ENetPacket* packet) {
	uint32_t id = read32(packet->data + 1);
	uint32_t length = read32(packet->data + 5);
	if (packet->data[0] == static_cast<uint8_t>(ContentType::IMAGE)) {
		SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Received content update (image ID %u)", id);
		renderer.load_image(id, packet->data + 9, length);
	}
	else if (packet->data[0] == static_cast<uint8_t>(ContentType::FONT)) {
		SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Received content update (font ID %u)", id);
		renderer.load_font(id, packet->data + 9, length);
	}
}
