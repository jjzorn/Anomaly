// Copyright 2023 Justus Zorn

#ifndef ANOMALY_CLIENT_CLIENT_H
#define ANOMALY_CLIENT_CLIENT_H

#include <string>

#include <enet.h>

#include <Client/Renderer.h>

class Client {
public:
	Client(const std::string& hostname, uint16_t port);
	Client(const Client&) = delete;
	~Client();

	Client& operator=(const Client&) = delete;

	bool update(Renderer& renderer);

private:
	ENetHost* host = nullptr;
	ENetPeer* peer = nullptr;

	void update_content(Renderer& renderer, ENetPacket* packet);
};

#endif
