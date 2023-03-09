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
			// try sending a file
			std::string content = read_file("test.txt");
			uint8_t* ptr = reinterpret_cast<uint8_t*>(&content[0]);
			std::vector<uint8_t> data(ptr, ptr + content.size());
			client.send_vec(data);
		}
	}
}
