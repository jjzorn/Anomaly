// Copyright 2023 Justus Zorn

#include <enet.h>

#include <chrono>

#include <Anomaly.h>
#include <Client/Client.h>
#include <Renderer/Renderer.h>
#include <Renderer/Window.h>

int SDL_main(int argc, char* argv[]) {
	try {
		Window window;
		Renderer renderer(window);
		Client client("2003:dd:ff26:3800:3593:208:5beb:fb69", 17899, window);
		auto last_update = std::chrono::high_resolution_clock::now();
		while (true) {
			auto now = std::chrono::high_resolution_clock::now();
			double duration = std::chrono::duration_cast<std::chrono::microseconds>(now -
				last_update).count() / 1000000.0;
			if (duration >= MINIMUM_FRAME_TIME) {
				last_update = now;
				if (!window.update()) break;
				if (!client.update(renderer)) break;
			}
		}
	} catch (...) {}
	return 0;
}
