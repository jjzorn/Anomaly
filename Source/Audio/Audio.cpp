// Copyright 2023 Justus Zorn

#include <cmath>

#include <Audio/Audio.h>

#define FREQ 44100.0f
#define SAMPLES 8192
#define CHANNELS 2

Audio::Audio(Window& window) {
	SDL_AudioSpec spec;
	spec.freq = FREQ;
	spec.format = AUDIO_S16SYS;
	spec.channels = CHANNELS;
	spec.samples = SAMPLES;
	spec.callback = audio_callback_helper;
	spec.userdata = this;
	
	channels.resize(ANOMALY_AUDIO_CHANNELS);

	device = SDL_OpenAudioDevice(nullptr, 0, &spec, nullptr, 0);
	if (device == 0) {
		window.error(SDL_GetError());
		return;
	}

	SDL_PauseAudioDevice(device, 0);
}

Audio::~Audio() {
	SDL_CloseAudioDevice(device);
}

void Audio::load_sound(uint32_t id, const uint8_t* data, uint32_t length) {
	if (sounds.size() <= id) {
		SDL_LockAudioDevice(device);
		sounds.resize(id + 1);
		SDL_UnlockAudioDevice(device);
	}
	Sound& sound = sounds[id];
	if (sound.samples != nullptr) {
		delete[] sound.samples;
		sound.samples = nullptr;
	}

	stb_vorbis* vorbis = stb_vorbis_open_memory(data, length, nullptr, nullptr);
	if (vorbis == nullptr) {
		return;
	}
	stb_vorbis_info info = stb_vorbis_get_info(vorbis);
	sound.length = stb_vorbis_stream_length_in_samples(vorbis) * info.channels;
	sound.samples = new int16_t[sound.length];

	stb_vorbis_get_samples_short_interleaved(vorbis, CHANNELS, sound.samples, sound.length);
	stb_vorbis_close(vorbis);
}

void Audio::perform_command(uint32_t id, uint16_t channel, uint8_t volume, AudioCommand::Type type) {
	SDL_LockAudioDevice(device);
	switch (type) {
	case AudioCommand::Type::PLAY:
		if (id < sounds.size() && sounds[id].samples != nullptr && channel < ANOMALY_AUDIO_CHANNELS / 2) {
			channels[channel].sound_id = id;
			channels[channel].index = 0;
			channels[channel].volume = volume;
		}
		break;
	case AudioCommand::Type::PLAY_ANY:
		if (id < sounds.size() && sounds[id].samples != nullptr) {
			for (uint32_t i = ANOMALY_AUDIO_CHANNELS / 2; i < ANOMALY_AUDIO_CHANNELS; ++i) {
				if (channels[i].sound_id == 0) {
					channels[i].sound_id = id;
					channels[i].index = 0;
					channels[i].volume = volume;
					break;
				}
			}
		}
		break;
	case AudioCommand::Type::STOP:
		channels[channel].sound_id = 0;
		channels[channel].index = 0;
		break;
	case AudioCommand::Type::STOP_ALL:
		for (uint32_t i = 0; i < ANOMALY_AUDIO_CHANNELS; ++i) {
			channels[i].sound_id = 0;
			channels[i].index = 0;
		}
		break;
	}
	SDL_UnlockAudioDevice(device);
}

void Audio::stop_all() {
	SDL_LockAudioDevice(device);
	for (uint32_t i = 0; i < ANOMALY_AUDIO_CHANNELS; ++i) {
		channels[i].sound_id = 0;
		channels[i].index = 0;
	}
	SDL_UnlockAudioDevice(device);
}

void Audio::audio_callback_helper(void* userdata, uint8_t* stream, int length) {
	Audio* audio = reinterpret_cast<Audio*>(userdata);
	int16_t* samples = reinterpret_cast<int16_t*>(stream);
	int sample_count = length / sizeof(int16_t);
	audio->audio_callback(samples, sample_count);
}

void Audio::audio_callback(int16_t* samples, uint32_t length) {
	for (uint32_t i = 0; i < length; ++i) {
		int32_t acc = 0;
		for (uint32_t c = 0; c < ANOMALY_AUDIO_CHANNELS; ++c) {
			Channel& channel = channels[c];
			if (channel.sound_id != 0) {
				Sound& sound = sounds[channel.sound_id];
				if (channel.index < sound.length) {
					acc += sound.samples[channel.index++] * (channel.volume / 128.0f);
				}
				else {
					channel.sound_id = 0;
					channel.index = 0;
				}
			}
		}
		if (acc > INT16_MAX) {
			acc = INT16_MAX;
		}
		if (acc < INT16_MIN) {
			acc = INT16_MIN;
		}
		samples[i] = static_cast<int16_t>(acc);
	}
}
