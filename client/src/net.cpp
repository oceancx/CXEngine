#include "net.h"

#include <stdio.h>
#include <script_system.h>
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
#include "ezio/tcp_client.h"

#include "scene/base_scene.h"
#include "kbase/pickle.h"
#include "scene/scene_manager.h"
#include "logger.h"
#include "net_thread_queue.h"
#include "lua_net.h"
#include "cxlua.h"

using namespace ezio;

NetThreadQueue g_ReadPacketQueue;
static bool g_ServerConnected = false;
static bool g_LastServerConnected = false;

class NetClient
{
public:
	NetClient(EventLoop* loop, const SocketAddress& serverAddr, const char* name);

	~NetClient();
	void Connect();

	void SendMessageToServer(int proto, const char* msg);

	void Disconnect();

private:
	void OnConnection(const TCPConnectionPtr&);

	void OnMessage(const TCPConnectionPtr&, Buffer&, TimePoint);

	TCPClient m_Client;
	EventLoop* m_EventLoop;
};


NetClient::NetClient(EventLoop* loop, const SocketAddress& serverAddr,const char* name)
	:m_Client(loop, serverAddr, name),m_EventLoop(loop)
{
	m_Client.set_on_connection(std::bind(&NetClient::OnConnection, this, std::placeholders::_1));
	m_Client.set_on_message(std::bind(&NetClient::OnMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
}

NetClient::~NetClient()
{

}

void NetClient::Connect()
{
	m_Client.Connect();
}

void NetClient::SendMessageToServer(int proto, const char* msg)
{
	ezio::Buffer buf;
	buf.Write(proto);
	buf.Write(msg, strlen(msg));
	int cnt = (int)buf.readable_size();
	buf.Prepend(cnt);
	m_EventLoop->RunTask([this, buf]() {
		if (m_Client.connection() != nullptr && m_Client.connection()->connected()) {
			m_Client.connection()->Send(kbase::StringView(buf.Peek(), buf.readable_size()));
		}
	});
}

void NetClient::Disconnect()
{
	if(m_Client.connection() != nullptr){
		m_Client.Disconnect();
	}
}
void NetClient::OnConnection(const TCPConnectionPtr& conn)
{
	const char* state = conn->connected() ? "connected" : "disconnected";
	cxlog_info("Connection %s is %s\n", conn->peer_addr().ToHostPort().c_str(), state);
	g_ServerConnected = conn->connected();
}

void NetClient::OnMessage(const TCPConnectionPtr& conn, Buffer& buf, TimePoint time)
{

	while (buf.readable_size() >= CX_MSG_HEADER_LEN)
	{
		int len = buf.PeekAsInt32();
		if (buf.readable_size() >= len + CX_MSG_HEADER_LEN)
		{
			buf.Consume(CX_MSG_HEADER_LEN);
			g_ReadPacketQueue.PushBack(NetThreadQueue::Read, buf.Peek(), len);
			buf.Consume(len);
		}
		else
		{
			break;
		}
	}
}

NetClient* g_Client = nullptr;
EventLoop* g_Loop = nullptr;

class NetThread : public Singleton<NetThread>
{
public:
	NetThread();
	~NetThread();
	void Init(const char* ip, int port);
	void Update(lua_State* L);
	void Deinit();
	void Run(const char* ip, int port);
private:
	std::thread* m_Thread;
};

NetThread::NetThread()
{
	
}

NetThread::~NetThread()
{
	
}

void NetThread::Run(const char* ip,int port)
{
	EventLoop loop;
	g_Loop = &loop;
	SocketAddress addr(ip, port);
	NetClient client(&loop,addr,"Client");
	g_Client = &client;
	client.Connect();
	loop.Run();
}

void NetThread::Init(const char* ip, int port)
{
	m_Thread = new std::thread(std::bind(&NetThread::Run, this , std::placeholders::_1,std::placeholders::_2), ip, port);
}

void NetThread::Update(lua_State* L )
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
		lua_getglobal(L , "game_dispatch_message");
		lua_push_ezio_buffer(L, pt);
		int res = lua_pcall(L, 1, 0, 0);
		check_lua_error(L, res);
		g_ReadPacketQueue.PopFront(NetThreadQueue::Read);
	}

}

void NetThread::Deinit()
{
	g_Client->Disconnect();
	g_Loop->Quit();
	m_Thread->join();
	m_Thread = nullptr;
}

void net_manager_init(const char* ip, int port)
{
	NetThread::GetInstance()->Init(ip, port);
} 

int net_manager_update(lua_State*L)
{
	NetThread::GetInstance()->Update(L);
	return 0;
}

void net_manager_deinit()
{
	NetThread::GetInstance()->Deinit();
}

void net_connect_to_server()
{

}

void net_manager_connect()
{
	if (g_Client != nullptr)
	{
		g_Client->Connect();
	}
}

void net_send_message_in_c(int proto, const char* msg) {
	g_Client->SendMessageToServer(proto, msg);
}


void net_manager_reconnect(){
	g_Client->Disconnect();
	g_Client->Connect();
}


void luaopen_net(lua_State* L)
{

	script_system_register_function(L, net_manager_init);
	script_system_register_luac_function(L, net_manager_update);
	script_system_register_function(L, net_manager_deinit);

	script_system_register_function(L, net_manager_reconnect);
	script_system_register_function(L, net_send_message_in_c);	
}
