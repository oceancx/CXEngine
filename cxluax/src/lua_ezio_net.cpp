#include "lua_net.h"
#include <iostream>
#include "ezio/tcp_connection.h"
#include <ezio/event_loop.h>
#include "ezio/tcp_server.h"
#include "ezio/tcp_client.h"
#include "ezio/io_service_context.h"
#include "kbase/at_exit_manager.h"
#include <chrono>
#include "cxlua.h"

#include "ezio/buffer.h"

static const char* skey_mt_tcp_connection = "key_mt_tcp_connection";
ezio::Buffer* lua_check_buffer(lua_State* L, int index);

void lua_push_tcp_connection(lua_State* L, const ezio::TCPConnectionPtr& conn)
{
	ezio::TCPConnection** ptr = (ezio::TCPConnection**)lua_newuserdata(L, sizeof(ezio::TCPConnection*));
	*ptr = conn.get();
	luaL_setmetatable(L, skey_mt_tcp_connection);
}

ezio::TCPConnection* lua_check_tcpconnection(lua_State* L, int index)
{
	ezio::TCPConnection** ptr = (ezio::TCPConnection**)lua_touserdata(L, index);
	return *ptr;
}


int tcp_connection_to_host_port(lua_State* L)
{
	ezio::TCPConnection* conn = lua_check_tcpconnection(L, 1);
	lua_pushstring(L, conn->peer_addr().ToHostPort().c_str());
	return 1;
}

int tcp_connection_connected(lua_State* L)
{
	ezio::TCPConnection* conn = lua_check_tcpconnection(L, 1);
	lua_pushboolean(L, conn->connected());
	return 1;
}

int tcp_connection_send(lua_State* L)
{
	ezio::TCPConnection* conn = lua_check_tcpconnection(L, 1);
	size_t len = 0;
	const char* data = lua_tolstring(L, 2, &len);
	conn->Send(kbase::StringView(data, len));
	return 0;
}


int tcp_connection_Send(lua_State* L)
{
	ezio::TCPConnection* conn = lua_check_tcpconnection(L, 1);
	ezio::Buffer* buff = lua_check_buffer(L, 2);
	conn->Send({ buff->Peek(),buff->readable_size() });
	return 0;
}


int tcp_connection_set_tcp_nodelay(lua_State* L)
{
	ezio::TCPConnection* conn = lua_check_tcpconnection(L, 1);
	bool nodelay = lua_toboolean(L, 2);
	conn->SetTCPNoDelay(nodelay);
	return 0;
}


luaL_Reg mt_tcp_connection_reg[] = {
	{ "tohostport",tcp_connection_to_host_port},
	{ "connected",tcp_connection_connected},
	{ "send",tcp_connection_send },
	{ "Send",tcp_connection_Send },
	{ "SetTCPNoDelay",tcp_connection_set_tcp_nodelay },
	{ NULL, NULL }
};


static const char* skey_mt_buffer = "key_mt_buffer";
using LuaEzioBuffer = ezio::Buffer*;

ezio::Buffer* lua_check_buffer(lua_State* L, int index)
{
	LuaEzioBuffer* buffer = (LuaEzioBuffer*)lua_touserdata(L, index);
	return *buffer;
}

int buffer_readablesize(lua_State* L)
{
	ezio::Buffer* buffer = (ezio::Buffer*)lua_check_buffer(L, 1);
	lua_pushinteger(L, buffer->readable_size());
	return 1;
}

int buffer_preview(lua_State* L)
{
	ezio::Buffer* buffer = (ezio::Buffer*)lua_check_buffer(L, 1);
	int len = (int)lua_tointeger(L, 2);
	luaL_Buffer lbuf;
	luaL_buffinitsize(L, &lbuf, len);
	luaL_addlstring(&lbuf, buffer->Peek(), len);
	luaL_pushresult(&lbuf);
	return 1;
}

int buffer_consume(lua_State* L)
{
	ezio::Buffer* buffer = (ezio::Buffer*)lua_check_buffer(L, 1);
	int len = (int)lua_tointeger(L, 2);
	buffer->Consume(len);
	return 0;
}


