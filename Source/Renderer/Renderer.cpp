// Copyright 2023 Justus Zorn

#include <glad/glad.h>
#include <stb_image.h>

#include <Anomaly.h>
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
"const float width = 0.52;\n"
"void main() {\n"
"float distance = texture(sdf, texcoords).r;\n"
"float aaf = fwidth(distance);\n"
"float opacity = smoothstep(width - aaf, width + aaf, distance);\n"
"out_color = vec4(color, opacity);\n"
"}\n";

float quad[] = {
	0.0f, 0.0f, 0.0f, 1.0f,
	1.0f, 0.0f, 1.0f, 1.0f,
	0.0f, 1.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f, 0.0f
};

uint8_t missing_texture_data[] = {
	255, 0, 255,
	0, 0, 0,
	0, 0, 0,
	255, 0, 255
};

Renderer::Renderer(Window& window)
	: window{ &window }, sprite_shader(window, vsh, sprite_fsh), font_shader(window, vsh, font_fsh) {
	sprite_shader_pos = sprite_shader.get_uniform_location("pos");
	sprite_shader_scale = sprite_shader.get_uniform_location("scale");

	font_shader_pos = font_shader.get_uniform_location("pos");
	font_shader_scale = font_shader.get_uniform_location("scale");
	font_shader_color = font_shader.get_uniform_location("color");

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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

	glGenTextures(1, &missing_texture);
	glBindTexture(GL_TEXTURE_2D, missing_texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 2, 2, 0, GL_RGB, GL_UNSIGNED_BYTE, missing_texture_data);

	glBindTexture(GL_TEXTURE_2D, 0);
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

void Renderer::load_image(uint32_t id, const uint8_t* data, uint32_t length) {
	if (textures.size() <= id) {
		textures.resize(id + 1);
	}
	if (textures[id].init && textures[id].texture != missing_texture) {
		glDeleteTextures(1, &textures[id].texture);
	}
	textures[id].init = true;
	int width, height;
	uint8_t* image_data = stbi_load_from_memory(data, length, &width, &height, nullptr, 4);
	if (image_data == nullptr) {
		textures[id].texture = missing_texture;
		textures[id].width = 2;
		textures[id].height = 2;
		return;
	}
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

void Renderer::load_font(uint32_t id, const uint8_t* data, uint32_t length) {
	if (fonts.size() <= id) {
		fonts.resize(id + 1);
	}
	fonts[id].buffer.assign(data, data + length);
	int offset = stbtt_GetFontOffsetForIndex(fonts[id].buffer.data(), 0);
	if (stbtt_InitFont(&fonts[id].info, fonts[id].buffer.data(), offset)) {
		fonts[id].init = true;
		float scale = stbtt_ScaleForPixelHeight(&fonts[id].info, FONT_PIXELS);
		int ascent, descent;
		stbtt_GetFontVMetrics(&fonts[id].info, &ascent, &descent, nullptr);
		fonts[id].offset = scale * (ascent + descent) / 2.0f;
	}
}

void Renderer::draw_sprite(uint32_t id, float x, float y, float scale) {
	if (id >= textures.size() || !textures[id].init) {
		return;
	}

	float window_aspect_ratio = window->aspect_ratio();
	float texture_aspect_ratio = static_cast<float>(textures[id].width) / static_cast<float>(textures[id].height);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textures[id].texture);

	float xscale = scale / window_aspect_ratio * texture_aspect_ratio;
	x /= window_aspect_ratio;

	sprite_shader.use();
	sprite_shader.set(sprite_shader_pos, x - xscale / 2.0f, y - scale / 2.0f);
	sprite_shader.set(sprite_shader_scale, xscale, scale);

	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);

	glBindTexture(GL_TEXTURE_2D, 0);
	glUseProgram(0);
}

void Renderer::draw_text(uint32_t id, float x, float y, float scale, uint8_t r, uint8_t g,
	uint8_t b, const uint8_t* text, uint32_t length) {
	if (id >= fonts.size() || !fonts[id].init) {
		return;
	}

	float window_aspect_ratio = window->aspect_ratio();

	float yscale = scale / FONT_PIXELS;
	float xscale = yscale / window_aspect_ratio;

	x /= window_aspect_ratio;
	
	float baseline = y - yscale * fonts[id].offset;

	font_shader.use();
	font_shader.set(font_shader_color, r / 255.0f, g / 255.0f, b / 255.0f);

	for (uint32_t i = 0; i < length; ++i) {
		uint32_t codepoint = text[i];
		Glyph& glyph = load_glyph(id, codepoint);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, glyph.texture);

		float left = x + glyph.left * xscale;
		float bottom = baseline + glyph.bottom * yscale;

		font_shader.set(font_shader_pos, left, bottom);
		font_shader.set(font_shader_scale, glyph.width * xscale, glyph.height * yscale);

		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);

		glBindTexture(GL_TEXTURE_2D, 0);

		x += glyph.advance * xscale;
	}

	glUseProgram(0);
}

Renderer::Glyph& Renderer::load_glyph(uint32_t id, uint32_t codepoint) {
	Font& font = fonts[id];
	auto it = font.glyphs.find(codepoint);
	if (it != font.glyphs.end()) {
		return it->second;
	}
	float scale = stbtt_ScaleForPixelHeight(&font.info, FONT_PIXELS);

	int advance, lsb;
	stbtt_GetCodepointHMetrics(&font.info, codepoint, &advance, &lsb);
	int x0, y0, x1, y1;
	stbtt_GetCodepointBox(&font.info, codepoint, &x0, &y0, &x1, &y1);

	Glyph& glyph = font.glyphs[codepoint];
	glyph.left = x0 * scale - FONT_SDF_PADDING;
	glyph.bottom = y0 * scale - FONT_SDF_PADDING;
	glyph.width = (x1 - x0) * scale + 2 * FONT_SDF_PADDING;
	glyph.height = (y1 - y0) * scale + 2 * FONT_SDF_PADDING;
	glyph.advance = advance * scale;

	int width, height;
	uint8_t* sdf = stbtt_GetCodepointSDF(&font.info, scale, codepoint, FONT_SDF_PADDING, 128, FONT_SDF_SCALE, &width, &height, nullptr, nullptr);
	if (sdf == nullptr) {
		glyph.texture = 0;
		return glyph;
	}

	glGenTextures(1, &glyph.texture);
	glBindTexture(GL_TEXTURE_2D, glyph.texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, sdf);
	
	glBindTexture(GL_TEXTURE_2D, 0);

	stbtt_FreeSDF(sdf, nullptr);

	return glyph;
}
