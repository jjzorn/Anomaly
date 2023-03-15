// Copyright 2023 Justus Zorn

#ifndef ANOMALY_CLIENT_RENDERER_H
#define ANOMALY_CLIENT_RENDERER_H

#include <vector>

#include <SDL.h>

class Renderer {
public:
	Renderer();
	Renderer(const Renderer&) = delete;
	~Renderer();

	Renderer& operator=(const Renderer&) = delete;

	bool update();

	void clear(uint8_t r, uint8_t g, uint8_t b);
	void present();

	void load_image(uint32_t id, const std::vector<uint8_t>& data);
	void draw_sprite(uint32_t sprite, uint16_t x, uint16_t y);

private:
	SDL_Window* window = nullptr;
	SDL_Renderer* renderer = nullptr;

	std::vector<SDL_Texture*> textures;
};

#endif
