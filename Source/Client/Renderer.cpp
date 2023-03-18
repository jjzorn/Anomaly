// Copyright 2023 Justus Zorn

#include <iostream>

#include <glad/glad.h>
#include <stb_image.h>

#include <Client/Renderer.h>

Renderer::Renderer() {
	SDL_Init(SDL_INIT_VIDEO);
#ifdef ANOMALY_MOBILE
	window = SDL_CreateWindow("Anomaly", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 0, 0,
		SDL_WINDOW_RESIZABLE | SDL_WINDOW_FULLSCREEN | SDL_WINDOW_OPENGL);
#else
	window = SDL_CreateWindow("Anomaly", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720,
		SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);
#endif
	if (!window) {
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", SDL_GetError(), window);
		exit(1);
	}
#ifdef ANOMALY_MOBILE
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
#else
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
#endif
	context = SDL_GL_CreateContext(window);
	if (!context) {
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", SDL_GetError(), window);
		exit(1);
	}
#ifdef ANOMALY_MOBILE
	if (!gladLoadGLES2Loader(SDL_GL_GetProcAddress)) {
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Could not initialize OpenGL", window);
		exit(1);
	}
#else
	if (!gladLoadGLLoader(SDL_GL_GetProcAddress)) {
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Could not initialize OpenGL", window);
		exit(1);
	}
#endif
}

Renderer::~Renderer() {
	SDL_GL_DeleteContext(context);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

void Renderer::error(const std::string& message) {
	if (SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", message.c_str(), window) < 0) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "%s", message.c_str());
	}
	throw std::exception();
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
	glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
}

void Renderer::present() {
	SDL_GL_SwapWindow(window);
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