int buffer_prependint(lua_State* L)
{
	ezio::Buffer* buffer = (ezio::Buffer*)lua_check_buffer(L, 1);
	int n = (int)lua_tointeger(L, 2);
	buffer->Prepend(n);

	return 0;
}

int buffer_prependstring(lua_State* L)
{
	ezio::Buffer* buffer = (ezio::Buffer*)lua_check_buffer(L, 1);
	size_t len = 0;
	const char* data = lua_tolstring(L, 2, &len);
	buffer->Prepend(data, len);
	return 0;
}


int buffer_writestring(lua_State* L)
{
	ezio::Buffer* buffer = (ezio::Buffer*)lua_check_buffer(L, 1);
	size_t len = 0;
	const char* data = lua_tolstring(L, 2, &len);
	buffer->Write(data, len);
	return 0;
}

int buffer_writeint(lua_State* L)
{
	ezio::Buffer* buffer = (ezio::Buffer*)lua_check_buffer(L, 1);
	int n = (int)lua_tointeger(L, 2);
	buffer->Write(n);
	return 0;
}

int buffer_writebyte(lua_State* L)
{
	ezio::Buffer* buffer = (ezio::Buffer*)lua_check_buffer(L, 1);
	int8_t n = (int8_t)lua_tointeger(L, 2);
	buffer->Write((int8_t)n);
	return 0;
}


int buffer_writeint64(lua_State* L)
{
	ezio::Buffer* buffer = (ezio::Buffer*)lua_check_buffer(L, 1);
	uint64_t n = (uint64_t)lua_tointeger(L, 2);
	buffer->Write(n);
	return 0;
}

int buffer_writebuffer(lua_State* L)
{
	ezio::Buffer* buf = (ezio::Buffer*)lua_check_buffer(L, 1);
	ezio::Buffer* src = (ezio::Buffer*)lua_check_buffer(L, 2);
	int len = (int)lua_tointeger(L, 3);
	buf->Write(src->Peek(), len);
	return 0;
}




int buffer_writefloat(lua_State* L)
{
	ezio::Buffer* buffer = (ezio::Buffer*)lua_check_buffer(L, 1);
	float num = (float)lua_tonumber(L, 2);
	buffer->Write(num);
	return 0;
}


int buffer_readstring(lua_State* L)
{
	ezio::Buffer* buffer = (ezio::Buffer*)lua_check_buffer(L, 1);
	int len = (int)lua_tointeger(L, 2);
	lua_pushstring(L, (buffer->ReadAsString(len)).c_str());
	return 1;
}

int buffer_readallstring(lua_State* L)
{
	ezio::Buffer* buffer = (ezio::Buffer*)lua_check_buffer(L, 1);
	std::string str(buffer->ReadAllAsString());
	lua_pushstring(L, str.c_str());
	return 1;
}

int buffer_peekstring(lua_State* L)
{
	ezio::Buffer* buffer = (ezio::Buffer*)lua_check_buffer(L, 1);
	int len = (int)lua_tointeger(L, 2);
	lua_pushlstring(L, buffer->Peek(), len);
	return 1;
}

int buffer_readfloat(lua_State* L)
{
	ezio::Buffer* buffer = (ezio::Buffer*)lua_check_buffer(L, 1);
	float v = buffer->ReadAsFloat();
	lua_pushnumber(L, v);
	return 1;
}

int buffer_peekfloat(lua_State* L)
{
	ezio::Buffer* buffer = (ezio::Buffer*)lua_check_buffer(L, 1);
	float v = buffer->PeekAsFloat();
	lua_pushnumber(L, v);
	return 1;
}


int buffer_readint(lua_State* L)
{
	ezio::Buffer* buffer = (ezio::Buffer*)lua_check_buffer(L, 1);
	int v = buffer->ReadAsInt32();
	lua_pushinteger(L, v);
	return 1;
}


