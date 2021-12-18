#pragma once


#include "protocol.h"


void net_connect_to_server();

void net_manager_connect();
void luaopen_net(lua_State* L);