// Copyright 2023 Justus Zorn

#include <chrono>
#include <iostream>

#include <Server/ContentManager.h>
#include <Server/Script.h>
#include <Server/Server.h>

int main(int argc, char* argv[]) {
	if (enet_initialize() < 0) {
		std::cerr << "ERROR: Could not initialize ENet\n";
		return 1;
	}

	ContentManager content;
	Server server(content, 17899);
	content.reload(server);
	Script script(server);

	auto last_update = std::chrono::high_resolution_clock::now();
	while (true) {
		auto now = std::chrono::high_resolution_clock::now();
		double duration = std::chrono::duration_cast<std::chrono::microseconds>(now -
			last_update).count() / 1000000.0;
		if (duration >= MINIMUM_FRAME_TIME) {
			last_update = now;
			server.update(script, duration);
			if (script.check_reload()) {
				content.reload(server);
			}
		}
	}

	enet_deinitialize();
}
