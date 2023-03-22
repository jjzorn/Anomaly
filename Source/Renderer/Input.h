// Copyright 2023 Justus Zorn

#ifndef ANOMALY_RENDERER_INPUT_H
#define ANOMALY_RENDERER_INPUT_H

#include <vector>

#include <enet.h>

struct KeyEvent {
	int32_t key;
	bool down;
};

struct MouseEvent {
	float x, y;
	uint8_t button;
	uint8_t type;
};

struct Input {
	std::vector<KeyEvent> key_events;
	std::string composition;
	std::vector<MouseEvent> mouse_events;

	bool changed_composition = false;

	ENetPacket* create_input_packet();
};

#endif
