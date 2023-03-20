// Copyright 2023 Justus Zorn

#ifndef ANOMALY_SERVER_SCRIPT_H
#define ANOMALY_SERVER_SCRIPT_H

#include <string>

#include <lua.hpp>

class Server;

class Script {
public:
	Script(Server& server, const std::string& path);
	Script(const Script&) = delete;
	~Script();

	Script& operator=(const Script&) = delete;

	void reload(const std::string& path);

	void on_key_event(uint16_t client, int32_t key, bool down);

private:
	Server* server;
	lua_State* L;

	bool get_function(const char* name);
};

#endif
