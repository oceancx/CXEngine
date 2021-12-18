/*
 @ 0xCCCCCCCC
*/

#ifndef EZIO_TCP_CONNECTION_H_
#define EZIO_TCP_CONNECTION_H_

#include <atomic>
#include <memory>
#include <string>

#include "kbase/basic_macros.h"
#include "kbase/string_view.h"

#include "ezio/buffer.h"
#include "ezio/common_event_handlers.h"
#include "ezio/notifier.h"
#include "ezio/scoped_socket.h"
#include "ezio/socket_address.h"

#if defined(OS_WIN)
#include "ezio/io_context.h"
#endif

namespace ezio {

class EventLoop;

class TCPConnection : public std::enable_shared_from_this<TCPConnection> {
public:
    // The instance is created in `Connecting` state and becomes `Connected` after execution
    // of MakeEstablished(). Once Shutdown() is called, the state then transitions into
    // `Disconnecting` state, and it finally turns into `Disconnected` either by HandleClose()
    // or MakeTeardown().
    enum class State {
        Connecting,
        Connected,
        Disconnecting,
        Disconnected
    };

    TCPConnection(EventLoop* loop, std::string name, ScopedSocket&& conn_sock,
                  const SocketAddress& local_addr, const SocketAddress& peer_addr);

    ~TCPConnection();

    DISALLOW_COPY(TCPConnection);

    DISALLOW_MOVE(TCPConnection);

    // This function is thread-safe.
    void Send(kbase::StringView data);

    // This function is thread-safe.
    void Shutdown();

    void SetTCPNoDelay(bool enable);

    // Must be called on connection's loop thread.
    void MakeEstablished();

    // Must be called on connection's loop thread.
    void MakeTeardown();

    EventLoop* event_loop() const noexcept
    {
        return loop_;
    }

    const std::string& name() const noexcept
    {
        return name_;
    }

    bool connected() const noexcept
    {
        return state() == State::Connected;
    }

    const SocketAddress& local_addr() const noexcept
    {
        return local_addr_;
    }

    const SocketAddress& peer_addr() const noexcept
    {
        return peer_addr_;
    }

    void set_on_connection(ConnectionEventHandler handler)
    {
        on_connection_ = std::move(handler);
    }

    void set_on_message(MessageEventHandler handler)
    {
        on_message_ = std::move(handler);
    }

    void set_on_close(CloseEventHandler handler)
    {
        on_close_ = std::move(handler);
    }

private:
    State state() const noexcept
    {
        return state_.load(std::memory_order_acquire);
    }

    void set_state(State new_state) noexcept
    {
        state_.store(new_state, std::memory_order_release);
    }

    void DoSend(kbase::StringView data);

    void DoSend(std::string&& data);

    void DoShutdown();

    void ForceClose();

    void HandleRead(TimePoint timestamp, IOContext::Details details);

    void HandleWrite(IOContext::Details details);

    void HandleClose();

    void HandleError() const;

#if defined(OS_WIN)
    void PostRead();

    void PostWrite();
#endif

private:
    EventLoop* loop_;
    std::string name_;
    std::atomic<State> state_;

    ScopedSocket conn_sock_;
    Notifier conn_notifier_;

    SocketAddress local_addr_;
    SocketAddress peer_addr_;

    Buffer input_buf_;
    Buffer output_buf_;

#if defined(OS_WIN)
    struct IORequests {
        IORequest read_req;
        IORequest write_req;
        bool outstanding_write_req;

        IORequests()
            : read_req(IOEvent::Read | IOEvent::Probe),
              write_req(IOEvent::Write),
              outstanding_write_req(false)
        {}
    };

    IORequests io_reqs_;
#endif

    ConnectionEventHandler on_connection_;
    MessageEventHandler on_message_;
    CloseEventHandler on_close_;
};

using TCPConnectionPtr = std::shared_ptr<TCPConnection>;

}   // namespace ezio

#endif  // EZIO_TCP_CONNECTION_H_
