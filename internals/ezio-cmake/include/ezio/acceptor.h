/*
 @ 0xCCCCCCCC
*/

#ifndef EZIO_ACCEPTOR_H_
#define EZIO_ACCEPTOR_H_

#include <functional>

#include "kbase/basic_macros.h"

#include "ezio/notifier.h"
#include "ezio/scoped_socket.h"
#include "ezio/socket_address.h"

#if defined(OS_POSIX)
#include "kbase/scoped_handle.h"
#elif defined(OS_WIN)
#include "ezio/io_context.h"
#endif

namespace ezio {

class EventLoop;

class Acceptor {
public:
    using NewConnectionHandler = std::function<void(ScopedSocket&&, const SocketAddress&)>;

    Acceptor(EventLoop* loop, const SocketAddress& addr);

    ~Acceptor();

    DISALLOW_COPY(Acceptor);

    DISALLOW_MOVE(Acceptor);

    void Listen();

    void set_on_new_connection(NewConnectionHandler handler)
    {
        on_new_connection_ = std::move(handler);
    }

    bool listening() const noexcept
    {
        return listening_;
    }

private:
#if defined(OS_WIN)
    void PostAccept();
#endif

    void HandleNewConnection();

private:
    EventLoop* loop_;

    ScopedSocket listening_sock_;
    Notifier listening_notifier_;

    NewConnectionHandler on_new_connection_;

#if defined(OS_POSIX)
    kbase::ScopedFD sentinel_fd_;
#elif defined(OS_WIN)
    ScopedSocket accept_conn_;
    IORequest accept_req_;
#endif

    bool listening_;
};

}   // namespace ezio

#endif  // EZIO_ACCEPTOR_H_
