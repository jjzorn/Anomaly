// Copyright 2023 Justus Zorn

#ifndef ANOMALY_SERVER_SERVER_H
#define ANOMALY_SERVER_SERVER_H

#include <vector>

#include <enet.h>

class ContentManager;

class Server {
public:
	Server(uint16_t port);
	Server(const Server&) = delete;
	~Server();

	Server& operator=(const Server&) = delete;

	void update(ContentManager& content);

	void update_client_image(uint16_t client, uint32_t id, const std::vector<uint8_t>& data);
	void update_image(uint32_t id, const std::vector<uint8_t>& data);

private:
	ENetHost* host;

	struct Client {
		bool connected = false;
		ENetPeer* peer;
	};

	std::vector<Client> clients;

	static ENetPacket* create_image_packet(uint32_t id, const std::vector<uint8_t>& data);
};

#endif
