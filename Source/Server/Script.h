// Copyright 2023 Justus Zorn

#ifndef ANOMALY_SERVER_SCRIPT_H
#define ANOMALY_SERVER_SCRIPT_H

#include <string>

#include <lua.hpp>

class Server;

class Script {
public:
	Script(Server& server);
	Script(const Script&) = delete;
	~Script();

	Script& operator=(const Script&) = delete;

	void reload();

	void on_tick(float dt);
	void on_key_event(uint16_t client, int32_t key, bool down);
	void on_touch_event(uint16_t client, float x, float y, uint8_t finger, bool down);

private:
	Server* server;
	lua_State* L;

	static int lua_reload(lua_State* L);

	static int start_text_input(lua_State* L);
	static int stop_text_input(lua_State* L);

	static int draw_sprite(lua_State* L);
	static int draw_text(lua_State* L);

	bool get_function(const char* name);
	void register_callback(const char* name, lua_CFunction callback);
};

#endif
