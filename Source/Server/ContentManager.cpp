// Copyright 2023 Justus Zorn

#include <fstream>
#include <iostream>

#include <Server/ContentManager.h>

static void read_file(const std::filesystem::path& path, std::vector<uint8_t>& data) {
	std::ifstream input(path, std::ios::binary | std::ios::ate);
	if (!input.is_open()) {
		std::cerr << "ERROR: Could not read file '" << path << "'\n";
		return;
	}
	size_t length = input.tellg();
	input.seekg(0);
	data.resize(length);
	input.read(reinterpret_cast<char*>(data.data()), length);
}

void ContentManager::reload(Server& server) {
	std::cout << "INFO: Reloading content...\n";
	for (auto entry : std::filesystem::recursive_directory_iterator("Content/Images")) {
		if (entry.is_regular_file()) {
			std::filesystem::path path = std::filesystem::canonical(entry.path());
			Image* image;
			auto it = images.find(path);
			if (it != images.end()) {
				image = &it->second;
				if (entry.last_write_time() == image->last_write) {
					continue;
				}
			}
			else {
				image = &images[path];
				image->id = image_id++;
			}
			image->last_write = entry.last_write_time();
			read_file(path, image->data);
			server.update_image(image->id, image->data);
		}
	}
}

void ContentManager::init_client(Server& server, uint16_t client) {
	for (auto it : images) {
		server.update_client_image(client, it.second.id, it.second.data);
	}
}
