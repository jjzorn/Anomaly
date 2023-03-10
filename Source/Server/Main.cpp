// Copyright 2023 Justus Zorn

#include <Common/Network.h>

#include <fstream>
#include <iostream>
#include <string>

static std::string read_file(const std::string& path) {
	std::ifstream input(path, std::ios::binary);
	if (!input.is_open()) {
		std::cerr << "ERROR: Could not read file '" << path << "'\n";
		return "";
	}
	std::string content;
	std::string line;
	while (input.good()) {
		std::getline(input, line);
		content += line;
		content += '\n';
	}
	return content;
}

int main(int argc, char* argv[]) {
	ServerSocket socket;
	socket.listen(17899);

	Socket client;
	while (true) {
		if (socket.accept(client)) {
			uint32_t num;
			if (client.recv_uint32(num)) {
				std::cout << "Client sent number " << num << '\n';
			}
		}
	}
}
