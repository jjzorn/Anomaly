// Copyright 2023 Justus Zorn

#ifndef ANOMALY_ANOMALY_H
#define ANOMALY_ANOMALY_H

#include <cstdint>

enum class PacketType {
	IMAGE
};

inline void write32(uint8_t* area, uint32_t i) {
	area[0] = (i & 0xFF000000) >> 24;
	area[1] = (i & 0x00FF0000) >> 16;
	area[2] = (i & 0x0000FF00) >> 8;
	area[3] = (i & 0x000000FF);
}

inline uint32_t read32(uint8_t* area) {
	return (area[0] << 24) | (area[1] << 16) | (area[2] << 8) | area[3];
}

constexpr uint16_t MAX_CLIENTS = 64;
constexpr uint16_t NET_CHANNELS = 4;
constexpr uint16_t CONTENT_CHANNEL = 3;

#endif
