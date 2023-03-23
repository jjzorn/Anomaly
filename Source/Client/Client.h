// Copyright 2023 Justus Zorn

#ifndef ANOMALY_CLIENT_CLIENT_H
#define ANOMALY_CLIENT_CLIENT_H

#include <string>

#include <enet.h>

#include <Audio/Audio.h>
#include <Renderer/Renderer.h>
#include <Renderer/Window.h>

class Client {
public:
	Client(Window& window);
	Client(const Client&) = delete;
	~Client();

	Client& operator=(const Client&) = delete;

	bool connect(Window& window, const std::string& hostname, uint16_t port);
	bool update(Audio& audio, Renderer& renderer);

private:
	ENetHost* host = nullptr;
	ENetPeer* peer = nullptr;

	void draw(Renderer& renderer, ENetPacket* packet);
	void handle_commands(Renderer& renderer, ENetPacket* packet);
	void handle_audio(Audio& audio, ENetPacket* packet);
	void update_content(Audio& audio, Renderer& window, ENetPacket* packet);
};

#endif
