#include "net_thread_queue.h"
#include <assert.h>
#include <iostream>

#include "lua_net.h"
void NetThreadQueue::PushBack(int q,const char* data, size_t len)
{
	assert(checkq(q));
	m_Locks[q].lock();
	m_Queues[q].emplace_back(len);
	ezio::Buffer& it = m_Queues[q].back();
	it.Write(data, len);
	m_Locks[q].unlock();
}

ezio::Buffer& NetThreadQueue::Front(int q)
{
	assert(checkq(q));
	m_Locks[q].lock();
	ezio::Buffer& f = m_Queues[q].front();
	m_Locks[q].unlock();
	return f;
}

void NetThreadQueue::PopFront(int q)
{
	assert(checkq(q));
	m_Locks[q].lock();
	m_Queues[q].pop_front();
	m_Locks[q].unlock();
}

int NetThreadQueue::Size(int q)
{
	assert(checkq(q));
	m_Locks[q].lock();
	int size = (int)m_Queues[q].size();
	m_Locks[q].unlock();
	return size;
}

bool NetThreadQueue::Empty(int q)
{
	assert(checkq(q));
	m_Locks[q].lock();
	auto res = m_Queues[q].empty();
	m_Locks[q].unlock();
	return res;
}

void NetThreadQueue::Clear(int q)
{
	m_Locks[q].lock();
	m_Queues[q].clear();
	m_Locks[q].unlock();
}



NetThreadQueue* lua_check_net_thread_queue(lua_State*L, int index)
{
	NetThreadQueue** ptr = (NetThreadQueue**)lua_touserdata(L, index);
	return *ptr;
} 
 
int net_thread_queue_push_back(lua_State* L)
{
	auto* ptr = lua_check_net_thread_queue(L, 1);
	int q = (int)lua_tointeger(L, 2);
	ezio::Buffer* buffer = (ezio::Buffer*)lua_check_buffer(L, 3);
	size_t len = (size_t)lua_tointeger(L, 4);
	ptr->PushBack(q, buffer->Peek(), len);
	return 0;
}

int net_thread_queue_pop_front(lua_State* L)
{
	auto* ptr = lua_check_net_thread_queue(L, 1);
	int q = (int)lua_tointeger(L, 2);
	ptr->PopFront(q);
	return 0;
}


int net_thread_queue_front(lua_State* L)
{
	auto* ptr = lua_check_net_thread_queue(L, 1);
	int q = (int)lua_tointeger(L, 2);
	auto& buf = ptr->Front(q);
	lua_push_ezio_buffer(L, buf);
	return 1;
}

int net_thread_queue_front_as_string(lua_State* L)
{
	auto* ptr = lua_check_net_thread_queue(L, 1);
	int q = (int)lua_tointeger(L, 2);
	auto& buf = ptr->Front(q);
	std::string s(buf.Peek(), buf.readable_size());
	lua_pushstring(L, s.c_str());
	return 1;
}


int net_thread_queue_size(lua_State* L)
{
	auto* ptr = lua_check_net_thread_queue(L, 1);
	int q = (int)lua_tointeger(L, 2);
	int size = ptr->Size(q);
	lua_pushinteger(L, size);
	return 1;
}

int net_thread_queue_empty(lua_State* L)
{
	auto* ptr = lua_check_net_thread_queue(L, 1);
	int q = (int)lua_tointeger(L, 2);
	bool empty = ptr->Empty(q);
	lua_pushboolean(L, empty);
	return 1;
}

int net_thread_queue_clear(lua_State* L)
{
	auto* ptr = lua_check_net_thread_queue(L, 1);
	int q = (int)lua_tointeger(L, 2);
	ptr->Clear(q);
	return 0;
}
luaL_Reg MT_NET_THREAD_QUEUE[] = {
	{ "push_back",net_thread_queue_push_back },
	{ "pop_front",net_thread_queue_pop_front },
	{ "front_as_string",net_thread_queue_front_as_string},
	{ "front",net_thread_queue_front },
	{ "size",net_thread_queue_size },
	{ "empty",net_thread_queue_empty },
	{ "clear",net_thread_queue_clear },
	{ NULL, NULL }
};
void lua_push_net_thread_queue(lua_State*L, NetThreadQueue* q)
{
	lua_push_pointer(L, q);
	if (luaL_newmetatable(L, "MT_NET_THREAD_QUEUE")) {
		luaL_setfuncs(L, MT_NET_THREAD_QUEUE, 0);
		lua_pushvalue(L, -1);
		lua_setfield(L, -2, "__index");
	}
	lua_setmetatable(L, -2);
}
int net_thread_queue_create(lua_State* L) {
	lua_push_net_thread_queue(L, new NetThreadQueue());
	return 1;
}

int net_thread_queue_destroy(lua_State* L) {
	auto* sp = lua_check_pointer<NetThreadQueue>(L, 1);
	delete sp;
	return 0;
}

#define register_luac_function(L, fn) (lua_pushcfunction(L, (fn)), lua_setglobal(L, #fn))
void luaopen_net_thread_queue(lua_State* L)
{
	/*if (luaL_newmetatable(L, "mt_net_thread_queue")) {
		luaL_setfuncs(L, mt_net_thread_queue, 0);
		lua_setfield(L, -1, "__index");
	}
	else {
		std::cout << "associate mt_net_thread_queue error!" << std::endl;
	}*/
	register_luac_function(L, net_thread_queue_create);
	register_luac_function(L, net_thread_queue_destroy);
}