int buffer_readint64(lua_State* L)
{
	ezio::Buffer* buffer = (ezio::Buffer*)lua_check_buffer(L, 1);
	int64_t v = buffer->ReadAsInt64();
	lua_pushinteger(L, v);
	return 1;
}



int buffer_peekint(lua_State* L)
{
	ezio::Buffer* buffer = (ezio::Buffer*)lua_check_buffer(L, 1);
	int v = buffer->PeekAsInt32();
	lua_pushinteger(L, v);
	return 1;
}


int buffer_gc(lua_State* L)
{
	LuaEzioBuffer* buf = (LuaEzioBuffer*)lua_touserdata(L, 1);
	free(buf);
	buf = nullptr;
	printf("ezio::TCPConnection :gc\n");
	return 0;
}

luaL_Reg mt_buffer_reg[] = {
	{ "readable_size",buffer_readablesize },
	{ "Preview",buffer_preview },
	{ "Consume",buffer_consume },
	{ "PrependInt",buffer_prependint},
	{ "PrependString",buffer_prependstring },
	{ "WriteString",buffer_writestring },
	{ "WriteFloat",buffer_writefloat },
	{ "WriteInt",buffer_writeint },
	{ "WriteByte",buffer_writebyte },
	{ "WriteInt64",buffer_writeint64 },
	{ "WriteBuffer",buffer_writebuffer},
	{ "ReadAsString",buffer_readstring },
	{ "ReadAllAsString",buffer_readallstring },

	{ "ReadAsFloat",buffer_readfloat },
	{ "ReadAsInt",buffer_readint },
	{ "ReadAsInt64",buffer_readint64 },
	{ "PeekAsInt",buffer_peekint},
	{ "PeekAsFloat",buffer_peekfloat },
	{ "PeekAsString",buffer_peekstring },

	//{ "__gc",buffer_gc },
	{ "Destroy",buffer_gc },
	{ NULL, NULL }
};

void lua_push_ezio_buffer(lua_State* L, ezio::Buffer& buf)
{
	LuaEzioBuffer* ptr = (LuaEzioBuffer*)lua_newuserdata(L, sizeof(LuaEzioBuffer));
	*ptr = &buf;
	luaL_setmetatable(L, skey_mt_buffer);
}

int ezio_buffer_create(lua_State* L)
{
	LuaEzioBuffer* ptr = (LuaEzioBuffer*)lua_newuserdata(L, sizeof(LuaEzioBuffer));
	*ptr = new ezio::Buffer();
	luaL_setmetatable(L, skey_mt_buffer);

	if (!lua_isnil(L, 1)) {
		auto* buf = lua_check_buffer(L, 1);
		int len = (int)lua_tointeger(L, 2);
		(*ptr)->Write(buf->Peek(), len);
	}
	return 1;
}

int ezio_buffer_destroy(lua_State* L)
{
	ezio::Buffer* ptr = lua_check_buffer(L, 1);
	delete ptr;
	return 0;
}

//EventLoop
using namespace ezio;
void ez_task_callback() {

}
void lua_push_ez_event_loop(lua_State* L, EventLoop* loop);

int ez_event_loop_run(lua_State* L) {
	auto* loop = lua_check_pointer<EventLoop>(L, 1);
	loop->Run();
	return 0;
};
int ez_event_loop_quit(lua_State* L) {
	auto* loop = lua_check_pointer<EventLoop>(L, 1);
	loop->Quit();
	return 0;
};

int ez_event_loop_queue_task(lua_State* L) {
	auto* loop = lua_check_pointer<EventLoop>(L, 1);
	//lua_pushvalue(L, 2);
	int ref = luaL_ref(L, LUA_REGISTRYINDEX);
	loop->QueueTask([L, ref]() {
		if (L) {
			lua_rawgeti(L, LUA_REGISTRYINDEX, ref);
			int res = lua_pcall(L, 0, 0, 0);
			check_lua_error(L, res);
			luaL_unref(L, LUA_REGISTRYINDEX, ref);
		}
		});
	return 0;
};

