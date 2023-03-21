// Copyright 2023 Justus Zorn

#include <iostream>

#include <Anomaly.h>
#include <Renderer/Input.h>

ENetPacket* Input::create_input_packet() {
	if (key_events.size() == 0 && touch_events.size() == 0) {
		return nullptr;
	}
	uint32_t length = 12 + textinput.length() + 5 * key_events.size() + 10 * touch_events.size();
	ENetPacket* packet = enet_packet_create(nullptr, length, ENET_PACKET_FLAG_RELIABLE);
	uint8_t* data = packet->data;
	write32(data, static_cast<uint32_t>(textinput.length()));
	data += 4;
	memcpy(data, textinput.data(), textinput.length());
	data += textinput.length();
	write32(data, static_cast<uint32_t>(key_events.size()));
	data += 4;
	for (const KeyEvent& e : key_events) {
		write32(data, e.key);
		data[4] = e.down;
		data += 5;
	}
	write32(data, static_cast<uint32_t>(touch_events.size()));
	data += 4;
	for (const TouchEvent& e : touch_events) {
		write_float(data, e.x);
		write_float(data + 4, e.y);
		data[8] = e.finger;
		data[9] = e.down;
		data += 9;
	}
	textinput.clear();
	key_events.clear();
	touch_events.clear();
	return packet;
}
