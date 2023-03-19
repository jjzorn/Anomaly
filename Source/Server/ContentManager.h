// Copyright 2023 Justus Zorn

#ifndef ANOMALY_SERVER_CONTENT_MANAGER_H
#define ANOMALY_SERVER_CONTENT_MANAGER_H

#include <filesystem>
#include <unordered_map>
#include <vector>

#include <stb_truetype.h>

#include <Server/Server.h>

class ContentManager {
public:
	void reload(Server& server);
	void init_client(Server& server, uint16_t client);

	uint16_t get_image_width(const std::filesystem::path& path) const;
	uint16_t get_image_height(const std::filesystem::path& path) const;

private:
	struct Image {
		std::vector<uint8_t> data;
		std::filesystem::file_time_type last_write;
		uint32_t id;
		uint16_t width;
		uint16_t height;
	};

	struct Font {
		std::vector<uint8_t> data;
		stbtt_fontinfo info;
		std::filesystem::file_time_type last_write;
		uint32_t id;
	};

	uint32_t image_id = 0;
	std::unordered_map<std::filesystem::path, Image> images;

	uint32_t font_id = 0;
	std::unordered_map<std::filesystem::path, Font> fonts;
};

#endif
