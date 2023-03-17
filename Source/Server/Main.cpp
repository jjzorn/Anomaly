// Copyright 2023 Justus Zorn

#include <iostream>

#include <Server/ContentManager.h>
#include <Server/Server.h>

int main(int argc, char* argv[]) {
	if (enet_initialize() < 0) {
		std::cerr << "ERROR: Could not initialize ENet\n";
		return 1;
	}

	ContentManager content;
	Server server(17899);
	content.reload(server);

	while (true) {
		server.update(content);
	}

	enet_deinitialize();
}
