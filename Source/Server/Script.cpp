// Copyright 2023 Justus Zorn

#include <iostream>
#include <unordered_map>

#include <Server/Script.h>
#include <Server/Server.h>

std::unordered_map<int32_t, const char*> keycodes = {
	{ 8, "Backspace" },
	{ 9, "Tab" },
	{ 13, "Return" },
	{ 27, "Escape" },
	{ 32, "Space" },
	{ 48, "0" },
	{ 49, "1" },
	{ 50, "2" },
	{ 51, "3" },
	{ 52, "4" },
	{ 53, "5" },
	{ 54, "6" },
	{ 55, "7" },
	{ 56, "8" },
	{ 57, "9" },
	{ 97, "A" },
	{ 98, "B" },
	{ 99, "C" },
	{ 100, "D" },
	{ 101, "E" },
	{ 102, "F" },
	{ 103, "G" },
	{ 104, "H" },
	{ 105, "I" },
	{ 106, "J" },
	{ 107, "K" },
	{ 108, "L" },
	{ 109, "M" },
	{ 110, "N" },
	{ 111, "O" },
	{ 112, "P" },
	{ 113, "Q" },
	{ 114, "R" },
	{ 115, "S" },
	{ 116, "T" },
	{ 117, "U" },
	{ 118, "V" },
	{ 119, "W" },
	{ 120, "X" },
	{ 121, "Y" },
	{ 122, "Z" },
	{ 1073741882, "F1" },
	{ 1073741883, "F2" },
	{ 1073741884, "F3" },
	{ 1073741885, "F4" },
	{ 1073741886, "F5" },
	{ 1073741887, "F6" },
	{ 1073741888, "F7" },
	{ 1073741889, "F8" },
	{ 1073741890, "F9" },
	{ 1073741891, "F10" },
	{ 1073741892, "F11" },
	{ 1073741893, "F12" },
	{ 1073742048, "Left Ctrl" },
	{ 1073742049, "Left Shift" },
	{ 1073742052, "Right Ctrl" },
	{ 1073742053, "Right Shift" },
	{ 1073742094, "Back" }
};

Script::Script(Server& server) : server{ &server } {
	L = luaL_newstate();
	if (!L) {
		std::cerr << "ERROR: Could not initialize Lua\n";
		return;
	}
	luaL_openlibs(L);
	reload();
}

Script::~Script() {
	lua_close(L);
}

void Script::reload() {
	luaL_dostring(L, "package.path = 'Content/Scripts/?.lua'");
	register_callback("reload", lua_reload);
	register_callback("start_text_input", start_text_input);
	register_callback("stop_text_input", stop_text_input);
	register_callback("get_composition", get_composition);
	register_callback("get_sprite_width", get_sprite_width);
	register_callback("draw_sprite", draw_sprite);
	register_callback("draw_text", draw_text);
	register_callback("kick", kick);
	register_callback("play_sound", play_sound);
	register_callback("stop_sound", stop_sound);
	register_callback("stop_all_sounds", stop_all_sounds);
	if (luaL_dofile(L, "Content/Scripts/main.lua") != LUA_OK) {
		std::cerr << "ERROR: Could not load lua file 'Content/Scripts/main.lua': " <<
			lua_tostring(L, -1) << '\n';
		return;
	}
	on_reload();
}

void Script::on_tick(double dt) {
	if (get_function("on_tick")) {
		lua_pushnumber(L, dt);
		if (lua_pcall(L, 1, 0, 0) != LUA_OK) {
			std::cerr << "ERROR: Could not call on_tick: " << lua_tostring(L, -1) << '\n';
		}
	}
	lua_settop(L, 0);
}

void Script::on_reload() {
	if (get_function("on_reload")) {
		if (lua_pcall(L, 0, 0, 0) != LUA_OK) {
			std::cerr << "ERROR: Could not call on_reload: " << lua_tostring(L, -1) << '\n';
		}
	}
	lua_settop(L, 0);
}

void Script::on_join(uint16_t client, bool has_touch) {
	if (get_function("on_join")) {
		lua_pushinteger(L, client);
		lua_pushboolean(L, has_touch);
		if (lua_pcall(L, 2, 0, 0) != LUA_OK) {
			std::cerr << "ERROR: Could not call on_join: " << lua_tostring(L, -1) << '\n';
		}
	}
	lua_settop(L, 0);
}

void Script::on_quit(uint16_t client) {
	if (get_function("on_quit")) {
		lua_pushinteger(L, client);
		if (lua_pcall(L, 1, 0, 0) != LUA_OK) {
			std::cerr << "ERROR: Could not call on_quit: " << lua_tostring(L, -1) << '\n';
		}
	}
	lua_settop(L, 0);
}