int ez_event_loop_run_task(lua_State* L) {
	auto* loop = lua_check_pointer<EventLoop>(L, 1);
	//lua_pushvalue(L, 2);
	int ref = luaL_ref(L, LUA_REGISTRYINDEX);
	loop->RunTask([L, ref]() {
		if (L) {
			lua_rawgeti(L, LUA_REGISTRYINDEX, ref);
			int res = lua_pcall(L, 0, 0, 0);
			check_lua_error(L, res);
			luaL_unref(L, LUA_REGISTRYINDEX, ref);
		}});
	return 0;
};

int ez_event_loop_run_task_at(lua_State* L) {
	L = L;
	/*auto* loop = lua_check_pointer<EventLoop>(L, 1);
	lua_pushvalue(L, 2);
	uint64_t when = (uint64_t)lua_tointeger(L, 3);
	int ref = luaL_ref(L, LUA_REGISTRYINDEX);
	loop->RunTaskAt([L, ref]() {
		if (L) {
			lua_rawgeti(L, LUA_REGISTRYINDEX, ref);
			int res = lua_pcall(L, 0, 0, 0);
			check_lua_error(L, res);
			luaL_unref(L, LUA_REGISTRYINDEX, ref);
		}
		}, ToTimePoint(when));
		*/
	return 0;
};
int ez_event_loop_run_task_after(lua_State* L) {
	auto* loop = lua_check_pointer<EventLoop>(L, 1);
	lua_pushvalue(L, 2);
	int delay = (int)lua_tointeger(L, 3);
	int ref = luaL_ref(L, LUA_REGISTRYINDEX);
	loop->RunTaskAfter([L, ref]() {
		if (L) {
			lua_rawgeti(L, LUA_REGISTRYINDEX, ref);
			int res = lua_pcall(L, 0, 0, 0);
			check_lua_error(L, res);
			luaL_unref(L, LUA_REGISTRYINDEX, ref);
		}
		}, TimeDuration(delay));

	return 0;
};

int ez_event_loop_run_task_every(lua_State* L) {
	auto* loop = lua_check_pointer<EventLoop>(L, 1);
	lua_pushvalue(L, 2);
	int ms = (int)lua_tointeger(L, 3);
	int ref = luaL_ref(L, LUA_REGISTRYINDEX);
	loop->RunTaskEvery([L, ref]() {
		if (L) {
			lua_rawgeti(L, LUA_REGISTRYINDEX, ref);
			int res = lua_pcall(L, 0, 0, 0);
			check_lua_error(L, res);
		}
		}, TimeDuration(ms));
	return 0;
};
int ez_event_loop_run_task_cancel_timed_task(lua_State* L) {
	L = L;
	return 0;
};
int ez_event_loop_belongs_to_current_thread(lua_State* L) {
	auto* loop = lua_check_pointer<EventLoop>(L, 1);
	auto ret = loop->BelongsToCurrentThread();
	lua_pushboolean(L, ret);
	return 1;
};
int ez_event_loop_register_notifier(lua_State* L) { L = L; return 0; };
int ez_event_loop_unregister_notifier(lua_State* L) { L = L; return 0; };

int ez_event_loop_wakeup(lua_State* L) {
	auto* loop = lua_check_pointer<EventLoop>(L, 1);
	loop->Wakeup();
	return 0;
};

luaL_Reg MT_EZ_EVENT_LOOP[] = {
	{ "Run",ez_event_loop_run },
	{ "Quit",ez_event_loop_quit },
	{ "QueueTask",ez_event_loop_queue_task },
	{ "RunTask",ez_event_loop_run_task },
	//{ "RunTaskAt",ez_event_loop_run_task_at },
	{ "RunTaskAfter",ez_event_loop_run_task_after },
	{ "RunTaskEvery",ez_event_loop_run_task_every },
	{ "CancelTimedTask",ez_event_loop_run_task_cancel_timed_task },
	{ "BelongsToCurrentThread",ez_event_loop_belongs_to_current_thread },
	{ "RegisterNotifier",ez_event_loop_register_notifier },
	{ "UnregisterNotifier",ez_event_loop_unregister_notifier },
	{ "Wakeup",ez_event_loop_wakeup },
	{ NULL, NULL}
};

