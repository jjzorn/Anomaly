// Copyright 2023 Justus Zorn

#ifndef ANOMALY_ANOMALY_H
#define ANOMALY_ANOMALY_H

#include <string>

enum class ContentType {
	IMAGE,
	FONT,
	SOUND
};

struct Sprite {
	bool is_text;
	uint32_t id;
	float x, y, scale;
	uint8_t r, g, b;
	std::string text;
};

struct Command {
	enum class Type {
		START_TEXT_INPUT,
		STOP_TEXT_INPUT
	} type;
};

struct AudioCommand {
	uint32_t id;
	uint16_t channel;
	uint8_t volume;
	enum class Type {
		PLAY,
		PLAY_ANY,
		STOP,
		STOP_ALL
	} type;
};

enum class InputEventType {
	UP,
	DOWN,
	MOTION
};

inline void write16(uint8_t* area, uint16_t i) {
	area[0] = (i & 0xFF00) >> 8;
	area[1] = (i & 0x00FF);
}

inline void write32(uint8_t* area, uint32_t i) {
	area[0] = (i & 0xFF000000) >> 24;
	area[1] = (i & 0x00FF0000) >> 16;
	area[2] = (i & 0x0000FF00) >> 8;
	area[3] = (i & 0x000000FF);
}

inline void write_float(uint8_t* area, float f) {
	uint32_t val;
	memcpy(&val, &f, sizeof(float));
	write32(area, val);
}

inline uint16_t read16(uint8_t* area) {
	return (area[0] << 8) | area[1];
}

inline uint32_t read32(uint8_t* area) {
	return (area[0] << 24) | (area[1] << 16) | (area[2] << 8) | area[3];
}

inline float read_float(uint8_t* area) {
	uint32_t val = read32(area);
	float result;
	memcpy(&result, &val, sizeof(float));
	return result;
}

constexpr uint64_t CONTENT_RELOAD = 1000;

constexpr uint16_t MAX_CLIENTS = 32;

constexpr uint16_t NET_CHANNELS = 5;
constexpr uint16_t INPUT_CHANNEL = 0;
constexpr uint16_t COMMAND_CHANNEL = 1;
constexpr uint16_t SPRITE_CHANNEL = 2;
constexpr uint16_t CONTENT_CHANNEL = 3;
constexpr uint16_t AUDIO_CHANNEL = 4;

constexpr uint16_t ANOMALY_AUDIO_CHANNELS = 16;

constexpr float FONT_PIXELS = 64.0f;
constexpr int FONT_SDF_PADDING = 10;
constexpr int FONT_SDF_SCALE = 20;

constexpr double MINIMUM_FRAME_TIME = 0.03;

#endif
