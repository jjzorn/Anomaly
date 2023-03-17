// Copyright 2023 Justus Zorn

#include <iostream>

#include <enet.h>

#include <Client/Client.h>
#include <Client/Renderer.h>

int SDL_main(int argc, char* argv[]) {
	Renderer renderer;

	if (enet_initialize() < 0) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Could not initialize ENet");
		return 1;
	}

	{
		Client client("::1", 17899);
		while (renderer.update()) {
			if (!client.update(renderer)) break;
			renderer.clear(255, 0, 255);
			renderer.draw_sprite(0, 600, 400);
			renderer.present();
		}
	}

	enet_deinitialize();

	return 0;
}