void lua_push_ez_event_loop(lua_State* L, EventLoop* loop)
{
	lua_push_pointer(L, loop);
	if (luaL_newmetatable(L, "MT_EZ_EVENT_LOOP")) {
		luaL_setfuncs(L, MT_EZ_EVENT_LOOP, 0);
		lua_pushvalue(L, -1);
		lua_setfield(L, -2, "__index");
	}
	lua_setmetatable(L, -2);
}

int ez_event_loop_create(lua_State* L)
{
	lua_push_ez_event_loop(L, new EventLoop());
	return 1;
}

int ez_event_loop_destroy(lua_State* L) {
	auto* sp = lua_check_pointer<ezio::EventLoop>(L, 1);
	delete sp;
	return 0;
}

//TCPServer
int ez_tcp_server_start(lua_State* L) {
	auto* tcpserver = lua_check_pointer<TCPServer>(L, 1);
	tcpserver->Start();
	return 0;
}

int ez_tcp_server_ip_port(lua_State* L) {
	TCPServer* tcpserver = lua_check_pointer<TCPServer>(L, 1);
	auto ret = tcpserver->ip_port();
	lua_pushstring(L, ret.c_str());
	return 1;
}

int ez_tcp_server_name(lua_State* L) {
	TCPServer* tcpserver = lua_check_pointer<TCPServer>(L, 1);
	auto name = tcpserver->name();
	lua_pushstring(L, name.c_str());
	return 1;
}

int ez_tcp_server_set_on_connection(lua_State* L) {
	TCPServer* tcpserver = lua_check_pointer<TCPServer>(L, 1);
	//lua_pushvalue(L, 2);
	int ref = luaL_ref(L, LUA_REGISTRYINDEX);
	tcpserver->set_on_connection([L, ref](const ezio::TCPConnectionPtr& conn) {
		lua_rawgeti(L, LUA_REGISTRYINDEX, ref);
		lua_push_tcp_connection(L, conn);
		int res = lua_pcall(L, 1, 0, 0);
		check_lua_error(L, res);
		});
	return 0;
}
int ez_tcp_server_set_on_message(lua_State* L) {
	TCPServer* tcpserver = lua_check_pointer<TCPServer>(L, 1);
	//lua_pushvalue(L, 2);
	int ref = luaL_ref(L, LUA_REGISTRYINDEX);
	tcpserver->set_on_message([L, ref](const ezio::TCPConnectionPtr& conn, Buffer& buf, TimePoint ts) {
		lua_rawgeti(L, LUA_REGISTRYINDEX, ref);
		lua_push_tcp_connection(L, conn);
		lua_push_ezio_buffer(L, buf);
		uint64_t tm = ts.time_since_epoch().count() / 10000;
		lua_pushinteger(L, tm);
		int res = lua_pcall(L, 3, 0, 0);
		check_lua_error(L, res);
		});
	return 0;
}


static luaL_Reg MT_EZ_TCP_SERVER[] = {
	{ "Start",ez_tcp_server_start },
	{ "ip_port",ez_tcp_server_ip_port },
	{ "name",ez_tcp_server_name},
	{ "set_on_connection",ez_tcp_server_set_on_connection},
	{ "set_on_message",ez_tcp_server_set_on_message},
	{ NULL, NULL}
};

void lua_push_ez_tcp_server(lua_State* L, TCPServer* server)
{
	lua_push_pointer<TCPServer>(L, server);
	luaL_setmetatable(L, "MT_EZ_TCP_SERVER");
}

int ez_tcp_server_create(lua_State* L)
{
	EventLoop* loop = lua_check_pointer<EventLoop>(L, 1);
	auto port = (int)lua_tointeger(L, 2);
	auto name = lua_tostring(L, 3);
	SocketAddress addr((unsigned short)port);
	lua_push_ez_tcp_server(L, new TCPServer(loop, addr, name));
	return 1;
}

int ez_tcp_server_destroy(lua_State* L) {
	auto* sp = lua_check_pointer<TCPServer>(L, 1);
	delete sp;
	return 0;
}

