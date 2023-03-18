// Copyright 2023 Justus Zorn

#include <glad/glad.h>
#include <stb_image.h>

#include <Renderer/Renderer.h>

std::string vsh =
"layout (location = 0) in vec2 in_pos;\n"
"layout (location = 1) in vec2 in_texcoords;\n"
"out vec2 texcoords;\n"
"uniform vec2 pos;\n"
"uniform vec2 scale;\n"
"void main() {\n"
"texcoords = in_texcoords;\n"
"gl_Position = vec4(in_pos.x * scale.x + pos.x, in_pos.y * scale.y + pos.y, 0.0, 1.0);\n"
"}\n";
std::string sprite_fsh =
"in vec2 texcoords;\n"
"out vec4 out_color;\n"
"uniform sampler2D sprite;\n"
"void main() {\n"
"out_color = texture(sprite, texcoords);\n"
"}\n";

std::string font_fsh =
"in vec2 texcoords;\n"
"out vec4 out_color;\n"
"uniform sampler2D sdf;\n"
"uniform vec3 color;\n"
"void main() {\n"
"float distance = texture(sdf, texcoords).r;\n"
"if (distance < 0.5) {\n"
"discard;\n"
"} else {\n"
"out_color = vec4(color, 1.0);\n"
"}\n"
"}\n";

float quad[] = {
	-1.0f, -1.0f, 0.0f, 1.0f,
	1.0f, -1.0f, 1.0f, 1.0f,
	-1.0f, 1.0f, 0.0f, 0.0f,
	-1.0f, 1.0f, 0.0f, 0.0f,
	1.0f, -1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f, 0.0f
};

Renderer::Renderer(Window& window)
	: window{ &window }, sprite_shader(window, vsh, sprite_fsh), font_shader(window, vsh, font_fsh) {
	sprite_shader_pos = sprite_shader.get_uniform_location("pos");
	sprite_shader_scale = sprite_shader.get_uniform_location("scale");

	font_shader_pos = font_shader.get_uniform_location("pos");
	font_shader_scale = font_shader.get_uniform_location("scale");
	font_shader_color = font_shader.get_uniform_location("color");

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW);
	
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), reinterpret_cast<void*>(0));
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), reinterpret_cast<void*>(2 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

Renderer::~Renderer() {
	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);
}

Window& Renderer::get_window() {
	return *window;
}

void Renderer::clear(float r, float g, float b) {
	glClearColor(r, g, b, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
}

void Renderer::present() {
	window->present();
}

void Renderer::load_image(uint16_t id, const uint8_t* data, uint32_t length) {
	if (textures.size() <= id) {
		textures.resize(id + 1);
	}
	if (textures[id].init) {
		glDeleteTextures(1, &textures[id].texture);
	}
	int width, height;
	uint8_t* image_data = stbi_load_from_memory(data, length, &width, &height, nullptr, 4);
	if (image_data == nullptr) {
		// TODO: Use a replacement texture
		return;
	}
	textures[id].init = true;
	textures[id].width = width;
	textures[id].height = height;

	GLuint texture;

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);

	glBindTexture(GL_TEXTURE_2D, 0);
	textures[id].texture = texture;
	stbi_image_free(image_data);
}

void Renderer::load_font(uint16_t id, const uint8_t* data, uint32_t length) {
	if (fonts.size() <= id) {
		fonts.resize(id + 1);
	}
	fonts[id].buffer.assign(data, data + length);
	int offset = stbtt_GetFontOffsetForIndex(fonts[id].buffer.data(), 0);
	if (stbtt_InitFont(&fonts[id].info, fonts[id].buffer.data(), offset)) {
		fonts[id].init = true;
	}
}

void Renderer::draw_sprite(uint16_t id, int16_t x, int16_t y, uint16_t scale) {
	if (id >= textures.size() || !textures[id].init) {
		return;
	}

	float window_aspect_ratio = static_cast<float>(window->width()) / static_cast<float>(window->height());
	float texture_aspect_ratio = static_cast<float>(textures[id].width) / static_cast<float>(textures[id].height);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textures[id].texture);

	sprite_shader.use();
	sprite_shader.set(sprite_shader_pos, x / 10000.0f / window_aspect_ratio, y / 10000.0f);
	sprite_shader.set(sprite_shader_scale, scale / 20000.0f / window_aspect_ratio * texture_aspect_ratio, scale / 20000.0f);

	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);

	glBindTexture(GL_TEXTURE_2D, 0);
	glUseProgram(0);
}

void Renderer::draw_text(uint16_t id, int16_t x, int16_t y, uint16_t scale, const char* text) {
	if (id >= fonts.size() || !fonts[id].init) {
		return;
	}

	Glyph& glyph = load_glyph(id, 'm');

	float window_aspect_ratio = static_cast<float>(window->width()) / static_cast<float>(window->height());
	float texture_aspect_ratio = static_cast<float>(glyph.width) / static_cast<float>(glyph.height);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, glyph.texture);

	/*sprite_shader.use();
	sprite_shader.set(sprite_shader_pos, x / 10000.0f / window_aspect_ratio, y / 10000.0f);
	sprite_shader.set(sprite_shader_scale, scale / 20000.0f / window_aspect_ratio * texture_aspect_ratio, scale / 20000.0f);*/

	font_shader.use();
	font_shader.set(font_shader_pos, x / 10000.0f / window_aspect_ratio, y / 10000.0f);
	font_shader.set(font_shader_scale, scale / 20000.0f / window_aspect_ratio * texture_aspect_ratio, scale / 20000.0f);
	font_shader.set(font_shader_color, 1.0f, 0.0f, 0.0f);

	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);

	glBindTexture(GL_TEXTURE_2D, 0);
	glUseProgram(0);
}

Renderer::Glyph& Renderer::load_glyph(uint16_t id, uint32_t codepoint) {
	Font& font = fonts[id];
	auto it = font.glyphs.find(codepoint);
	if (it != font.glyphs.end()) {
		return it->second;
	}
	float scale = stbtt_ScaleForPixelHeight(&font.info, 32);
	int width, height;
	uint8_t* sdf = stbtt_GetCodepointSDF(&font.info, scale, codepoint, 0, 128, 10, &width, &height, nullptr, nullptr);

	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, sdf);
	
	glBindTexture(GL_TEXTURE_2D, 0);

	Glyph& glyph = font.glyphs[codepoint];
	glyph.texture = texture;
	glyph.width = width;
	glyph.height = height;

	stbtt_FreeSDF(sdf, nullptr);

	SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Glyph dimensions: %dx%d", glyph.width, glyph.height);

	return glyph;
}
