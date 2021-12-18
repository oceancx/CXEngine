/*
 @ 0xCCCCCCCC
*/

#ifndef EZIO_TCP_CLIENT_H_
#define EZIO_TCP_CLIENT_H_

#include <atomic>
#include <memory>
#include <mutex>
#include <string>

#include "kbase/basic_macros.h"

#include "ezio/common_event_handlers.h"
#include "ezio/connector.h"
#include "ezio/socket_address.h"
#include "ezio/tcp_connection.h"

namespace ezio {

class EventLoop;

class TCPClient {
    enum class State {
        Disconnected,
        Disconnecting,
        Connecting,
        Connected
    };

    struct AliveToken {};

public:
    TCPClient(EventLoop* loop, const SocketAddress& remote_addr, std::string name);

    ~TCPClient();

    DISALLOW_COPY(TCPClient);

    DISALLOW_MOVE(TCPClient);

    // This function is thread-safe.
    void Connect();

    // This function is thread-safe.
    void Disconnect();

    // This function is thread-safe.
    void Cancel();

    const std::string& name() const noexcept
    {
        return name_;
    }

    const SocketAddress& remote_addr() const noexcept
    {
        return connector_->remote_addr();
    }

    TCPConnectionPtr connection() const
    {
        std::lock_guard<std::mutex> lock(conn_mutex_);
        return conn_;
    }

    bool auto_reconnect() const noexcept
    {
        return auto_reconnect_;
    }

    void set_auto_reconnect(bool enable) noexcept
    {
        auto_reconnect_ = enable;
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
    void HandleConnection(ScopedSocket&& sock, const SocketAddress& local_addr);

    void HandleDisconnection(const TCPConnectionPtr& conn);

private:
    EventLoop* loop_;
    std::string name_;
    std::atomic<State> state_;
    int next_conn_id_;

    bool auto_reconnect_;
    std::unique_ptr<Connector> connector_;

    ConnectionEventHandler on_connection_;
    MessageEventHandler on_message_;

    mutable std::mutex conn_mutex_;
    TCPConnectionPtr conn_;

    std::shared_ptr<AliveToken> alive_token_;
};

}   // namespace ezio

#endif  // EZIO_TCP_CLIENT_H_
