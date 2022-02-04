#include <script_system.h>
#include <script_system.inl>

#include "define_types.h"
#include "singleton.h"
#include "file_system.h" 
#include "logger.h"

#include "file_loading.h"
#include "imgui/cximgui.h"
#include "net_thread_queue.h"
#include "lua_net.h"
#include "time/timer_manager.h"

#ifdef SIMPLE_ENGINE
#include "window.h"
#include "game.h"
#include "ui.h"
#include "text_renderer.h"
#include "input_manager.h"
#include "net.h"
#include "animation/sprite.h"
#include "resource_manager.h"
#include "sprite_renderer.h"
#include "audio/audio_manager.h"
#include "graphics/ui_renderer.h"
#endif

#ifdef SIMPLE_SERVER
#include "server.h"
#endif // SIMPLE_SERVER

#ifndef CXLUAX
#include "protocol.h"
#include "actor/action.h"
#include "actor/actor_manager.h"
#endif // !CXLUAX
#ifdef SIMPLE_ENGINE
#include <scene/scene_manager.h>
extern "C" {
#define luaL_requirelib(L,name,fn) (luaL_requiref(L, name, fn, 1),lua_pop(L, 1))
}
#endif
#include <cxlua.h>
#include <tsv.h>

static bool g_DebugInCpp = false;
static lua_State* L = nullptr;
void luaopen_script_system(lua_State* L);

HINSTANCE hDll = nullptr;

void script_system_prepare_init()
{
	L = luaL_newstate();
	luaL_openlibs(L);
	luaopen_filesystem(L);
	luaopen_script_system(L);
	luaopen_cximgui(L);
	luaopen_logger(L);
	luaopen_ne_support(L);
	luaopen_timer_manager(L);

#ifndef CXLUAX
	luaopen_actor(L);
	luaopen_actor_manager(L);
	luaopen_protocol(L);
	luaopen_cxlua(L);
#else
	luaopen_cxlua(L);
#endif // !CXLUAX
#ifdef SIMPLE_ENGINE
	luaopen_window(L);
	luaopen_game(L);
	luaopen_resource_manager(L);
	luaopen_input_manager(L);
	luaopen_ui(L);
	luaopen_sprite_renderer(L);
	luaopen_text_renderer(L);
	luaopen_sprite(L);
	luaopen_net(L);
	luaopen_audio_manager(L);
	luaopen_ui_renderer(L);
	luaopen_action(L);
	luaopen_scene_manager(L);
#endif
#ifdef SIMPLE_SERVER
	luaopen_game_server(L);
#endif // !SIMPLE_SERVER

	int res = luaL_loadbuffer(L, script_system_lua_code, strlen(script_system_lua_code), "__script_system_lua_code__");
	check_lua_error(L, res);
	res = lua_pcall(L, 0, LUA_MULTRET, 0);
	check_lua_error(L, res);

	typedef 	int (*LuaDbgFunc)(lua_State* L);
	hDll = nullptr;
#ifdef SIMPLE_ENGINE
	hDll = ::LoadLibrary(L"cxluadbg.dll");
#else
	hDll = ::LoadLibrary("cxluadbg.dll");
#endif
	if (hDll)
	{
		LuaDbgFunc instance = (LuaDbgFunc)GetProcAddress(hDll, "luaopen_luadbg");
		instance(L);
	}
}

void script_system_run_main_script()
{
	string path = command_arg_opt_str("luapath", "main.lua");
	int res = luaL_dofile(L, FileSystem::GetLuaPath(path).c_str());
	check_lua_error(L, res);
}

void script_system_init()
{
	if (g_DebugInCpp)return;
	
	script_system_call_function(L, "on_script_system_init");
}

bool script_system_update()
{
	if (g_DebugInCpp)return true;
	std::vector<any> rets = script_system_call_function(L, "on_script_system_update");
	if (rets.size() > 0) {
		bool success = any_cast<bool>(rets[0]);
		return success;
	} 
	else {
		return true;
	}
}


void script_system_draw()
{
	if (g_DebugInCpp)return;
	script_system_call_function(L, "on_script_system_draw");
}

void script_system_deinit()
{
	if (g_DebugInCpp)return;
	if (hDll != nullptr)
	{
		FreeLibrary(hDll);
	}
	script_system_call_function(L, "on_script_system_deinit");
}

lua_State* script_system_get_luastate()
{
	return L;
}

any script_system_get_globle(const char* name)
{
	lua_getglobal(L, name);
	any v = lua_getanyvalue(L, -1);
	lua_pop(L, 1);
	return v;
}


bool process_is_server() {
#ifdef SIMPLE_SERVER
	return true;
#else
	return false;
#endif
}



uint64_t time_now() {
	auto now = std::chrono::system_clock::now();
	return  now.time_since_epoch().count() / 10000;
}
uint64_t time_now_nano() {
	auto now = std::chrono::system_clock::now();
	return  now.time_since_epoch().count();
}


int lua_time_now(lua_State* L) {
	uint64_t  now = time_now();
	lua_pushinteger(L, now);
	return 1;
}


void luaopen_script_system(lua_State* L)
{

#define REG_ENUM(name, macro)  (lua_pushinteger(L, macro),lua_setglobal(L, name))
#ifdef SIMPLE_SERVER
	REG_ENUM("SIMPLE_SERVER", SIMPLE_SERVER);
#endif
#ifdef SIMPLE_ENGINE
	REG_ENUM("SIMPLE_ENGINE", SIMPLE_ENGINE);
#endif
#ifdef CXLUAX
	REG_ENUM("CXLUAX", CXLUAX);
#endif
#undef REG_ENUM

	script_system_register_function(L, process_is_server);

	script_system_register_luac_function_with_name(L, "time_now", lua_time_now);
	script_system_register_luac_function_with_name(L, "time_now_ms", lua_time_now);
	script_system_register_function(L, time_now_nano);

}