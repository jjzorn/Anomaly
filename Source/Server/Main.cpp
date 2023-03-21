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
		uint64_t duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_update).count();
		if (duration >= 33) {
			last_update = now;
			server.update(script);
		}
	}

	enet_deinitialize();
}
