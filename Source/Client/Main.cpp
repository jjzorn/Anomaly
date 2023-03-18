// Copyright 2023 Justus Zorn

#include <iostream>

#include <enet.h>

#include <Client/Client.h>
#include <Renderer/Renderer.h>
#include <Renderer/Window.h>

int SDL_main(int argc, char* argv[]) {
	try {
		Window window;
		Renderer renderer(window);
		//Client client("::1", 17899);
		while (window.update()) {
			renderer.clear(0.0f, 1.0f, 0.0f);
			window.present();
		}
	} catch (...) {}
	return 0;
}
