#include <string>
#include "kbase/at_exit_manager.h"
#include "ezio/io_service_context.h"
#include "ezio/event_loop.h"
#include "ezio/tcp_server.h"
#include "ezio/tcp_client.h"
#include "kbase/string_util.h"
#include "lua_net.h"
#include "net_thread_queue.h"
#include <iostream>
#include <string.h>
#include <future>
#include "cxlua.h"
#include <luadbg.h>
#include <fstream>

using namespace ezio;

std::string CWD = "";
std::string EXTENSION_DIR(const char *dir)
{
	return CWD + dir;
}

std::string LINES_ENDING = "";
void set_line_ending_in_c(const char *le)
{
	if (LINES_ENDING == "")
	{
		LINES_ENDING = le;
	}
}
const char *get_line_ending_in_c()
{
	return LINES_ENDING.c_str();
}

void sleepms(int ms)
{
	Sleep(ms);
}

NetThreadQueue vscode_netq;
void start_stdio_read_thread(){
	new std::thread([]() {
		lua_State *L = luaL_newstate();
		luaL_openlibs(L);
		luaopen_cxlua(L);
		script_system_register_function(L, get_line_ending_in_c);
		script_system_register_function(L, set_line_ending_in_c);

		int res = luaL_loadfile(L, EXTENSION_DIR("main.lua").c_str());
		check_lua_error(L, res);
		lua_pushstring(L, "vscode");
		res = lua_pcall(L, 1, LUA_MULTRET, 0);
		check_lua_error(L, res);
		 
		int c = 0;
		Buffer buf;
		while (std::cin.read(reinterpret_cast<char *>(&c), 1))
		{
			buf.Write((int8_t)c);
			lua_getglobal(L, "vscode_on_message");
			lua_pushnil(L);
			lua_push_ezio_buffer(L, buf);
			lua_push_net_thread_queue(L, &vscode_netq);
			int res = lua_pcall(L, 3, 0, 0);
			check_lua_error(L, res);
		}
	});
}

int main(int argc, char *argv[])
{
	handle_command_args(argc, argv);
	kbase::AtExitManager exit_manager;
	init_default_cwd(argv[0]);
	CWD = command_arg_opt_str("cwd", get_default_cwd().c_str());

	ezio::IOServiceContext::Init();
	lua_State *L = luaL_newstate();
	luaL_openlibs(L);
	luaopen_cxlua(L);
	script_system_register_function(L, start_stdio_read_thread);
	script_system_register_function(L, get_line_ending_in_c);
	script_system_register_function(L, set_line_ending_in_c);
	script_system_register_function(L, sleepms);

	lua_push_net_thread_queue(L, &vscode_netq);
	lua_setglobal(L, "vscode_netq");

	int res = luaL_dofile(L, EXTENSION_DIR("main.lua").c_str());
	check_lua_error(L, res);

	lua_close(L);
	return 0;
}
