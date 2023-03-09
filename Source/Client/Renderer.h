// Copyright 2023 Justus Zorn

#ifndef ANOMALY_CLIENT_RENDERER_H
#define ANOMALY_CLIENT_RENDERER_H

#include <SDL.h>

class Renderer {
public:
	Renderer();
	Renderer(const Renderer&) = delete;
	~Renderer();

	Renderer& operator=(const Renderer&) = delete;

	bool update();
	void present();

private:
	SDL_Window* window = nullptr;
	SDL_Renderer* renderer = nullptr;
};

#endif
