// Copyright 2023 Justus Zorn

#include <Common/Network.h>

#include <SDL.h>

int SDL_main(int argc, char* argv[]) {
	SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "netlib init: %d\n", netlib_init());

	ip_address addr;

	Socket socket;
	if (socket.connect("localhost", 17899)) {
		socket.send("hello!", 6);
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
