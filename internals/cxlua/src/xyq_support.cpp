#include "xyq_support.h"

#include "NESupport.h"
#include "lua_bind.h"

luaL_Reg MT_XYQ_WDF[] = {
	/*{ "FileList",ez_event_loop_run },
	{ "Unpack",ez_event_loop_run },
	{ "Type",ez_event_loop_run },*/
	{ NULL, NULL}
};


luaL_Reg MT_XYQ_MAP[] = {
	//{ "Run",ez_event_loop_run },
	{ NULL, NULL}
};


int xyq_load_wdf(lua_State* L) {
	const char* path = lua_tostring(L, 1);
	NE::WDF* wdf = new NE::WDF(path);
	lua_push_pointer(L, wdf);
	if (luaL_newmetatable(L, "MT_XYQ_WDF")) {
		luaL_setfuncs(L, MT_XYQ_WDF, 0);
		lua_pushvalue(L, -1);
		lua_setfield(L, -2, "__index");
	}
	lua_setmetatable(L, -2);
	return 1;
}

int xyq_unload_wdf(lua_State* L) {
	(void*)L;
	return 0;
}


int xyq_load_map(lua_State* L) {
	(void*)L;
	return 0;
}


int xyq_unload_map(lua_State* L) {
	(void*)L;
	return 0;
}
luaL_Reg MT_XYQ[] = {
	{ "LoadWDF", xyq_load_wdf},
	{ "LoadMAP",xyq_load_map},
	{ "UnLoadWDF",xyq_unload_wdf},
	{ "UnLoadMAP",xyq_unload_map},
	{ NULL, NULL}
};

void lua_push_xyq_wdf(lua_State* L)
{
	/*lua_push_pointer(L, loop);
	if (luaL_newmetatable(L, "MT_EZ_EVENT_LOOP")) {
		luaL_setfuncs(L, MT_EZ_EVENT_LOOP, 0);
		lua_pushvalue(L, -1);
		lua_setfield(L, -2, "__index");
	}
	lua_setmetatable(L, -2);*/
	(void*)L;
}



//int ez_event_loop_destroy(lua_State* L) {
//	auto* sp = lua_check_pointer<ezio::EventLoop>(L, 1);
//	delete sp;
//	return 0;
//}
//


void luaopen_xyq_support(lua_State* L)
{
	(void*)L;
}