#pragma once
#include "singleton.h"
#include "ezio/event_loop.h"
#include "kbase/basic_macros.h"
#include "ezio/socket_address.h"
#include "ezio/tcp_connection.h"
#include "ezio/tcp_server.h"
using namespace ezio;



class GameServer 
{
public:
	GameServer(EventLoop* loop, SocketAddress addr,const char* name);
	~GameServer();

	DISALLOW_COPY(GameServer);
	DISALLOW_MOVE(GameServer);

	void Start();
	void Stop();
	
	void OnClose();
	void SendMessageToPlayer(uint64_t pid, int proto, const char* msg);
	void SendMessageToPlayers(std::vector<uint64_t> pids, int proto, const char* msg);

	EventLoop* GetLoop() { return m_EventLoop; }
private:
	

	void OnConnection(const TCPConnectionPtr& conn);
	void OnMessage(const TCPConnectionPtr& conn, Buffer& buf, TimePoint ts);
	int m_Port;
	EventLoop* m_EventLoop;
	TCPServer m_Server;
	lua_State* m_L;
};


void game_server_start(int port);
void game_server_stop();

void luaopen_game_server(lua_State* L);