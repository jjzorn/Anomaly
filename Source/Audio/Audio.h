// Copyright 2023 Justus Zorn

#ifndef ANOMALY_AUDIO_AUDIO_H
#define ANOMALY_AUDIO_AUDIO_H

#include <SDL.h>

#include <Renderer/Window.h>

class Audio {
public:
	Audio(Window& window);
	Audio(const Audio&) = delete;
	~Audio();

	Audio& operator=(const Audio&) = delete;

private:
	float index = 0.0f;

	static void audio_callback(void* userdata, uint8_t* stream, int length);
};

#endif
