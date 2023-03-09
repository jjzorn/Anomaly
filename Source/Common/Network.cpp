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

int Socket::send(const char* buffer, int len) {
	return netlib_tcp_send(socket, buffer, len);
}

int Socket::recv(char* buffer, int maxlen) {
	return netlib_tcp_recv(socket, buffer, maxlen);
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
