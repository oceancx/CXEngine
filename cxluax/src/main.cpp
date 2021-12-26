#include <script_system.h>
#include <script_system.inl>

#include <luadbg.h>
#include "imgui/cximgui.h"
#include "logger.h"
#include "window.h"
#include "file_loading.h"

#include <file_system.h>
#include <ezio/event_loop.h>
#include <ezio/tcp_server.h>
#include <kbase/at_exit_manager.h>
#include <ezio/io_service_context.h>
#include "cxlua.h"
#include "net_thread_queue.h"
#include <time/timer_manager.h>
#include <record.h>
#include <asio_test.h>

int main(int argc, char* argv[])
{
	handle_command_args(argc, argv);
	FileSystem::InitWorkPath();

	lua_State* L = luaL_newstate();
	luaL_openlibs(L);
	luaopen_luadbg(L);
	luaopen_filesystem(L);
	luaopen_cximgui(L);
	luaopen_logger(L);
	luaopen_ne_support(L);
	luaopen_window(L);
	luaopen_cxlua(L);
	luaopen_timer_manager(L);
	luaopen_recorder(L);

	luaopen_asio_test(L);


	int res = luaL_loadbuffer(L, script_system_lua_code, strlen(script_system_lua_code), "__script_system_lua_code__");
	check_lua_error(L, res);
	res = lua_pcall(L, 0, LUA_MULTRET, 0);
	check_lua_error(L, res);

	std::string luapath = command_arg_opt_str("luapath", "main.lua");

	res = luaL_dofile(L, FileSystem::GetLuaPath(luapath).c_str());
	check_lua_error(L, res);
	lua_close(L);

	return 0;
}




//
// blocking_tcp_echo_client.cpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2021 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

//#include <cstdlib>
//#include <cstring>
//#include <iostream>
//#include "asio.hpp"
//
//using asio::ip::tcp;
//
//enum { max_length = 1024 };
//
//int main(int argc, char* argv[])
//{
//	try
//	{
//		if (argc != 3)
//		{
//			std::cerr << "Usage: blocking_tcp_echo_client <host> <port>\n";
//			return 1;
//		}
//
//		asio::io_context io_context;
//
//		tcp::socket s(io_context);
//		tcp::resolver resolver(io_context);
//		asio::connect(s, resolver.resolve(argv[1], argv[2]));
//
//		std::cout << "Enter message: ";
//		char request[max_length];
//		std::cin.getline(request, max_length);
//		size_t request_length = std::strlen(request);
//		asio::write(s, asio::buffer(request, request_length));
//
//		char reply[max_length];
//		size_t reply_length = asio::read(s,
//			asio::buffer(reply, request_length));
//		std::cout << "Reply is: ";
//		std::cout.write(reply, reply_length);
//		std::cout << "\n";
//	}
//	catch (std::exception& e)
//	{
//		std::cerr << "Exception: " << e.what() << "\n";
//	}
//
//	return 0;
//}
