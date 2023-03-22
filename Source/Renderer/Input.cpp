// Copyright 2023 Justus Zorn

#include <Anomaly.h>
#include <Renderer/Input.h>

ENetPacket* Input::create_input_packet() {
	if (key_events.size() == 0 && mouse_events.size() == 0 && !changed_composition) {
		return nullptr;
	}
	changed_composition = false;
	uint32_t length = 12 + 5 * key_events.size() + composition.length() + 10 * mouse_events.size();
	ENetPacket* packet = enet_packet_create(nullptr, length, ENET_PACKET_FLAG_RELIABLE);
	uint8_t* data = packet->data;
	write32(data, static_cast<uint32_t>(key_events.size()));
	data += 4;
	for (const KeyEvent& e : key_events) {
		write32(data, e.key);
		data[4] = e.down;
		data += 5;
	}
	write32(data, static_cast<uint32_t>(composition.length()));
	data += 4;
	memcpy(data, composition.data(), composition.length());
	data += composition.length();
	write32(data, static_cast<uint32_t>(mouse_events.size()));
	data += 4;
	for (const MouseEvent& e : mouse_events) {
		write_float(data, e.x);
		write_float(data + 4, e.y);
		data[8] = e.button;
		data[9] = e.type;
		data += 10;
	}
	key_events.clear();
	mouse_events.clear();
	return packet;
}
