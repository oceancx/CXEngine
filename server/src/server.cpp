#include "server.h"
#include "tsv.h"
#include "protocol.h"
#include "cxezio/endian_utils.h"
#include "cxezio/buffer.h"
#include "logger.h"
#include <script_system.h>
#include "net_thread_queue.h"
#include "lua_net.h"
#include "file_system.h"
#include "cxlua.h"
#include "asio.hpp"

#include <ctime>
#include <chrono>

using namespace cxezio;
// A custom implementation of the Clock concept from the standard C++ library.
struct time_t_clock
{
	// The duration type.
	typedef std::chrono::steady_clock::duration duration;

	// The duration's underlying arithmetic representation.
	typedef duration::rep rep;

	// The ratio representing the duration's tick period.
	typedef duration::period period;

	// An absolute time point represented using the clock.
	typedef std::chrono::time_point<time_t_clock> time_point;

	// The clock is not monotonically increasing.
	static constexpr bool is_steady = false;

	// Get the current time.
	static time_point now() noexcept
	{
		return time_point() + std::chrono::seconds(std::time(0));
	}
};

// The asio::basic_waitable_timer template accepts an optional WaitTraits
// template parameter. The underlying time_t clock has one-second granularity,
// so these traits may be customised to reduce the latency between the clock
// ticking over and a wait operation's completion. When the timeout is near
// (less than one second away) we poll the clock more frequently to detect the
// time change closer to when it occurs. The user can select the appropriate
// trade off between accuracy and the increased CPU cost of polling. In extreme
// cases, a zero duration may be returned to make the timers as accurate as
// possible, albeit with 100% CPU usage.
struct time_t_wait_traits
{
	// Determine how long until the clock should be next polled to determine
	// whether the duration has elapsed.
	static time_t_clock::duration to_wait_duration(
		const time_t_clock::duration& d)
	{
		if (d > std::chrono::seconds(1))
			return d - std::chrono::seconds(1);
		else if (d > std::chrono::seconds(0))
			return std::chrono::milliseconds(10);
		else
			return std::chrono::seconds(0);
	}

	// Determine how long until the clock should be next polled to determine
	// whether the absoluate time has been reached.
	static time_t_clock::duration to_wait_duration(
		const time_t_clock::time_point& t)
	{
		return to_wait_duration(t - time_t_clock::now());
	}
};
using asio::ip::tcp;
typedef asio::basic_waitable_timer<
	time_t_clock, time_t_wait_traits> time_t_timer;

NetThreadQueue g_ReadPacketQueue;


GameServer* CXGameServer = NULL;

asio::io_context _IOContext;


int insert_pid_connection_pair(lua_State* L);

int erase_pid_connection_pair(lua_State* L);

#define luaL_requirelib(L,name,fn) (luaL_requiref(L, name, fn, 1),lua_pop(L, 1))
extern "C"  int luaopen_cjson(lua_State * L);

void thread_init_script_system(lua_State* L) {


}


class GameSession
{
public:
	GameSession(tcp::socket socket)
		:m_Socket(std::move(socket))
	{
		m_Buffer.ReserveWritable(40960);
		ReadHeader();
	}

	void Send(const Buffer buf) {
		asio::post(_IOContext,
			[this, buf]()
			{
				asio::async_write(m_Socket,
					asio::buffer(buf.Peek(),
						buf.readable_size()),
					[this](std::error_code ec, std::size_t /*length*/)
					{
						if (!ec)
						{
							printf("write msg ok");
						}
						else
						{
							m_Socket.close();
						}
					});
			});
	}
private:
	void ReadHeader()
	{
		asio::async_read(m_Socket,
			asio::buffer(m_Buffer.BeginWrite(), 4),
			[this](std::error_code ec, std::size_t /*length*/)
			{
				if (!ec)
				{
					m_Buffer.EndWrite(4);
					ReadPack();
				}
				else
				{

				}
			});
	}
	void ReadPack()
	{
		int len = m_Buffer.ReadAsInt32();
		asio::async_read(m_Socket,
			asio::buffer(m_Buffer.BeginWrite(), len),
			[this, len](std::error_code ec, std::size_t /*length*/)
			{
				if (!ec)
				{
					m_Buffer.EndWrite(len);

					auto* m_L = script_system_get_luastate();
					lua_getglobal(m_L, "server_thread_on_message");
					lua_pushlightuserdata(m_L, this);
					lua_push_cxezio_buffer(m_L, m_Buffer);
					lua_pushinteger(m_L, len);
					lua_push_net_thread_queue(m_L, &g_ReadPacketQueue);
					int res = lua_pcall(m_L, 4, 0, 0);
					check_lua_error(m_L, res);

					ReadHeader();
				}
				else
				{

				}
			});
	}
	tcp::socket m_Socket;
	Buffer m_Buffer;
};
std::map<uint64_t, GameSession*> g_PlayerConnections;

GameServer::GameServer(int port)
	:m_Port(port), m_Acceptor(_IOContext, tcp::endpoint(tcp::v4(), m_Port))
{
	m_L = script_system_get_luastate();

	/*m_Server.set_on_connection(std::bind(&GameServer::OnConnection, this, std::placeholders::_1));
	m_Server.set_on_message(std::bind(&GameServer::OnMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));*/
	//cxlog_info("server init at %s\n",addr.ToHostPort().c_str());
}

GameServer::~GameServer()
{

}

