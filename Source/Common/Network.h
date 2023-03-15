// Copyright 2023 Justus Zorn

#ifndef ANOMALY_COMMON_NETWORK_H
#define ANOMALY_COMMON_NETWORK_H

#include <string>
#include <vector>

#include <netlib.h>

enum class MessageType {
	SERVER_CLOSED,
	SERVER_CONTENT_UPDATE_IMAGE
};

class Socket {
public:
	Socket();
	Socket(const Socket&) = delete;
	~Socket();

	Socket& operator=(const Socket&) = delete;

	bool connect(const std::string& host, uint16_t port);
	void disconnect();

	bool send_uint32(uint32_t i);
	bool recv_uint32(uint32_t& i);

	bool send_vec(const std::vector<uint8_t>& vec);
	bool recv_vec(std::vector<uint8_t>& vec);

private:
	tcp_socket socket = nullptr;

	bool recv(uint8_t* buffer, int len);

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
