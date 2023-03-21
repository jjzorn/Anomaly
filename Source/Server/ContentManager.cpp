// Copyright 2023 Justus Zorn

#include <fstream>
#include <iostream>

#include <stb_image.h>

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
	try {
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
				int width, height;
				stbi_info_from_memory(image->data.data(), image->data.size(), &width, &height, nullptr);
				image->width = width;
				image->height = height;
				server.update_content(ContentType::IMAGE, image->id, image->data);
			}
		}
	} catch (...) {}
	try {
		for (auto entry : std::filesystem::recursive_directory_iterator("Content/Fonts")) {
			if (entry.is_regular_file()) {
				std::filesystem::path path = std::filesystem::canonical(entry.path());
				Font* font;
				auto it = fonts.find(path);
				if (it != fonts.end()) {
					font = &it->second;
					if (entry.last_write_time() == font->last_write) {
						continue;
					}
				}
				else {
					font = &fonts[path];
					font->id = font_id++;
				}
				font->last_write = entry.last_write_time();
				read_file(path, font->data);
				int offset = stbtt_GetFontOffsetForIndex(font->data.data(), 0);
				stbtt_InitFont(&font->info, font->data.data(), offset);
				server.update_content(ContentType::FONT, font->id, font->data);
			}
		}
	} catch (...) {}
}

void ContentManager::init_client(Server& server, uint16_t client) {
	for (auto it : images) {
		server.update_client_content(client, ContentType::IMAGE, it.second.id, it.second.data);
	}
	for (auto it : fonts) {
		server.update_client_content(client, ContentType::FONT, it.second.id, it.second.data);
	}
}

uint32_t ContentManager::get_image_id(const std::string& path) const {
	std::filesystem::path p = std::filesystem::canonical(path);
	auto it = images.find(p);
	if (it != images.end()) {
		return it->second.id;
	}
	return 0;
}

uint32_t ContentManager::get_font_id(const std::string& path) const {
	std::filesystem::path p = std::filesystem::canonical(path);
	auto it = fonts.find(p);
	if (it != fonts.end()) {
		return it->second.id;
	}
	return 0;
}
