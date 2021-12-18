#include "luadbg.h"
#include "lua_net.h"
#include "net_thread_queue.h"

#include <functional>
#include <thread>

#include "kbase/at_exit_manager.h"
#include "ezio/io_context.h"
#include "ezio/io_service_context.h"
#include "ezio/event_loop.h"
#include "ezio/socket_address.h"
#include "ezio/tcp_server.h"
#include "ezio/tcp_connection.h"
#include "ezio/buffer.h"
#include "ezio/acceptor.h"
#include "ezio/connector.h"

#include "luadbg.inl"
#include "lua_bind.h"
#include "cxlua.h"
 
using namespace ezio;
    
      
std::string LUADBG_LINES_ENDING = "";
int debugger_send_message(lua_State* L);
void luadbg_set_line_ending_in_c(const char* le)
{   
	LUADBG_LINES_ENDING = le;
}
const char* luadbg_get_line_ending_in_c()
{
	return LUADBG_LINES_ENDING.c_str();
}
  
std::thread* debuggee_thread;
NetThreadQueue g_DebugAdapterQueue;
TCPConnectionPtr g_DebugAdapterHandler;
TimerID g_RuntimeSendTimerID ;
EventLoop* g_RuntimeServerLoop = nullptr;

#define luaL_requirelib(L,name,fn) (luaL_requiref(L, name, fn, 1),lua_pop(L, 1))
extern "C"  int luaopen_cjson(lua_State *L);

void luadbg_register_common_lua_functions(lua_State* L){
	luaL_requirelib(L, "cjson", luaopen_cjson);
	luaopen_netlib(L);
	luaopen_net_thread_queue(L);
	script_system_register_function(L, luadbg_set_line_ending_in_c);
	script_system_register_function(L, luadbg_get_line_ending_in_c);
}

void DebuggeeThreadFunc(int port)
{
	lua_State* L = luaL_newstate();
	luaL_openlibs(L);
	luadbg_register_common_lua_functions(L);

	int res = luaL_loadbuffer(L, debuggee_code,strlen(debuggee_code),"__debuggee__");
	check_lua_error(L, res);
	lua_pushstring(L, "debuggee");
	res = lua_pcall(L, 1, LUA_MULTRET, 0);
	check_lua_error(L, res);

	EventLoop loop;
	g_RuntimeServerLoop = &loop;
	SocketAddress addr(port);
	TCPServer server(&loop, addr, "RuntimeServer");
	 
	server.set_on_connection([L](const TCPConnectionPtr& conn) {
		g_DebugAdapterHandler = conn->connected() ? conn : nullptr;
		lua_getglobal(L, "debuggee_on_connection");
		lua_push_tcp_connection(L, conn);
		lua_push_net_thread_queue(L, &g_DebugAdapterQueue);
		int res = lua_pcall(L, 2, 0, 0); 
		check_lua_error(L, res);
	});
	 
	server.set_on_message([L](const TCPConnectionPtr& conn, Buffer& buf, TimePoint ts) {
		lua_getglobal(L, "debuggee_on_message");
		lua_push_tcp_connection(L, conn);
		lua_push_ezio_buffer(L, buf);
		lua_push_net_thread_queue(L, &g_DebugAdapterQueue);
		int res = lua_pcall(L, 3, 0, 0);
		check_lua_error(L, res);
	});
	server.Start();
	loop.Run();
	lua_close(L); 
}

int luadbg_listen(lua_State* L)
{
	bool init_net_context = false;
	if (!lua_isnil(L, 2))
	{
		init_net_context = (bool)lua_toboolean(L, 2);
	}
	if (init_net_context)
	{
		new kbase::AtExitManager();
		ezio::IOServiceContext::Init();
	}
	int port = (int)lua_tointeger(L, 1);
	if (port > 0)
	{
		debuggee_thread = new std::thread(DebuggeeThreadFunc, port);
		int res = luaL_loadbuffer(L, debugger_code, strlen(debugger_code), "@__debugger__");
		check_lua_error(L, res);
		lua_pushstring(L, "debugger");
		res = lua_pcall(L, 1, LUA_MULTRET, 0);
		check_lua_error(L, res);
	}
	printf("luadbg_listen %d\n", port);
	return 0;
}

void luadbg_stop()
{
	if(g_RuntimeServerLoop)
		g_RuntimeServerLoop->Quit();

	if (debuggee_thread)
	{
		debuggee_thread->join();
		delete debuggee_thread;
		debuggee_thread = nullptr;
	}
}


void debugger_sleep(int s)
{
#if defined(_WIN32)
	Sleep(s);
#else 
    sleep(s);
#endif     
}     
   
const char* debugger_fetch_message()
{    
	if (!g_DebugAdapterQueue.Empty(NetThreadQueue::Read))
	{  
		static std::string msgstr;
		auto& msg = g_DebugAdapterQueue.Front(NetThreadQueue::Read);
		msgstr = std::string(msg.Peek(), msg.readable_size());
		g_DebugAdapterQueue.PopFront(NetThreadQueue::Read);
		return msgstr.c_str();
	}
	return "";
} 
   
int debugger_send_message(lua_State* L)
{   
	if (g_DebugAdapterHandler && g_DebugAdapterHandler->connected())
	{ 
		size_t len = 0;
		const char* data = luaL_tolstring(L, 1,&len);
		g_DebugAdapterHandler->Send({ data, len });
	}  
	return 0; 
}
 
bool debugger_is_connected()
{
	return g_DebugAdapterHandler != nullptr && g_DebugAdapterHandler->connected();
}

LuaProxy* (*__proxy__)(); 
LuaProxy* __lua_proxy__()
{
	return __proxy__();
}

#ifdef __cplusplus
extern "C" {
#endif	 
LUADBGAPI int _luaopen_luadbg(LuaProxy* (*proxy)(), lua_State* L)
{
	__proxy__ = proxy;
	luaL_requirelib(L, "cjson", luaopen_cjson);

	script_system_register_luac_function(L, luadbg_listen);
	script_system_register_function(L, luadbg_stop);
	
	script_system_register_function(L, debugger_sleep);
	  
	script_system_register_function(L, debugger_fetch_message);
	script_system_register_function(L, debugger_is_connected);
	script_system_register_luac_function(L, debugger_send_message);

	script_system_register_function(L, luadbg_get_line_ending_in_c);
	return 1;
} 
#ifdef __cplusplus
}
#endif
