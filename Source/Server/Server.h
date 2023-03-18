// Copyright 2023 Justus Zorn

#ifndef ANOMALY_SERVER_SERVER_H
#define ANOMALY_SERVER_SERVER_H

#include <vector>

#include <enet.h>

#include <Anomaly.h>

class ContentManager;

class Server {
public:
	Server(uint16_t port);
	Server(const Server&) = delete;
	~Server();

	Server& operator=(const Server&) = delete;

	void update(ContentManager& content);

	void update_client_content(uint16_t client, ContentType type, uint16_t id, const std::vector<uint8_t>& data);
	void update_content(ContentType type, uint16_t id, const std::vector<uint8_t>& data);

private:
	ENetHost* host;

	struct Client {
		bool connected = false;
		ENetPeer* peer;
		std::vector<Sprite> sprites;
	};

	std::vector<Client> clients;

	ENetPacket* create_sprite_packet(Client& client);
	static ENetPacket* create_content_packet(ContentType type, uint16_t id, const std::vector<uint8_t>& data);
};

#endif
