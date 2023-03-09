// Copyright 2023 Justus Zorn

#ifndef ANOMALY_SOCKET_H
#define ANOMALY_SOCKET_H

#include <netlib.h>

#include <string>

class Socket {
public:
	Socket();
	Socket(const Socket&) = delete;
	~Socket();

	Socket& operator=(const Socket&) = delete;

	bool connect(const std::string& host, uint16_t port);

	int send(const char* buffer, int len);
	int recv(char* buffer, int maxlen);

private:
	tcp_socket socket = nullptr;

	friend class ServerSocket;
};

class ServerSocket {
public:
	ServerSocket();
	ServerSocket(const ServerSocket&) = delete;
	~ServerSocket();

	ServerSocket& operator=(const ServerSocket&) = delete;

	bool listen(uint16_t port);
	bool accept(Socket& client);

private:
	tcp_socket socket = nullptr;
};

#endif
