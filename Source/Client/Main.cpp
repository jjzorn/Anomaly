// Copyright 2023 Justus Zorn

#include <iostream>

#include <enet.h>

#include <Client/Client.h>
#include <Client/Renderer.h>

int SDL_main(int argc, char* argv[]) {
	try {
		Renderer renderer;
		//Client client("::1", 17899);
		while (renderer.update()) {
			//if (!client.update(renderer)) break;
			renderer.clear(0, 0, 0);
			renderer.present();
		}
	} catch (...) {}
	return 0;
}
