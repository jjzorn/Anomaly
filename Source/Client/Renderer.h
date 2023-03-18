// Copyright 2023 Justus Zorn

#ifndef ANOMALY_CLIENT_RENDERER_H
#define ANOMALY_CLIENT_RENDERER_H

#include <vector>

#include <stb_truetype.h>
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

	void load_image(uint16_t id, const uint8_t* data, uint32_t length);
	void load_font(uint16_t id, const uint8_t* data, uint32_t length);

	void draw_sprite(uint16_t sprite, uint16_t x, uint16_t y);
	void draw_text(uint16_t font, uint16_t x, uint16_t y, const char* text);

private:
	struct Font {
		std::vector<uint8_t> buffer;
		stbtt_fontinfo info;
		bool init = false;
	};

	SDL_Window* window = nullptr;
	SDL_Renderer* renderer = nullptr;

	std::vector<SDL_Texture*> textures;
	std::vector<Font> fonts;
};

#endif
