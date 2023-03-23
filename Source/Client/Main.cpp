// Copyright 2023 Justus Zorn

#include <enet.h>

#include <chrono>

#include <Anomaly.h>
#include <Audio/Audio.h>
#include <Client/typed_ttf.h>
#include <Client/Client.h>
#include <Renderer/Renderer.h>
#include <Renderer/Window.h>

std::string start_menu(Renderer& renderer) {
	Window& window = renderer.get_window();
	window.start_text_input();
	std::string hostname = window.input.composition;
	auto last_update = std::chrono::high_resolution_clock::now();
	while (true) {
		auto now = std::chrono::high_resolution_clock::now();
		double duration = std::chrono::duration_cast<std::chrono::microseconds>(now -
			last_update).count() / 1000000.0;
		if (duration >= MINIMUM_FRAME_TIME) {
			last_update = now;
			if (!window.update()) {
				throw std::exception();
			}
			for (const MouseEvent& e : window.input.mouse_events) {
				if (e.type == static_cast<uint8_t>(InputEventType::DOWN)) {
					window.start_text_input();
				}
			}
			for (const KeyEvent& e : window.input.key_events) {
				if (e.key == SDLK_RETURN && e.down == true) {
					window.stop_text_input();
					window.input.mouse_events.clear();
					window.input.key_events.clear();
					renderer.clear(0.0f, 0.0f, 0.0f);
					renderer.draw_string(0, 0.0f, 0.0f, 0.2f, 255, 255, 255, "Connecting...");
					window.present();
					return hostname;
				}
			}
			hostname = window.input.composition;
			window.input.mouse_events.clear();
			window.input.key_events.clear();
			renderer.clear(0.0f, 0.0f, 0.0f);
			renderer.draw_string(0, 0.0f, 0.8f, 0.2f, 255, 255, 255, "Connect to:");
			renderer.draw_string(0, 0.0f, 0.5f, 0.2f, 255, 255, 0, window.input.composition);
			window.present();
		}
	}
}

void run_client(Audio& audio, Renderer& renderer, const std::string& hostname) {
	auto last_update = std::chrono::high_resolution_clock::now();
	Client client(renderer.get_window());
	if (!client.connect(renderer.get_window(), hostname, 17899)) {
		return;
	}
	while (true) {
		auto now = std::chrono::high_resolution_clock::now();
		double duration = std::chrono::duration_cast<std::chrono::microseconds>(now -
			last_update).count() / 1000000.0;
		if (duration >= MINIMUM_FRAME_TIME) {
			last_update = now;
			if (!renderer.get_window().update()) throw std::exception();
			if (!client.update(audio, renderer)) break;
		}
	}
}

int SDL_main(int argc, char* argv[]) {
	try {
		Window window;
		Audio audio(window);
		Renderer renderer(window);
		renderer.load_font(0, typed_ttf, typed_ttf_length);
		while (true) {
			std::string hostname = start_menu(renderer);
			run_client(audio, renderer, hostname);
		}
	} catch (...) {}
	return 0;
}
