// Copyright 2023 Justus Zorn

#include <glad/glad.h>

#include <Renderer/Window.h>

Window::Window() {
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		error(SDL_GetError());
	}
#ifdef ANOMALY_MOBILE
	SDL_SetHint(SDL_HINT_ORIENTATIONS, "LandscapeRight LandscapeLeft");
	window = SDL_CreateWindow("Anomaly", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 0, 0,
		SDL_WINDOW_RESIZABLE | SDL_WINDOW_FULLSCREEN | SDL_WINDOW_OPENGL);
#else
	window = SDL_CreateWindow("Anomaly", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720,
		SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);
#endif
	if (window == nullptr) {
		error(SDL_GetError());
	}
#ifdef ANOMALY_MOBILE
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
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
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
}

Window::~Window() {
	SDL_GL_DeleteContext(context);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

void Window::error(const std::string& message) {
	if (SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", message.c_str(), window) < 0) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "%s", message.c_str());
	}
	throw std::exception();
}

bool Window::update() {
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
		case SDL_QUIT:
			return false;
		case SDL_WINDOWEVENT:
			switch (event.window.event) {
			case SDL_WINDOWEVENT_RESIZED:
				int width, height;
				SDL_GL_GetDrawableSize(window, &width, &height);
				glViewport(0, 0, width, height);
				break;
			}
			break;
		case SDL_KEYDOWN:
			input.key_events.push_back({ event.key.keysym.sym, true });
			break;
		case SDL_KEYUP:
			input.key_events.push_back({ event.key.keysym.sym, false });
			break;
		case SDL_FINGERDOWN:
			input.touch_events.push_back({ (event.tfinger.x * 2.0f - 1.0f) * aspect_ratio(),
				-event.tfinger.y * 2.0f + 1.0f,
				static_cast<uint8_t>(event.tfinger.fingerId), true });
			break;
		case SDL_FINGERUP:
			input.touch_events.push_back({ (event.tfinger.x * 2.0f - 1.0f) * aspect_ratio(),
				-event.tfinger.y * 2.0f + 1.0f,
				static_cast<uint8_t>(event.tfinger.fingerId), false });
			break;
		case SDL_TEXTINPUT:
			input.textinput += event.text.text;
			break;
		}
	}
	return true;
}

void Window::present() {
	SDL_GL_SwapWindow(window);
}

float Window::aspect_ratio() const {
	int width, height;
	SDL_GL_GetDrawableSize(window, &width, &height);
	return static_cast<float>(width) / static_cast<float>(height);
}

ENetPacket* Window::create_input_packet() {
	return input.create_input_packet();
}

void Window::start_text_input() {
	SDL_StartTextInput();
}

void Window::stop_text_input() {
	SDL_StopTextInput();
}
