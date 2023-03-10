// Copyright 2023 Justus Zorn

#include <Client/Renderer.h>

#include <Common/Network.h>

#include <iostream>

int SDL_main(int argc, char* argv[]) {
	/*Renderer renderer;

	while (renderer.update()) {
		renderer.present();
	}
	*/

	Renderer renderer;
	Socket socket;
	if (!socket.connect("localhost", 17899)) {
		return 1;
	}

	socket.send_uint32(93);

	while (renderer.update()) {
		renderer.clear(255, 0, 0);
		renderer.present();
	}

	return 0;

	/*if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Could not initialize SDL");
		return 1;
	}
	SDL_Window* window = SDL_CreateWindow("My window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, 0);
	bool running = true;
	while (running) {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_QUIT:
				running = false;
				break;
			}
		}
	}
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;*/
}