//Client 

int ez_tcp_client_is_connected(lua_State* L) {
	auto* tcpclient = lua_check_pointer<TCPClient>(L, 1);
	auto conn = tcpclient->connection();
	lua_pushboolean(L, conn && conn->connected());
	return 1;
}

int ez_tcp_client_connect(lua_State* L) {
	auto* tcpclient = lua_check_pointer<TCPClient>(L, 1);
	tcpclient->Connect();
	return 0;
}

int ez_tcp_client_disconnect(lua_State* L) {
	auto* tcpclient = lua_check_pointer<TCPClient>(L, 1);
	tcpclient->Disconnect();
	return 0;
}

int ez_tcp_client_set_auto_reconnect(lua_State* L) {
	auto* tcpclient = lua_check_pointer<TCPClient>(L, 1);
	bool _auto = lua_toboolean(L, 2);
	tcpclient->set_auto_reconnect(_auto);
	return 0;
}

int ez_tcp_client_shutdown(lua_State* L) {
	auto* tcpclient = lua_check_pointer<TCPClient>(L, 1);
	tcpclient->connection()->Shutdown();
	return 0;
}


int ez_tcp_client_cancel(lua_State* L) {
	auto* tcpclient = lua_check_pointer<TCPClient>(L, 1);
	tcpclient->Cancel();
	return 0;
}

int ez_tcp_client_name(lua_State* L) {
	auto* tcpclient = lua_check_pointer<TCPClient>(L, 1);
	auto name = tcpclient->name();
	lua_pushstring(L, name.c_str());
	return 1;
}

int ez_tcp_client_connection(lua_State* L) {
	auto* tcpclient = lua_check_pointer<TCPClient>(L, 1);
	auto connection = tcpclient->connection();
	lua_push_tcp_connection(L, connection);
	return 1;
}

int ez_tcp_client_set_on_connection(lua_State* L) {
	TCPClient* tcpclient = lua_check_pointer<TCPClient>(L, 1);
	//lua_pushvalue(L, 2);
	int ref = luaL_ref(L, LUA_REGISTRYINDEX);
	tcpclient->set_on_connection([L, ref](const ezio::TCPConnectionPtr& conn) {
		lua_rawgeti(L, LUA_REGISTRYINDEX, ref);
		lua_push_tcp_connection(L, conn);
		int res = lua_pcall(L, 1, 0, 0);
		check_lua_error(L, res);
		});
	return 0;
}
int ez_tcp_client_set_on_message(lua_State* L) {
	TCPClient* tcpclient = lua_check_pointer<TCPClient>(L, 1);
	//lua_pushvalue(L, 2);
	int ref = luaL_ref(L, LUA_REGISTRYINDEX);
	tcpclient->set_on_message([L, ref](const ezio::TCPConnectionPtr& conn, Buffer& buf, TimePoint ts) {
		lua_rawgeti(L, LUA_REGISTRYINDEX, ref);
		lua_push_tcp_connection(L, conn);
		lua_push_ezio_buffer(L, buf);
		uint64_t tm = ts.time_since_epoch().count() / 10000;
		lua_pushinteger(L, tm);
		int res = lua_pcall(L, 3, 0, 0);
		check_lua_error(L, res);
		});
	return 0;
}

static luaL_Reg MT_EZ_TCP_CLIENT[] = {
	{ "IsConnected",ez_tcp_client_is_connected},
	{ "Connect",ez_tcp_client_connect},
	{ "Cancel",ez_tcp_client_cancel},
	{ "Disconnect",ez_tcp_client_disconnect},
	{ "set_auto_reconnect",ez_tcp_client_set_auto_reconnect},
	{ "Cancel",ez_tcp_client_cancel},
	{ "name",ez_tcp_client_name},
	{ "connection",ez_tcp_client_connection},
	{ "set_on_connection",ez_tcp_client_set_on_connection},
	{ "set_on_message",ez_tcp_client_set_on_message},
	{ NULL, NULL}
};

