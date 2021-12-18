#include "connection_manager.hpp"
#include <algorithm>

#include <cstdlib>
#include <iostream>

#include "../essential/utility/strutil.h"
#include "../essential/check_function.h"
#include <ikcp.h>
#include "../util/connect_packet.hpp"
#include "asio_kcp_log.hpp"



/* get system time */
static inline void itimeofday(long* sec, long* usec)
{
	auto dur = std::chrono::system_clock::now().time_since_epoch();
	*sec = std::chrono::duration_cast<std::chrono::seconds>(dur).count();
	*usec = std::chrono::duration_cast<std::chrono::microseconds>(dur).count();
}

/* get clock in millisecond 64 */
static inline IINT64 iclock64(void)
{
	long s, u;
	IINT64 value;
	itimeofday(&s, &u);
	value = ((IINT64)s) * 1000 + (u / 1000);
	return value;
}

static inline IUINT32 iclock()
{
	return (IUINT32)(iclock64() & 0xfffffffful);
}

/* sleep in millisecond */
static inline void isleep(unsigned long millisecond)
{
#ifdef __unix 	/* usleep( time * 1000 ); */
	struct timespec ts;
	ts.tv_sec = (time_t)(millisecond / 1000);
	ts.tv_nsec = (long)((millisecond % 1000) * 1000000);
	/*nanosleep(&ts, NULL);*/
	usleep((millisecond << 10) - (millisecond << 4) - (millisecond << 3));
#elif defined(_WIN32)
	Sleep(millisecond);
#endif
}

namespace kcp_svr {

	uint64_t endpoint_to_i(const udp::endpoint& ep)
	{
		uint64_t addr_i = ep.address().to_v4().to_ulong();
		uint32_t port = ep.port();
		return (addr_i << 32) + port;
	}


	connection_manager::connection_manager(asio::io_service& io_service, const std::string& address, int udp_port) :
		stopped_(false),
		udp_socket_(io_service, udp::endpoint(asio::ip::address::from_string(address), udp_port)),
		kcp_timer_(io_service),
		cur_clock_(0)
	{
		//udp_socket_.set_option(udp::socket::non_blocking_io(false)); // why this make compile fail

		hook_udp_async_receive();
		hook_kcp_timer();
	}

	void connection_manager::stop_all()
	{
		stopped_ = true;
		connections_.stop_all();

		udp_socket_.cancel();
		udp_socket_.close();
	}

	void connection_manager::force_disconnect(const kcp_conv_t& conv)
	{
		std::cout << "force_disconnect: " << conv << std::endl;
		if (!connections_.find_by_conv(conv))
			return;

		std::shared_ptr<std::string> msg(new std::string("server force disconnect"));
		call_event_callback_func(conv, eEventType::eDisconnect, msg);
		connections_.remove_connection(conv);
	}

	void connection_manager::set_callback(const std::function<event_callback_t>& func)
	{
		event_callback_ = func;
	}

	void connection_manager::call_event_callback_func(kcp_conv_t conv, eEventType event_type, std::shared_ptr<std::string> msg)
	{
		event_callback_(conv, event_type, msg);
	}

	void connection_manager::handle_connect_packet()
	{
		kcp_conv_t conv = connections_.get_new_conv();
		std::string send_back_msg = asio_kcp::making_send_back_conv_packet(conv);
		udp_socket_.send_to(asio::buffer(send_back_msg), udp_remote_endpoint_);
		connections_.add_new_connection(shared_from_this(), conv, udp_remote_endpoint_);
	}

	void connection_manager::handle_kcp_packet(size_t bytes_recvd)
	{
		IUINT32 conv;
		int ret = ikcp_get_conv(udp_data_, bytes_recvd, &conv);
		if (ret == 0)
		{
			assert_check(false, "ikcp_get_conv return 0");
			return;
		}

		connection::shared_ptr conn_ptr = connections_.find_by_conv(conv);
		if (!conn_ptr)
		{
			std::cout << "connection not exist with conv: " << conv << std::endl;
			return;
		}

		if (conn_ptr)
			conn_ptr->input(udp_data_, bytes_recvd, udp_remote_endpoint_);
		else
			std::cout << "add_new_connection failed! can not connect!" << std::endl;
	}

	void connection_manager::handle_udp_receive_from(const asio::error_code& error, size_t bytes_recvd)
	{
		if (!error && bytes_recvd > 0)
		{
			/*
			std::cout << "\nudp_sender_endpoint: " << udp_remote_endpoint_ << std::endl;
			unsigned long addr_i = udp_remote_endpoint_.address().to_v4().to_ulong();
			std::cout << addr_i << " " << udp_remote_endpoint_.port() << std::endl;
			std::cout << "udp recv: " << bytes_recvd << std::endl <<
				Essential::ToHexDumpText(std::string(udp_data_, bytes_recvd), 32) << std::endl;
			*/

#if AK_ENABLE_UDP_PACKET_LOG
			AK_UDP_PACKET_LOG << "udp_recv:" << udp_remote_endpoint_.address().to_string() << ":" << udp_remote_endpoint_.port()
				<< " conv:" << 0
				<< " size:" << bytes_recvd << "\n"
				<< Essential::ToHexDumpText(std::string(udp_data_, bytes_recvd), 32);
#endif

			if (asio_kcp::is_connect_packet(udp_data_, bytes_recvd))
			{
				handle_connect_packet();
				goto END;
			}

			handle_kcp_packet(bytes_recvd);
		}
		else
		{
			printf("\nhandle_udp_receive_from error end! error: %s, bytes_recvd: %ld\n", error.message().c_str(), bytes_recvd);
		}

	END:
		hook_udp_async_receive();
	}

	void connection_manager::hook_udp_async_receive(void)
	{
		if (stopped_)
			return;
		udp_socket_.async_receive_from(
			asio::buffer(udp_data_, sizeof(udp_data_)), udp_remote_endpoint_,
			std::bind(&connection_manager::handle_udp_receive_from, this,
				std::placeholders::_1,
				std::placeholders::_2));
	}

	void connection_manager::hook_kcp_timer(void)
	{
		if (stopped_)
			return;
		kcp_timer_.expires_from_now(std::chrono::milliseconds(5));
		kcp_timer_.async_wait(std::bind(&connection_manager::handle_kcp_time, this));
	}

	void connection_manager::handle_kcp_time(void)
	{
		//std::cout << "."; std::cout.flush();
		hook_kcp_timer();
		cur_clock_ = iclock();
		connections_.update_all_kcp(cur_clock_);
	}

	void connection_manager::send_udp_packet(const std::string& msg, const asio::ip::udp::endpoint& endpoint)
	{
		udp_socket_.send_to(asio::buffer(msg), endpoint);
	}

	int connection_manager::send_msg(const kcp_conv_t& conv, std::shared_ptr<std::string> msg)
	{
		connection::shared_ptr connection_ptr = connections_.find_by_conv(conv);
		if (!connection_ptr)
			return -1;

		connection_ptr->send_kcp_msg(*msg);
		return 0;
	}

} // namespace kcp_svr
