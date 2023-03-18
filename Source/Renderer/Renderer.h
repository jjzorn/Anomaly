// Copyright 2023 Justus Zorn

#ifndef ANOMALY_RENDERER_RENDERER_H
#define ANOMALY_RENDERER_RENDERER_H

#include <vector>

#include <stb_truetype.h>
#include <SDL.h>

#include <Renderer/Shader.h>
#include <Renderer/Window.h>

class Renderer {
public:
	Renderer(Window& window);
	Renderer(const Renderer&) = delete;
	~Renderer();

	Renderer& operator=(const Renderer&) = delete;

	Window& get_window();

	void clear(float r, float g, float b);
	void present();

	void load_image(uint16_t id, const uint8_t* data, uint32_t length);
	//void load_font(uint16_t id, const uint8_t* data, uint32_t length);

	void draw_sprite(uint16_t id, int16_t x, int16_t y, uint16_t scale);
	//void draw_text(uint16_t font, uint16_t x, uint16_t y, const char* text);

private:
	Window* window;
	Shader sprite_shader;
	GLint sprite_shader_pos;
	GLint sprite_shader_scale;

	GLuint vao, vbo;

	struct Texture {
		GLuint texture;
		int width, height;
		bool init = false;
	};

	struct Font {
		std::vector<uint8_t> buffer;
		stbtt_fontinfo info;
		bool init = false;
	};

	std::vector<Texture> textures;
	std::vector<Font> fonts;
};

#endif
