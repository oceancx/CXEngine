/*
 @ 0xCCCCCCCC
*/

#ifndef EZIO_CONNECTOR_BASE_H_
#define EZIO_CONNECTOR_BASE_H_

#include <chrono>
#include <functional>
#include <memory>

#include "ezio/scoped_socket.h"
#include "ezio/socket_address.h"

namespace ezio {

class EventLoop;

class ConnectorBase {
public:
    using NewConnectionHandler = std::function<void(ScopedSocket&&, const SocketAddress&)>;

    virtual ~ConnectorBase() = default;

    virtual void Connect() = 0;

    // It should be safe to call this function multiple times.
    virtual void Cancel() = 0;

    void WeaklyBind(const std::shared_ptr<void>& obj);

    void set_on_new_connection(NewConnectionHandler handler)
    {
        on_new_connection_ = std::move(handler);
    }

    const SocketAddress& remote_addr() const noexcept
    {
        return remote_addr_;
    }

    bool connecting() const noexcept
    {
        return connecting_;
    }

protected:
    ConnectorBase(EventLoop* loop, const SocketAddress& addr);

    virtual void HandleNewConnection() = 0;

    virtual void HandleError(bool restart) = 0;

protected:
    static constexpr std::chrono::milliseconds kInitialRetryDelay {500};
    static constexpr std::chrono::seconds kMaxRetryDelay {30};

    EventLoop* loop_;

    SocketAddress remote_addr_;
    ScopedSocket socket_;

    std::weak_ptr<void> bound_object_;
    bool weakly_bound_;

    NewConnectionHandler on_new_connection_;

    bool connecting_;

    std::chrono::milliseconds retry_delay_;
};

}   // namespace ezio

#endif  // EZIO_CONNECTOR_BASE_H_
