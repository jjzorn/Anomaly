// Copyright 2023 Justus Zorn

#include <Common/Network.h>

#include <iostream>

int main(int argc, char* argv[]) {
	ServerSocket socket;
	socket.listen(17899);

	Socket client;
	while (true) {
		if (socket.accept(client)) {
			char buffer[256];
			while (true) {
				int len = client.recv(buffer, 256);
				if (len > 0) {
					std::cout << "CLIENT: " << std::string(buffer, len) << '\n';
					break;
				}
			}
		}
	}
}
