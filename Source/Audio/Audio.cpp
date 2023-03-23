// Copyright 2023 Justus Zorn

#include <cmath>

#include <Audio/Audio.h>

#define SAMPLE_RATE 48000.0f
#define FREQ 400.0f

Audio::Audio(Window& window) {
	SDL_AudioSpec spec;
	spec.freq = SAMPLE_RATE;
	spec.format = AUDIO_S16SYS;
	spec.channels = 1;
	spec.samples = 1024;
	spec.callback = audio_callback;
	spec.userdata = this;

	if (SDL_OpenAudio(&spec, nullptr) != 0) {
		window.error(SDL_GetError());
		return;
	}
	
	SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Playing!");
	SDL_PauseAudio(0);
}

Audio::~Audio() {
	SDL_CloseAudio();
}

void Audio::audio_callback(void* userdata, uint8_t* stream, int length) {
	Audio* audio = reinterpret_cast<Audio*>(userdata);
	int req_samples = length / sizeof(uint16_t);
	uint16_t* samples = reinterpret_cast<uint16_t*>(stream);
	for (int i = 0; i < req_samples ; ++i) {
		samples[i] = INT16_MAX * sin(audio->index);
		audio->index += (FREQ * M_PI * 2) / SAMPLE_RATE;
		if (audio->index >= M_PI * 2) {
			audio->index -= M_PI * 2;
		}
	}
}
