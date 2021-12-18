/*
 @ 0xCCCCCCCC
*/

#ifndef EZIO_TCP_SERVER_H_
#define EZIO_TCP_SERVER_H_

#include <atomic>
#include <memory>
#include <string>
#include <unordered_map>

#include "kbase/basic_macros.h"

#include "ezio/acceptor.h"
#include "ezio/common_event_handlers.h"
#include "ezio/socket_address.h"
#include "ezio/tcp_connection.h"
#include "ezio/worker_pool.h"

namespace ezio {

class EventLoop;

class TCPServer {
public:
    // 0 worker indicates we don't use worker-pool.
    // If worker-pool is used but `worker_pool_name` is empty, we use TCPServer's name
    // as the pool's name.
    struct Options {
        size_t worker_num;
        std::string worker_pool_name;

        Options()
            : worker_num(0)
        {}
    };

    TCPServer(EventLoop* loop, const SocketAddress& addr, std::string name);

    ~TCPServer();

    DISALLOW_COPY(TCPServer);

    DISALLOW_MOVE(TCPServer);

    // Start functions is thread-safe and it is no harm to call the function multiple times.

    void Start();

    void Start(const Options& opt);

    const std::string& name() const noexcept
    {
        return name_;
    }

    std::string ip_port() const
    {
        return listen_addr_.ToHostPort();
    }

    void set_on_connection(ConnectionEventHandler handler)
    {
        on_connection_ = std::move(handler);
    }

    void set_on_message(MessageEventHandler handler)
    {
        on_message_ = std::move(handler);
    }

private:
    void HandleNewConnection(ScopedSocket&& conn_sock, const SocketAddress& conn_addr);

    void RemoveConnection(const TCPConnectionPtr& conn);

    // If we are using worker-pool, then use worker thread's loop for new connections.
    // Otherwise, use main loop.
    EventLoop* GetEventLoopForConnection() const;

private:
    EventLoop* loop_;
    SocketAddress listen_addr_;
    std::string name_;
    std::atomic<bool> started_;
    Acceptor acceptor_;
    std::unique_ptr<WorkerPool> worker_pool_;
    int next_conn_id_;

    using ConnectionMap = std::unordered_map<std::string, TCPConnectionPtr>;

    ConnectionMap connections_;

    ConnectionEventHandler on_connection_;
    MessageEventHandler on_message_;
};

}   // namespace ezio

#endif  // EZIO_TCP_SERVER_H_
