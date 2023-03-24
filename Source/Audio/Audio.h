// Copyright 2023 Justus Zorn

#ifndef ANOMALY_AUDIO_AUDIO_H
#define ANOMALY_AUDIO_AUDIO_H

#include <vector>

#include <SDL.h>
#include <stb_vorbis.h>

#include <Anomaly.h>
#include <Renderer/Window.h>

class Audio {
public:
	Audio(Window& window);
	Audio(const Audio&) = delete;
	~Audio();

	Audio& operator=(const Audio&) = delete;

	void load_sound(uint32_t id, const uint8_t* data, uint32_t length);
	void perform_command(uint32_t id, uint16_t channel, uint8_t volume, AudioCommand::Type type);

	void stop_all();

private:
	struct Sound {
		int16_t* samples;
		uint32_t length;
	};

	struct Channel {
		uint32_t sound_id = 0;
		uint32_t index = 0;
		uint8_t volume;
	};

	SDL_AudioDeviceID device;

	std::vector<Sound> sounds;
	std::vector<Channel> channels;

	size_t index = 0;

	static void audio_callback_helper(void* userdata, uint8_t* stream, int length);
	void audio_callback(int16_t* samples, uint32_t length);
};

#endif
