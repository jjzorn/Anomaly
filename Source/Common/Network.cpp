// Copyright 2023 Justus Zorn

#include <Common/Network.h>

#include <iostream>

Socket::Socket() {
	netlib_init();
}

Socket::~Socket() {
	netlib_tcp_close(socket);
	netlib_quit();
}

bool Socket::connect(const std::string& host, uint16_t port) {
	if (socket) {
		netlib_tcp_close(socket);
		socket = nullptr;
	}
	ip_address addr;
	if (netlib_resolve_host(&addr, host.c_str(), port)) {
		return false;
	}
	socket = netlib_tcp_open(&addr);
	if (!socket) {
		return false;
	}
	return true;
}

bool Socket::send_uint32(uint32_t i) {
	uint8_t buf[4] = {
		(i & 0xFF000000) >> 24,
		(i & 0x00FF0000) >> 16,
		(i & 0x0000FF00) >> 8,
		(i & 0x000000FF)
	};
	if (netlib_tcp_send(socket, buf, 4) < 4) {
		return false;
	}
	return true;
}

bool Socket::recv_uint32(uint32_t& i) {
	uint8_t buf[4];
	if (!recv(buf, 4)) {
		return false;
	}
	i = (buf[0] << 24) + (buf[1] << 16) + (buf[2] << 8) + buf[3];
	return true;
}

bool Socket::send_vec(const std::vector<uint8_t>& vec) {
	if (vec.size() > INT_MAX) {
		return false;
	}
	uint32_t len = vec.size();
	if (!send_uint32(len)) {
		return false;
	}
	if (netlib_tcp_send(socket, vec.data(), len) < len) {
		return false;
	}
	return true;
}

bool Socket::recv_vec(std::vector<uint8_t>& vec) {
	uint32_t len;
	if (!recv_uint32(len)) {
		return false;
	}
	vec.resize(len);
	if (!recv(vec.data(), len)) {
		return false;
	}
	return true;
}

bool Socket::recv(uint8_t* buffer, int len) {
	int got = 0;
	while (got < len) {
		int newlen = netlib_tcp_recv(socket, buffer + got, len - got);
		if (newlen == -1) {
			return false;
		}
		got += newlen;
	}
	return true;
}

ServerSocket::ServerSocket() {
	netlib_init();
}

ServerSocket::~ServerSocket() {
	netlib_tcp_close(socket);
	netlib_quit();
}

bool ServerSocket::listen(uint16_t port) {
	if (socket) {
		netlib_tcp_close(socket);
		socket = nullptr;
	}
	ip_address addr;
	if (netlib_resolve_host(&addr, nullptr, port)) {
		return false;
	}
	socket = netlib_tcp_open(&addr);
	if (!socket) {
		return false;
	}
	return true;
}

bool ServerSocket::accept(Socket& client) {
	client.socket = netlib_tcp_accept(socket);
	return client.socket;
}
