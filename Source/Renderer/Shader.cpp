// Copyright 2023 Justus Zorn

#include <Renderer/Renderer.h>
#include <Renderer/Shader.h>

static GLuint create_shader(Window& window, GLenum type, const std::string& source) {
#ifdef ANOMALY_MOBILE
	std::string shader_source = "#version 300 es\n" + source;
#else
	std::string shader_source = "#version 330 core\n" + source;
#endif
	const char* s = shader_source.c_str();

	GLuint shader = glCreateShader(type);
	glShaderSource(shader, 1, &s, nullptr);
	glCompileShader(shader);

	int success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		char log[512];
		glGetShaderInfoLog(shader, 512, nullptr, log);
		std::string message = "Shader compilation failed:\n";
		window.error(message + log);
	}

	return shader;
}

Shader::Shader(Window& window, const std::string& vertex, const std::string& fragment) {
	program = glCreateProgram();
	GLuint vsh = create_shader(window, GL_VERTEX_SHADER, vertex);
	GLuint fsh = create_shader(window, GL_FRAGMENT_SHADER, fragment);

	glAttachShader(program, vsh);
	glAttachShader(program, fsh);
	glLinkProgram(program);

	int success;
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (!success) {
		char log[512];
		glGetProgramInfoLog(program, 512, nullptr, log);
		std::string message = "Shader linking failed:\n";
		window.error(message + log);
	}

	glDeleteShader(vsh);
	glDeleteShader(fsh);
}

Shader::~Shader() {
	glDeleteProgram(program);
}

void Shader::use() const {
	glUseProgram(program);
}

GLint Shader::get_uniform_location(const char* name) {
	return glGetUniformLocation(program, name);
}

void Shader::set(GLint location, float x) {
	glUniform1f(location, x);
}

void Shader::set(GLint location, float x, float y) {
	glUniform2f(location, x, y);
}

void Shader::set(GLint location, float x, float y, float z) {
	glUniform3f(location, x, y, z);
}
