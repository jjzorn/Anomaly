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
	{ 1073742048, "Left Ctrl" },
	{ 1073742049, "Left Shift" },
	{ 1073742052, "Right Ctrl" },
	{ 1073742053, "Right Shift" }
};

Script::Script(Server& server, const std::string& path) : server{ &server } {
	L = luaL_newstate();
	if (!L) {
		std::cerr << "ERROR: Could not initialize Lua\n";
		return;
	}
	luaL_openlibs(L);
	reload(path);
}

Script::~Script() {
	lua_close(L);
}

void Script::reload(const std::string& path) {
	if (luaL_dofile(L, path.c_str()) != LUA_OK) {
		std::cerr << "ERROR: Could not load lua file '" << path << "': " << lua_tostring(L, -1) << '\n';
		return;
	}
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

bool Script::get_function(const char* name) {
	lua_getglobal(L, name);
	if (lua_isnil(L, -1)) {
		return false;
	}
	return true;
}
