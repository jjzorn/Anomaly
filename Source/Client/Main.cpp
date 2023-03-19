// Copyright 2023 Justus Zorn

#include <enet.h>

#include <Client/Client.h>
#include <Renderer/Renderer.h>
#include <Renderer/Window.h>

int SDL_main(int argc, char* argv[]) {
	try {
		Window window;
		Renderer renderer(window);
		Client client("2003:dd:ff4f:ea00:19db:908a:2f79:8acc", 17899, window);
		while (window.update()) {
			if (!client.update(renderer)) break;
		}
	} catch (...) {}
	return 0;
}
