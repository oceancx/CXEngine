#pragma once
#include "lua_bind.h"
#include "ezio/tcp_connection.h"

void lua_push_tcp_connection(lua_State*L,const ezio::TCPConnectionPtr& conn);
void lua_push_ezio_buffer(lua_State*L, ezio::Buffer& buf);
ezio::TCPConnection* lua_check_tcpconnection(lua_State* L, int index);
ezio::Buffer* lua_check_buffer(lua_State* L, int index);


void luaopen_netlib(lua_State* L);


