// Copyright 2023 Justus Zorn

#include <iostream>

#include <glad/glad.h>
#include <stb_image.h>

#include <Renderer/Renderer.h>

Renderer::Renderer(Window& window) : window{ &window } {}

Window& Renderer::get_window() {
	return *window;
}

void Renderer::clear(float r, float g, float b) {
	glClearColor(r, g, b, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
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
	//SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
	//textures[id] = texture;
	SDL_FreeSurface(surface);
	stbi_image_free(image_data);
}

void Renderer::load_font(uint16_t id, const uint8_t* data, uint32_t length) {
	if (fonts.size() <= id) {
		fonts.resize(id + 1);
	}
	fonts[id].init = true;
	fonts[id].buffer.assign(data, data + length);
	int offset = stbtt_GetFontOffsetForIndex(fonts[id].buffer.data(), 0);
	stbtt_InitFont(&fonts[id].info, fonts[id].buffer.data(), offset);
}

void Renderer::draw_sprite(uint16_t sprite, uint16_t x, uint16_t y) {
	if (sprite >= textures.size() || textures[sprite] == nullptr) {
		return;
	}
	return;
	int width, height;
	//SDL_QueryTexture(textures[sprite], nullptr, nullptr, &width, &height);
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
	//SDL_RenderCopy(renderer, textures[sprite], &src, &dst);
}

void Renderer::draw_text(uint16_t font, uint16_t x, uint16_t y, const char* text) {
	if (font >= fonts.size() || !fonts[font].init) {
		return;
	}
}
