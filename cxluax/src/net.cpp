#include "lua_net.h"
#include <iostream>
 
#include <chrono>
#include "cxlua.h"
#include "cxezio/buffer.h"
#include "logger.h"

#include "net_thread_queue.h"
using namespace cxezio;

using asio::ip::tcp;

NetThreadQueue g_ReadPacketQueue;
static bool g_ServerConnected = false;
static bool g_LastServerConnected = false;


class CXClient
{
public:
	CXClient(string ip, string port)
		:m_Socket(m_IOContext), m_IP(ip), m_Port(port), m_Thread(nullptr)
	{
		m_Buffer.ReserveWritable(40960);
	}

	void Connect()
	{
		m_IOContext.restart();
		tcp::resolver resolver(m_IOContext);
		auto endpoints = resolver.resolve(m_IP, m_Port);
		asio::async_connect(m_Socket, endpoints, std::bind(&CXClient::OnConnected, this, std::placeholders::_1, std::placeholders::_2));
		m_Thread = new std::thread([this]() { m_IOContext.run(); });
	}

	void DisConnect()
	{
		if (m_Socket.is_open()) {
			printf("DisConnect\n");
			m_Socket.close();
		}
		if (m_Thread != nullptr && m_Thread->joinable()) {
			m_Thread->join();
			m_Thread = nullptr;
		}
	}

	void OnConnected(std::error_code ec, tcp::endpoint endpoint)
	{
		g_ServerConnected = !ec ? true : false;
		if (!ec) {
			cxlog_info("Connection is done\n");
			ReadHeader();
		}
		else {
			cxlog_info("Connection is error\n");

		}
	}

	void SendMsgToServer(const  Buffer buf) {
		asio::post(m_IOContext,
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

	~CXClient() {
		printf("Destroy\n");
	}
private:
	void ReadHeader() {
		asio::async_read(m_Socket,
			asio::buffer(m_Buffer.BeginWrite(), 4),
			[this](std::error_code ec, std::size_t /*length*/)
			{
				if (!ec)
				{
					m_Buffer.EndWrite(4);
					ReadPackage();
				}
				else
				{
					m_Socket.close();
				}
			});
	}

	void ReadPackage() {
		int len = m_Buffer.ReadAsInt32();
		printf("read len %d\n", len);
		asio::async_read(m_Socket,
			asio::buffer(m_Buffer.BeginWrite(), len),
			[this, len](std::error_code ec, std::size_t /*length*/)
			{
				if (!ec)
				{
					m_Buffer.EndWrite(len);
					g_ReadPacketQueue.PushBack(NetThreadQueue::Read, m_Buffer.Peek(), len);
					m_Buffer.Consume(len);
					ReadHeader();
				}
				else
				{
					m_Socket.close();
				}
			});
	}

	std::thread* m_Thread;
	asio::io_context m_IOContext;
	tcp::socket m_Socket;
	std::string m_IP;
	std::string m_Port;
	Buffer m_Buffer;

};
CXClient* _CXClient = nullptr;




void net_manager_init(const char* ip, int port)
{
	_CXClient = new CXClient(ip, std::to_string(port));
	_CXClient->Connect();
}

int net_manager_update(lua_State* L)
{
	if (g_LastServerConnected != g_ServerConnected) {
		g_LastServerConnected = g_ServerConnected;
		lua_getglobal(L, "game_server_on_connection");
		lua_pushboolean(L, g_ServerConnected);
		int res = lua_pcall(L, 1, 0, 0);
		check_lua_error(L, res);
	}
	while (!g_ReadPacketQueue.Empty(NetThreadQueue::Read))
	{
		Buffer& pt = g_ReadPacketQueue.Front(NetThreadQueue::Read);
		lua_getglobal(L, "game_dispatch_message");
		lua_push_cxezio_buffer(L, pt);
		int res = lua_pcall(L, 1, 0, 0);
		check_lua_error(L, res);
		g_ReadPacketQueue.PopFront(NetThreadQueue::Read);
	}

	return 0;
}

void net_manager_deinit()
{
	_CXClient->DisConnect();
	delete _CXClient;
	_CXClient = nullptr;
}

void net_connect_to_server()
{

}

void net_manager_connect()
{
	if (_CXClient != nullptr) {
		_CXClient->Connect();
	}
}

void net_send_message_in_c(int proto, const char* msg) {
	Buffer buf;
	buf.Write(proto);
	buf.Write(msg, strlen(msg));
	int cnt = (int)buf.readable_size();
	buf.Prepend(cnt);
	_CXClient->SendMsgToServer(buf);
}


void net_manager_reconnect() {
	if (_CXClient != nullptr) {
		_CXClient->DisConnect();
		_CXClient->Connect();
	}
}

int net_get_shared_queue(lua_State* L)
{
	lua_push_net_thread_queue(L, &g_ReadPacketQueue);
	return 1;
}

int net_send_buffer_in_c(lua_State* L)
{
	auto* buf = lua_check_cxezio_buffer(L, 1);
	if (_CXClient != nullptr) {
		_CXClient->SendMsgToServer(*buf);
	}
	return 0;
}

#define register_luac_function(L, fn) (lua_pushcfunction(L, (fn)), lua_setglobal(L, #fn))
void luaopen_cxluax_netlib(lua_State* L)
{
	script_system_register_function(L, net_manager_init);
	script_system_register_luac_function(L, net_manager_update);
	script_system_register_function(L, net_manager_deinit);

	script_system_register_function(L, net_manager_reconnect);
	script_system_register_function(L, net_send_message_in_c);

	script_system_register_luac_function(L, net_get_shared_queue);
	script_system_register_luac_function(L, net_send_buffer_in_c);


}