void GameServer::DoAccept()
{
	m_Acceptor.async_accept(
		[this](std::error_code ec, tcp::socket socket)
		{
			if (!ec)
			{
				new GameSession(std::move(socket));
				//std::make_shared<GameSession>();
			}
			DoAccept();
		});
}

void main_game_update() {
	time_t_timer timer(_IOContext);
	timer.expires_after(std::chrono::milliseconds(16));
	timer.async_wait(
		[](const std::error_code& /*error*/)
		{
			script_system_update();
			main_game_update();
			//std::cout << "timeout\n";
		});
}


void GameServer::Start()
{
	//m_EventLoop->RunTaskEvery(main_game_update, TimeDuration(16));
	script_system_register_luac_function(m_L, insert_pid_connection_pair);
	script_system_register_luac_function(m_L, erase_pid_connection_pair);
	_IOContext.restart();
	DoAccept();

	
	main_game_update();
	_IOContext.run();


	
}

void GameServer::Stop()
{
	lua_close(m_L);
	cxlog_info("server stop\n");

	//m_EventLoop->Quit();
	m_Thread->join();
}


void GameServer::Run()
{
}

void GameServer::OnClose()
{
	lua_getglobal(m_L, "save_player_database");
	int res = lua_pcall(m_L, 1, 0, 0);
	check_lua_error(m_L, res);
}

void GameServer::SendMessageToPlayer(uint64_t pid, int proto, const char* msg)
{

	auto it = g_PlayerConnections.find(pid);
	if (it != g_PlayerConnections.end())
	{
		Buffer buf;
		buf.Write(proto);
		buf.Write(msg, strlen(msg));
		buf.Prepend((int)buf.readable_size());
		it->second->Send(buf);
	}

}

void GameServer::SendMessageToPlayers(std::vector<uint64_t> pids, int proto, const char* msg)
{
	Buffer buf;
	buf.Write(proto);
	buf.Write(msg, strlen(msg));
	buf.Prepend((int)buf.readable_size());
	for (auto pid : pids) {
		auto it = g_PlayerConnections.find(pid);
		if (it != g_PlayerConnections.end())
		{
			it->second->Send(buf);
		}
	}
}

 
void game_server_start(int port) {

	CXGameServer = new GameServer(port);
	CXGameServer->Start();
}

int game_server_update(lua_State* L) {

	while (!g_ReadPacketQueue.Empty(NetThreadQueue::Read))
	{
		Buffer& pt = g_ReadPacketQueue.Front(NetThreadQueue::Read);
		lua_getglobal(L, "game_server_dispatch_message");
		lua_push_cxezio_buffer(L, pt);
		int res = lua_pcall(L, 1, 0, 0);
		check_lua_error(L, res);
		g_ReadPacketQueue.PopFront(NetThreadQueue::Read);
	}
	return 0;
}

void game_server_stop()
{
	CXGameServer->Stop();
}


int net_send_message_in_c(lua_State* L) {
	uint64_t pid = (uint64_t)lua_tointeger(L, 1);
	int proto = (int)lua_tointeger(L, 2);
	const char* msg = lua_tostring(L, 3);
	CXGameServer->SendMessageToPlayer(pid, proto, msg);
	return 0;
}



int net_send_message_to_players_in_c(lua_State* L) {
	std::vector<uint64_t> pids;
	int len = (int)luaL_len(L, 1);
	int i = 0;
	lua_pushnil(L);
	while (lua_next(L, 1) != 0) {
		uint64_t pid = (uint64_t)lua_tointeger(L, -1);
		pids.push_back(pid);
		i++;
		lua_pop(L, 1);
	}
	int proto = (int)lua_tointeger(L, 2);
	const char* msg = lua_tostring(L, 3);
	CXGameServer->SendMessageToPlayers(pids, proto, msg);
	return 0;
}

int net_send_message_to_all_players_in_c(lua_State* L) {
	int proto = (int)lua_tointeger(L, 1);
	const char* msg = lua_tostring(L, 2);
	Buffer buf;
	buf.Write(proto);
	buf.Write(msg, strlen(msg));
	buf.Prepend((int)buf.readable_size());
	for (auto& it : g_PlayerConnections) {
		it.second->Send(buf);
	}
	return 0;
}

int insert_pid_connection_pair(lua_State* L) {
	uint64_t pid = (uint64_t)lua_tointeger(L, 1);
	GameSession* conn = (GameSession*)lua_touserdata(L, 2);
	if (g_PlayerConnections.find(pid) != g_PlayerConnections.end()) {
		return 0;
	}
	g_PlayerConnections.insert({ pid,conn });
	return 0;
}

int erase_pid_connection_pair(lua_State* L) {
	uint64_t pid = (uint64_t)lua_tointeger(L, 1);
	g_PlayerConnections.erase(pid);
	return 0;
}

int game_server_send_buffer(lua_State* L) {
	GameSession* session = (GameSession*)lua_touserdata(L, 1);
	auto* buffer = lua_check_cxezio_buffer(L, 2);
	session->Send(*buffer);
	return 0;
}

void luaopen_game_server(lua_State* L)
{
	script_system_register_luac_function(L, net_send_message_in_c);
	script_system_register_luac_function(L, net_send_message_to_players_in_c);
	script_system_register_luac_function(L, net_send_message_to_all_players_in_c);

	script_system_register_function(L, game_server_start);
	script_system_register_luac_function(L, game_server_update);
	script_system_register_function(L, game_server_stop);

	script_system_register_luac_function(L, game_server_send_buffer);
}

