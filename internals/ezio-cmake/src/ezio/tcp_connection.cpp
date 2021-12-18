/*
 @ 0xCCCCCCCC
*/

#include "ezio/tcp_connection.h"

#include "kbase/error_exception_util.h"
#include "kbase/logging.h"

#include "ezio/event_loop.h"
#include "ezio/socket_utils.h"

namespace {

using namespace std::placeholders;

}   // namespace

namespace ezio {

TCPConnection::TCPConnection(EventLoop* loop,
                             std::string name,
                             ScopedSocket&& conn_sock,
                             const SocketAddress& local_addr,
                             const SocketAddress& peer_addr)
    : loop_(loop),
      name_(std::move(name)),
      state_(State::Connecting),
      conn_sock_(std::move(conn_sock)),
      conn_notifier_(loop, conn_sock_),
      local_addr_(local_addr),
      peer_addr_(peer_addr)
{
    conn_notifier_.set_on_read(std::bind(&TCPConnection::HandleRead, this, _1, _2));
    conn_notifier_.set_on_write(std::bind(&TCPConnection::HandleWrite, this, _1));
    conn_notifier_.set_on_close(std::bind(&TCPConnection::HandleClose, this));
    conn_notifier_.set_on_error(std::bind(&TCPConnection::HandleError, this));
}

TCPConnection::~TCPConnection()
{
    bool expected = state() == State::Disconnected;
    DLOG_IF(ERROR, !expected) << "TCPConnection is in " << kbase::enum_cast(state())
                              << "while destructing";
    ENSURE(CHECK, expected)(state()).Require();
}

void TCPConnection::Send(kbase::StringView data)
{
    if (state() != State::Connected) {
        LOG(WARNING) << "Writing to a not-connected connection!";
        return;
    }

    if (loop_->BelongsToCurrentThread()) {
        DoSend(data);
    } else {
        loop_->QueueTask([replica = data.ToString(), self = shared_from_this()] () mutable {
            self->DoSend(std::move(replica));
        });
    }
}

void TCPConnection::DoSend(std::string&& data)
{
    ENSURE(CHECK, loop_->BelongsToCurrentThread()).Require();

    auto str = std::move(data);
    DoSend(str);
}

void TCPConnection::MakeEstablished()
{
    ENSURE(CHECK, loop_->BelongsToCurrentThread()).Require();
    ENSURE(CHECK, state() == State::Connecting)(state()).Require();

    set_state(State::Connected);

    conn_notifier_.WeaklyBind(shared_from_this());
    conn_notifier_.EnableReading();

    on_connection_(shared_from_this());

#if defined(OS_WIN)
    ENSURE(CHECK, io_reqs_.read_req.IsProbing())(io_reqs_.read_req.events).Require();
    PostRead();
#endif
}

void TCPConnection::MakeTeardown()
{
    ENSURE(CHECK, loop_->BelongsToCurrentThread()).Require();

    auto running_state = State::Connected;
    if (state_.compare_exchange_strong(running_state, State::Disconnected,
                                       std::memory_order_acq_rel, std::memory_order_relaxed)) {
        conn_notifier_.DisableAll();
        on_connection_(shared_from_this());
    }

    conn_notifier_.Detach();
}

void TCPConnection::Shutdown()
{
    auto running_state = State::Connected;
    if (state_.compare_exchange_strong(running_state, State::Disconnecting,
                                       std::memory_order_acq_rel, std::memory_order_relaxed)) {
        loop_->RunTask(std::bind(&TCPConnection::DoShutdown, this));
    }
}

void TCPConnection::DoShutdown()
{
    FORCE_AS_NON_CONST_FUNCTION();

    ENSURE(CHECK, loop_->BelongsToCurrentThread()).Require();

    if (!conn_notifier_.WatchWriting()) {
        socket::ShutdownWrite(conn_sock_);
    }
}

void TCPConnection::ForceClose()
{
    ENSURE(CHECK, loop_->BelongsToCurrentThread()).Require();

    HandleClose();
}

void TCPConnection::SetTCPNoDelay(bool enable)
{
    FORCE_AS_NON_CONST_FUNCTION();

    socket::SetTCPNoDelay(conn_sock_, enable);
}

void TCPConnection::HandleClose()
{
    ENSURE(CHECK, loop_->BelongsToCurrentThread()).Require();
    ENSURE(CHECK, state() == State::Connected || state() == State::Disconnecting)
        (state()).Require();

    set_state(State::Disconnected);

    conn_notifier_.DisableAll();

    TCPConnectionPtr conn(shared_from_this());

    on_connection_(conn);
    on_close_(conn);
}

void TCPConnection::HandleError() const
{
    auto err_code = socket::GetSocketErrorCode(conn_sock_);
    LOG(ERROR) << "Error occurred on " << name() << " with code " << err_code;
}

}   // namespace ezio
