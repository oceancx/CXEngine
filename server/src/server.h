#pragma once
#include "singleton.h"
#include "kbase/basic_macros.h"
#include "asio.hpp"




class GameServer 
{
public:
	GameServer(int port);
	~GameServer();


	void Start();
	void Stop();
	
	void OnClose();
	void SendMessageToPlayer(uint64_t pid, int proto, const char* msg);
	void SendMessageToPlayers(std::vector<uint64_t> pids, int proto, const char* msg);

	void Run();
private:
	
	void DoAccept();
	int m_Port;
	lua_State* m_L;
	asio::ip::tcp::acceptor m_Acceptor;
	std::thread* m_Thread;
	
};


void game_server_start(int port);
void game_server_stop();

void luaopen_game_server(lua_State* L);