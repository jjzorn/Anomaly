// Copyright 2023 Justus Zorn

#ifndef ANOMALY_RENDERER_INPUT_H
#define ANOMALY_RENDERER_INPUT_H

#include <vector>

#include <enet.h>

struct KeyEvent {
	int32_t key;
	bool down;
};

struct TouchEvent {
	float x, y;
	uint8_t finger;
	bool down;
};

struct Input {
	std::vector<KeyEvent> key_events;
	std::vector<TouchEvent> touch_events;

	ENetPacket* create_input_packet();
};

#endif
