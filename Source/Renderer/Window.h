// Copyright 2023 Justus Zorn

#ifndef ANOMALY_RENDERER_WINDOW_H
#define ANOMALY_RENDERER_WINDOW_H

#include <string>

#include <SDL.h>

#include <Renderer/Input.h>

class Window {
public:
	Window();
	Window(const Window&) = delete;
	~Window();

	Window& operator=(const Window&) = delete;

	void error(const std::string& message);

	bool update();
	void present();

	int width() const;
	int height() const;

	ENetPacket* create_input_packet();

private:
	SDL_Window* window = nullptr;
	SDL_GLContext context = nullptr;

	Input input;
};

#endif
