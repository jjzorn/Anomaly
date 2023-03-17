// Copyright 2023 Justus Zorn

#ifndef ANOMALY_SERVER_CONTENT_MANAGER_H
#define ANOMALY_SERVER_CONTENT_MANAGER_H

#include <filesystem>
#include <unordered_map>
#include <vector>

#include <Server/Server.h>

class ContentManager {
public:
	void reload(Server& server);
	void init_client(Server& server, uint16_t client);

private:
	struct Image {
		std::vector<uint8_t> data;
		uint32_t id;
		std::filesystem::file_time_type last_write;
	};

	uint32_t image_id = 0;
	std::unordered_map<std::filesystem::path, Image> images;
};

#endif