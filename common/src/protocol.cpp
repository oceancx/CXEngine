#include "protocol.h"

void luaopen_protocol(lua_State* L)
{
	(lua_pushinteger(L, CX_MSG_HEADER_LEN), lua_setglobal(L, "CX_MSG_HEADER_LEN"));
}