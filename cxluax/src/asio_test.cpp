#include "asio_test.h"

#include <cstdlib>
#include <cstring>
#include <iostream>
#include "asio.hpp"
#include <ezio/buffer.h>
#include <net_thread_queue.h>

using asio::ip::tcp;
NetThreadQueue g_ReadPacketQueue;

class CXClient
{
public:
	CXClient(string ip, string port)
		:m_Socket(m_IOContext), m_IP(ip), m_Port(port), m_Thread(nullptr)
	{
		printf("OnCreate");
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
		asio::post(m_IOContext, [this]() {
			printf("DisConnect");
			m_Socket.close();
			});

		if (m_Thread != nullptr && m_Thread->joinable()) {
			m_Thread->join();
			m_Thread = nullptr;
		}
	}

	void OnConnected(std::error_code ec, tcp::endpoint endpoint)
	{
		if (!ec) {
			printf("OnConnected %s\n", ec);
			ReadHeader();
		}
	}

	void SendMsgToServer(const ezio::Buffer buf) {
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
	ezio::Buffer m_Buffer;

};

enum { max_length = 1024 };

void net_send_message(int pt, string msg)
{
	printf("net_send_message");
	ezio::Buffer buf;
	buf.Write(pt);
	buf.Write(msg.data(), msg.size());
	int cnt = buf.readable_size();
	buf.Prepend(cnt);
}

bool wrap_message(int pt, string msg, ezio::Buffer& buf)
{
	buf.Write(pt);
	buf.Write(msg.data(), msg.size());
	int cnt = buf.readable_size();
	buf.Prepend(cnt);
	return true;
}


CXClient* _CXClient;
void cxclient_disconnect()
{
	_CXClient->DisConnect();
}
void cxclient_connect() {
	_CXClient->Connect();
}

void cxclient_create()
{
	_CXClient = new CXClient("127.0.0.1", "45000");
}

void cxclient_destroy()
{
	delete _CXClient;
	_CXClient = nullptr;
}

void cxclient_update() {
	while (!g_ReadPacketQueue.Empty(NetThreadQueue::Read))
	{
		ezio::Buffer& pt = g_ReadPacketQueue.Front(NetThreadQueue::Read);
		int type = pt.ReadAsInt32();
		std::string js = pt.ReadAllAsString();
		printf("type:%d\n%s\n", type, js.c_str());
		g_ReadPacketQueue.PopFront(NetThreadQueue::Read);
	}
}
void cxclient_sendmsg(int proto, const char* msg)
{
	ezio::Buffer buf;
	buf.Write(proto);
	buf.Write(msg, strlen(msg));
	int cnt = (int)buf.readable_size();
	buf.Prepend(cnt);
	_CXClient->SendMsgToServer(buf);
}

void luaopen_asio_test(lua_State* L)
{
	script_system_register_function(L, cxclient_disconnect);
	script_system_register_function(L, cxclient_connect);
	script_system_register_function(L, cxclient_create);
	script_system_register_function(L, cxclient_destroy);
	script_system_register_function(L, cxclient_update);
	script_system_register_function(L, cxclient_sendmsg);

}