void lua_push_ez_tcp_client(lua_State* L, TCPClient* client)
{
	lua_push_pointer<TCPClient>(L, client);
	luaL_setmetatable(L, "MT_EZ_TCP_CLIENT");
}

int ez_tcp_client_create(lua_State* L)
{
	EventLoop* loop = lua_check_pointer<EventLoop>(L, 1);
	auto ip = lua_tostring(L, 2);
	auto port = (int)lua_tointeger(L, 3);
	auto name = lua_tostring(L, 4);
	SocketAddress addr(ip, (unsigned short)port);
	lua_push_ez_tcp_client(L, new TCPClient(loop, addr, name));
	return 1;
}

int ez_tcp_client_destroy(lua_State* L) {
	auto* sp = lua_check_pointer<TCPClient>(L, 1);
	delete sp;
	return 0;
}

int newthread_dofile(lua_State* L) {
	const char* path = lua_tostring(L, 1);
	new std::thread([path]() {
		lua_State* _L = luaL_newstate();
		luaL_openlibs(_L);
		luaopen_cxlua(_L);
		int res = luaL_dofile(_L, path);
		check_lua_error(_L, res);
		lua_close(_L);
		});
	return 0;
}

int newthread_dostring(lua_State* L) {
	const char* code = lua_tostring(L, 1);
	new std::thread([code]() {
		lua_State* _L = luaL_newstate();
		luaL_openlibs(_L);
		luaopen_cxlua(_L);
		int res = luaL_dostring(_L, code);
		check_lua_error(_L, res);
		lua_close(_L);
		});
	return 0;
}

void io_service_context_init() {
	ezio::IOServiceContext::Init();
}

void at_exit_manager_init() {
	new kbase::AtExitManager();
}

int at_exit_manager_regcb(lua_State* L) {
	int ref = luaL_ref(L, LUA_REGISTRYINDEX);
	kbase::AtExitManager::RegisterCallback([&L,&ref] {
		lua_rawgeti(L, LUA_REGISTRYINDEX, ref);
		int res = lua_pcall(L, 1, 0, 0);
		check_lua_error(L, res);
		});
	return 0;
}

#define register_luac_function(L, fn) (lua_pushcfunction(L, (fn)), lua_setglobal(L, #fn))
void luaopen_ezio_netlib(lua_State* L)
{
	if (luaL_newmetatable(L, skey_mt_tcp_connection)) {
		luaL_setfuncs(L, mt_tcp_connection_reg, 0);
		lua_setfield(L, -1, "__index");
	}
	else {
		std::cout << "associate mt_tcp_connection error!" << std::endl;
	}

	if (luaL_newmetatable(L, skey_mt_buffer)) {
		luaL_setfuncs(L, mt_buffer_reg, 0);
		lua_setfield(L, -1, "__index");
	}
	else {
		std::cout << "associate mt_buffer error!" << std::endl;
	}

	if (luaL_newmetatable(L, "MT_EZ_TCP_SERVER")) {
		luaL_setfuncs(L, MT_EZ_TCP_SERVER, 0);
		lua_setfield(L, -1, "__index");
	}

	if (luaL_newmetatable(L, "MT_EZ_TCP_CLIENT")) {
		luaL_setfuncs(L, MT_EZ_TCP_CLIENT, 0);
		lua_setfield(L, -1, "__index");
	}

	register_luac_function(L, ezio_buffer_create);
	register_luac_function(L, ezio_buffer_destroy);

	register_luac_function(L, ez_event_loop_create);
	register_luac_function(L, ez_event_loop_destroy);

	register_luac_function(L, ez_tcp_server_create);
	register_luac_function(L, ez_tcp_server_destroy);

	register_luac_function(L, ez_tcp_client_create);
	register_luac_function(L, ez_tcp_client_destroy);

	register_luac_function(L, newthread_dofile);
	register_luac_function(L, newthread_dostring);
	script_system_register_function(L, io_service_context_init);
	script_system_register_function(L, at_exit_manager_init);
	register_luac_function(L, at_exit_manager_regcb);

}
