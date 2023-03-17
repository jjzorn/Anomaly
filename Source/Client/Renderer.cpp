// Copyright 2023 Justus Zorn

#include <iostream>

#include <stb_image.h>

#include <Client/Renderer.h>

Renderer::Renderer() {
	SDL_Init(SDL_INIT_VIDEO);
#ifdef ANOMALY_MOBILE
	window = SDL_CreateWindow("Anomaly", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 0, 0, SDL_WINDOW_RESIZABLE | SDL_WINDOW_FULLSCREEN);
#else
	window = SDL_CreateWindow("Anomaly", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, SDL_WINDOW_RESIZABLE);
#endif
	if (!window) {
		std::cerr << "ERROR: Could not create SDL window\n";
		return;
	}
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if (!renderer) {
		std::cerr << "ERROR: Could not create SDL renderer\n";
		return;
	}
}

Renderer::~Renderer() {
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

bool Renderer::update() {
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
		case SDL_QUIT:
			return false;
		}
	}
	return true;
}

void Renderer::clear(uint8_t r, uint8_t g, uint8_t b) {
	SDL_SetRenderDrawColor(renderer, r, g, b, 255);
	SDL_RenderClear(renderer);
}

void Renderer::present() {
	SDL_RenderPresent(renderer);
}

void Renderer::load_image(uint16_t id, const uint8_t* data, uint32_t length) {
	if (textures.size() <= id) {
		textures.resize(id + 1);
	}
	if (textures[id] != nullptr) {
		SDL_DestroyTexture(textures[id]);
		textures[id] = nullptr;
	}
	int width, height;
	uint8_t* image_data = stbi_load_from_memory(data, length, &width, &height, nullptr, 4);
	if (image_data == nullptr) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Could not load image (ID %u)", id);
		return;
	}
	SDL_Surface* surface = SDL_CreateRGBSurfaceWithFormatFrom(image_data, width, height, 32, width * 4, SDL_PIXELFORMAT_RGBA32);
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
	textures[id] = texture;
	SDL_FreeSurface(surface);
	stbi_image_free(image_data);
}

void Renderer::draw_sprite(uint16_t sprite, uint16_t x, uint16_t y) {
	if (sprite >= textures.size() || textures[sprite] == nullptr) {
		return;
	}
	int width, height;
	SDL_QueryTexture(textures[sprite], nullptr, nullptr, &width, &height);
	SDL_Rect src;
	src.w = width;
	src.h = height;
	src.x = 0;
	src.y = 0;
	SDL_Rect dst;
	dst.w = width;
	dst.h = height;
	dst.x = x - width / 2;
	dst.y = y - height / 2;
	SDL_RenderCopy(renderer, textures[sprite], &src, &dst);
}