void Script::on_key_event(uint16_t client, int32_t key, bool down) {
	const char* method = down ? "on_key_down" : "on_key_up";
	if (get_function(method)) {
		auto it = keycodes.find(key);
		if (it != keycodes.end()) {
			lua_pushinteger(L, client);
			lua_pushstring(L, it->second);
			if (lua_pcall(L, 2, 0, 0) != LUA_OK) {
				std::cerr << "ERROR: Could not call " << method << ": " << lua_tostring(L, -1) <<
					'\n';
			}
		}
		else {
			std::cerr << "ERROR: Unknown keycode '" << key << "'\n";
		}
	}
	lua_settop(L, 0);
}

void Script::on_finger_event(uint16_t client, float x, float y, uint8_t finger, uint8_t type) {
	const char* method = nullptr;
	switch (static_cast<InputEventType>(type)) {
	case InputEventType::DOWN:
		method = "on_finger_down";
		break;
	case InputEventType::UP:
		method = "on_finger_up";
		break;
	case InputEventType::MOTION:
		method = "on_finger_motion";
		break;
	}
	if (get_function(method)) {
		lua_pushinteger(L, client);
		lua_pushinteger(L, finger);
		lua_pushnumber(L, x);
		lua_pushnumber(L, y);
		if (lua_pcall(L, 4, 0, 0) != LUA_OK) {
			std::cerr << "ERROR: Could not call " << method << ": " << lua_tostring(L, -1) << '\n';
		}
	}
	lua_settop(L, 0);
}

void Script::on_mouse_button(uint16_t client, float x, float y, uint8_t button, bool down) {
	const char* method = down ? "on_mouse_button_down" : "on_mouse_button_up";
	if (get_function(method)) {
		lua_pushinteger(L, client);
		switch (button) {
		case 1:
			lua_pushstring(L, "Left");
			break;
		case 2:
			lua_pushstring(L, "Middle");
			break;
		case 3:
			lua_pushstring(L, "Right");
			break;
		case 4:
			lua_pushstring(L, "Extra 1");
			break;
		case 5:
			lua_pushstring(L, "Extra 2");
			break;
		}
		lua_pushnumber(L, x);
		lua_pushnumber(L, y);
		if (lua_pcall(L, 4, 0, 0) != LUA_OK) {
			std::cerr << "ERROR: Could not call " << method << ": " << lua_tostring(L, -1) << '\n';
		}
	}
	lua_settop(L, 0);
}

void Script::on_mouse_motion(uint16_t client, float x, float y) {
	if (get_function("on_mouse_motion")) {
		lua_pushinteger(L, client);
		lua_pushnumber(L, x);
		lua_pushnumber(L, y);
		if (lua_pcall(L, 3, 0, 0) != LUA_OK) {
			std::cerr << "ERROR: Could not call on_mouse_motion: " << lua_tostring(L, -1) << '\n';
		}
	}
	lua_settop(L, 0);
}

void Script::on_mouse_wheel(uint16_t client, float x, float y) {
	if (get_function("on_mouse_wheel")) {
		lua_pushinteger(L, client);
		lua_pushnumber(L, x);
		lua_pushnumber(L, y);
		if (lua_pcall(L, 3, 0, 0) != LUA_OK) {
			std::cerr << "ERROR: Could not call on_mouse_wheel: " << lua_tostring(L, -1) << '\n';
		}
	}
	lua_settop(L, 0);
}

void Script::request_reload() {
	should_reload = true;
}

bool Script::check_reload() {
	if (should_reload) {
		should_reload = false;
		reload();
		return true;
	}
	else {
		return false;
	}
}

int Script::lua_reload(lua_State* L) {
	Script* script = reinterpret_cast<Script*>(lua_touserdata(L, lua_upvalueindex(1)));
	script->should_reload = true;
	return 0;
}

int Script::start_text_input(lua_State* L) {
	Script* script = reinterpret_cast<Script*>(lua_touserdata(L, lua_upvalueindex(1)));
	int client = luaL_checkinteger(L, 1);
	if (!script->server->start_text_input(client)) {
		luaL_error(L, "Client %d is not online", client);
	}
	return 0;
}

int Script::stop_text_input(lua_State* L) {
	Script* script = reinterpret_cast<Script*>(lua_touserdata(L, lua_upvalueindex(1)));
	int client = luaL_checkinteger(L, 1);
	if (!script->server->stop_text_input(client)) {
		luaL_error(L, "Client %d is not online", client);
	}
	return 0;
}

int Script::get_composition(lua_State* L) {
	Script* script = reinterpret_cast<Script*>(lua_touserdata(L, lua_upvalueindex(1)));
	int client = luaL_checkinteger(L, 1);
	const char* result = script->server->get_composition(client);
	if (result == nullptr) {
		return luaL_error(L, "Client %d is not online", client);
	}
	else {
		lua_pushstring(L, result);
		return 1;
	}
}

