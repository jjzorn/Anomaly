// Copyright 2023 Justus Zorn

#ifndef ANOMALY_RENDERER_RENDERER_H
#define ANOMALY_RENDERER_RENDERER_H

#include <unordered_map>
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
	void load_font(uint16_t id, const uint8_t* data, uint32_t length);

	void draw_sprite(uint16_t id, int16_t x, int16_t y, uint16_t scale);
	void draw_text(uint16_t id, int16_t x, int16_t y, uint16_t scale, const char* text);

private:
	Window* window;

	Shader sprite_shader;
	GLint sprite_shader_pos;
	GLint sprite_shader_scale;

	Shader font_shader;
	GLint font_shader_pos;
	GLint font_shader_scale;
	GLint font_shader_color;

	GLuint vao, vbo;

	struct Texture {
		GLuint texture;
		int width, height;
		bool init = false;
	};

	struct Glyph {
		GLuint texture;
		int width, height;
	};

	struct Font {
		std::vector<uint8_t> buffer;
		stbtt_fontinfo info;
		std::unordered_map<uint32_t, Glyph> glyphs;
		bool init = false;
	};

	std::vector<Texture> textures;
	std::vector<Font> fonts;

	Glyph& load_glyph(uint16_t id, uint32_t codepoint);
};

#endif
