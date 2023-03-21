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
	{ 1073742053, "Right Shift" }
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
	register_callback("reload", lua_reload);
	register_callback("start_text_input", start_text_input);
	register_callback("stop_text_input", stop_text_input);
	register_callback("draw_sprite", draw_sprite);
	register_callback("draw_text", draw_text);
	if (luaL_dofile(L, "Content/Scripts/main.lua") != LUA_OK) {
		std::cerr << "ERROR: Could not load lua file 'Content/Scripts/main.lua': " <<
			lua_tostring(L, -1) << '\n';
		return;
	}
}

void Script::on_tick(float dt) {
	if (get_function("on_tick")) {
		lua_pushnumber(L, dt);
		if (lua_pcall(L, 1, 0, 0) != LUA_OK) {
			std::cerr << "ERROR: Could not call on_tick: " << lua_tostring(L, -1) << '\n';
		}
	}
	lua_settop(L, 0);
}

void Script::on_key_event(uint16_t client, int32_t key, bool down) {
	if (get_function("on_key_event")) {
		auto it = keycodes.find(key);
		if (it != keycodes.end()) {
			lua_pushinteger(L, client);
			lua_pushstring(L, it->second);
			lua_pushboolean(L, down);
			if (lua_pcall(L, 3, 0, 0) != LUA_OK) {
				std::cerr << "ERROR: Could not call on_key_event: " << lua_tostring(L, -1) << '\n';
			}
		}
		else {
			std::cerr << "ERROR: Unknown keycode '" << key << "'\n";
		}
	}
	lua_settop(L, 0);
}

void Script::on_touch_event(uint16_t client, float x, float y, uint8_t finger, bool down) {
	if (get_function("on_touch_event")) {
		lua_pushinteger(L, client);
		lua_pushnumber(L, x);
		lua_pushnumber(L, y);
		lua_pushinteger(L, finger);
		lua_pushboolean(L, down);
		if (lua_pcall(L, 5, 0, 0) != LUA_OK) {
			std::cerr << "ERROR: Could not call on_touch_event: " << lua_tostring(L, -1) << '\n';
		}
	}
	lua_settop(L, 0);
}

int Script::lua_reload(lua_State* L) {
	Script* script = reinterpret_cast<Script*>(lua_touserdata(L, lua_upvalueindex(1)));
	script->reload();
	return 0;
}

int Script::start_text_input(lua_State* L) {
	Script* script = reinterpret_cast<Script*>(lua_touserdata(L, lua_upvalueindex(1)));
	uint16_t client = luaL_checkinteger(L, 1);
	if (!script->server->start_text_input(client)) {
		luaL_error(L, "Client %d is not online", client);
	}
	return 0;
}

int Script::stop_text_input(lua_State* L) {
	Script* script = reinterpret_cast<Script*>(lua_touserdata(L, lua_upvalueindex(1)));
	uint16_t client = luaL_checkinteger(L, 1);
	if (!script->server->stop_text_input(client)) {
		luaL_error(L, "Client %d is not online", client);
	}
	return 0;
}

int Script::draw_sprite(lua_State* L) {
	Script* script = reinterpret_cast<Script*>(lua_touserdata(L, lua_upvalueindex(1)));
	uint16_t client = luaL_checkinteger(L, 1);
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
	uint16_t client = luaL_checkinteger(L, 1);
	std::string path = luaL_checkstring(L, 2);
	float x = luaL_checknumber(L, 3);
	float y = luaL_checknumber(L, 4);
	float scale = luaL_checknumber(L, 5);
	uint8_t r = luaL_checkinteger(L, 6);
	uint8_t g = luaL_checkinteger(L, 7);
	uint8_t b = luaL_checkinteger(L, 8);
	std::string text = luaL_checkstring(L, 9);
	int result = script->server->draw_text(client, path, x, y, scale, r, g, b, text);
	if (result == 1) {
		luaL_error(L, "Client %d is not online", client);
	}
	else if (result == 2) {
		luaL_error(L, "Font %s is not loaded", path.c_str());
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
