// Copyright 2023 Justus Zorn

#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

#include <Common/Network.h>

#include <Server/ContentManager.h>

int main(int argc, char* argv[]) {
	ServerSocket socket;
	socket.listen(17899);

	ContentManager content;

	while (true) {
		Socket client;
		if (socket.accept(client)) {
			content.reload(client);
		}
	}
}
