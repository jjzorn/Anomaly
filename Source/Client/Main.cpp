// Copyright 2023 Justus Zorn

#include <enet.h>

#include <Client/Client.h>
#include <Renderer/Renderer.h>
#include <Renderer/Window.h>

int SDL_main(int argc, char* argv[]) {
	try {
		Window window;
		Renderer renderer(window);
		Client client("127.0.0.1", 17899, window);
		while (window.update()) {
			if (!client.update(renderer)) break;
		}
	} catch (...) {}
	return 0;
}
