// Copyright 2023 Justus Zorn

#include <fstream>
#include <iostream>

#include <Server/ContentManager.h>

static std::vector<uint8_t> read_file(const std::filesystem::path& path) {
	std::vector<uint8_t> content;
	std::ifstream input(path, std::ios::binary | std::ios::ate);
	if (!input.is_open()) {
		std::cerr << "ERROR: Could not read file '" << path << "'\n";
		return content;
	}
	size_t length = input.tellg();
	input.seekg(0);
	content.resize(length);
	input.read(reinterpret_cast<char*>(content.data()), length);
	return content;
}

void ContentManager::reload(Socket& client) {
	for (auto entry : std::filesystem::recursive_directory_iterator("Content/Images")) {
		if (entry.is_regular_file()) {
			std::filesystem::path path = std::filesystem::canonical(entry.path());
			if (images.find(path) != images.end()) {
				Image& image = images[path];
				if (entry.last_write_time() == image.last_write) {
					continue;
				}
			}
			Image image;
			image.id = image_id++;
			image.data = read_file(path);
			images[path] = image;
			client.send_uint32(static_cast<uint32_t>(MessageType::SERVER_CONTENT_UPDATE_IMAGE));
			client.send_uint32(image.id);
			client.send_vec(image.data);
		}
	}
}
