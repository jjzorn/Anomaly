// Copyright 2023 Justus Zorn

#include <iostream>

#include <Common/Network.h>

#include <Client/Renderer.h>

bool recv(tcp_socket socket, uint8_t* buf, int length) {
	int got = 0;
	while (got < length) {
		int remaining = length - got;
		int next = netlib_tcp_recv(socket, buf + got, remaining);
		if (next <= 0) {
			return false;
		}
		got += next;
	}
	return true;
}

bool recv_uint32(tcp_socket socket, uint32_t& value) {
	uint8_t buf[4];
	if (!recv(socket, buf, 4)) {
		return false;
	}
	value = netlib_read32(buf);
	return true;
}

bool recv_vec(tcp_socket socket, std::vector<uint8_t>& vec) {
	uint32_t length;
	if (!recv_uint32(socket, length)) {
		return false;
	}
	vec.resize(length);
	recv(socket, vec.data(), length);
	return true;
}

int SDL_main(int argc, char* argv[]) {
	/*Renderer renderer;

	while (renderer.update()) {
		renderer.present();
	}
	*/

	Renderer renderer;

	netlib_init();

	tcp_socket socket;
	ip_address addr;
	if (netlib_resolve_host(&addr, "192.168.178.29", 17899) != 0) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Could not connect resolve server address");
		return 0;
	}

	socket = netlib_tcp_open(&addr);
	if (socket == nullptr) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Could not connect to server");
		return 0;
	}

	netlib_socket_set set = netlib_alloc_socket_set(1);
	netlib_tcp_add_socket(set, socket);

	bool loaded_image = false;
	uint32_t image_id;

	while (renderer.update()) {
		netlib_check_socket_set(set, 0);
		while (netlib_socket_ready(socket) && !loaded_image) {
			uint32_t msg_type;
			if (!recv_uint32(socket, msg_type)) {
				continue;
			}
			MessageType type = static_cast<MessageType>(msg_type);
			if (type == MessageType::SERVER_CLOSED) {
				return 0;
			}
			else if (type == MessageType::SERVER_CONTENT_UPDATE_IMAGE) {
				uint32_t id;
				if (!recv_uint32(socket, id)) {
					return 0;
				}
				std::vector<uint8_t> data;
				if (!recv_vec(socket, data)) {
					return 0;
				}
				image_id = id;
				renderer.load_image(id, data);
				loaded_image = true;
			}
			netlib_check_socket_set(set, 0);
		}

		renderer.clear(0, 255, 0);
		if (loaded_image) {
			renderer.draw_sprite(image_id, 600, 400);
		}
		renderer.present();
	}

	netlib_quit();

	return 0;
}