int Script::get_sprite_width(lua_State* L) {
	Script* script = reinterpret_cast<Script*>(lua_touserdata(L, lua_upvalueindex(1)));
	std::string path = luaL_checkstring(L, 1);
	lua_pushnumber(L, script->server->get_sprite_width(path));
	return 1;
}

int Script::draw_sprite(lua_State* L) {
	Script* script = reinterpret_cast<Script*>(lua_touserdata(L, lua_upvalueindex(1)));
	int client = luaL_checkinteger(L, 1);
	std::string path = luaL_checkstring(L, 2);
	float x = luaL_checknumber(L, 3);
	float y = luaL_checknumber(L, 4);
	float scale = luaL_checknumber(L, 5);
	int result = script->server->draw_sprite(client, path, x, y, scale);
	if (result == 1) {
		luaL_error(L, "Client %d is not online", client);
	}
	else if (result == 2) {
		luaL_error(L, "Image %s is not loaded", path.c_str());
	}
	return 0;
}

int Script::draw_text(lua_State* L) {
	Script* script = reinterpret_cast<Script*>(lua_touserdata(L, lua_upvalueindex(1)));
	int client = luaL_checkinteger(L, 1);
	std::string path = luaL_checkstring(L, 2);
	float x = luaL_checknumber(L, 3);
	float y = luaL_checknumber(L, 4);
	float scale = luaL_checknumber(L, 5);
	uint8_t r = luaL_checknumber(L, 6);
	uint8_t g = luaL_checknumber(L, 7);
	uint8_t b = luaL_checknumber(L, 8);
	std::string text = luaL_checkstring(L, 9);
	int result = script->server->draw_text(client, path, x, y, scale, r, g, b, text);
	if (result == 1) {
		return luaL_error(L, "Client %d is not online", client);
	}
	else if (result == 2) {
		return luaL_error(L, "Font %s is not loaded", path.c_str());
	}
	return 0;
}

int Script::kick(lua_State* L) {
	Script* script = reinterpret_cast<Script*>(lua_touserdata(L, lua_upvalueindex(1)));
	int client = luaL_checkinteger(L, 1);
	if (!script->server->kick(client)) {
		luaL_error(L, "Client %d is not online", client);
	}
	return 0;
}

int Script::play_sound(lua_State* L) {
	Script* script = reinterpret_cast<Script*>(lua_touserdata(L, lua_upvalueindex(1)));
	int client = luaL_checkinteger(L, 1);
	std::string path = luaL_checkstring(L, 2);
	int volume = luaL_checkinteger(L, 3);
	if (volume < 0 || volume > 128) {
		return luaL_error(L, "Invalid volume, must be between 0 and 128");
	}
	int result;
	if (lua_gettop(L) > 3) {
		uint16_t channel = luaL_checkinteger(L, 4);
		if (channel >= ANOMALY_AUDIO_CHANNELS) {
			return luaL_error(L, "Invalid channel, must be between 0 and %d",
				static_cast<int>(ANOMALY_AUDIO_CHANNELS) / 2 - 1);
		}
		result = script->server->play(client, path, channel, volume);
	}
	else {
		result = script->server->play_any(client, path, volume);
	}
	if (result == 1) {
		return luaL_error(L, "Client %d is not online", client);
	}
	else if (result == 2) {
		return luaL_error(L, "Sound %s is not loaded", path.c_str());
	}
	return 0;
}

int Script::stop_sound(lua_State* L) {
	Script* script = reinterpret_cast<Script*>(lua_touserdata(L, lua_upvalueindex(1)));
	int client = luaL_checkinteger(L, 1);
	uint16_t channel = luaL_checkinteger(L, 2);
	if (channel >= ANOMALY_AUDIO_CHANNELS) {
		return luaL_error(L, "Invalid channel, must be between 0 and %d",
			static_cast<int>(ANOMALY_AUDIO_CHANNELS) / 2 - 1);
	}
	if (!script->server->stop(client, channel)) {
		return luaL_error(L, "Client %d is not online", client);
	}
	return 0;
}

int Script::stop_all_sounds(lua_State* L) {
	Script* script = reinterpret_cast<Script*>(lua_touserdata(L, lua_upvalueindex(1)));
	int client = luaL_checkinteger(L, 1);
	if (!script->server->stop_all(client)) {
		return luaL_error(L, "Client %d is not online", client);
	}
	return 0;
}

bool Script::get_function(const char* name) {
	lua_getglobal(L, name);
	if (lua_isnil(L, -1)) {
		return false;
	}
	return true;
}

void Script::register_callback(const char* name, lua_CFunction callback) {
	lua_pushlightuserdata(L, this);
	lua_pushcclosure(L, callback, 1);
	lua_setglobal(L, name);
}
