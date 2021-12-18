/*
 @ 0xCCCCCCCC
*/

#ifndef EZIO_NOTIFIER_H_
#define EZIO_NOTIFIER_H_

#include <functional>
#include <memory>

#include "kbase/basic_macros.h"

#include "ezio/chrono_utils.h"
#include "ezio/io_context.h"
#include "ezio/scoped_socket.h"

namespace ezio {

class EventLoop;

class Notifier {
public:
    using ReadEventHandler = std::function<void(TimePoint, IOContext::Details)>;
    using WriteEventHandler = std::function<void(IOContext::Details)>;
    using CloseEventHandler = std::function<void()>;
    using ErrorHandler = std::function<void()>;

    enum class State {
        Unused,
        Active,
        Inactive
    };

    Notifier(EventLoop* loop, const ScopedSocket& socket) noexcept;

    ~Notifier() = default;

    DISALLOW_COPY(Notifier);

    DISALLOW_MOVE(Notifier);

    void set_on_read(ReadEventHandler handler)
    {
        on_read_ = std::move(handler);
    }

    void set_on_write(WriteEventHandler handler)
    {
        on_write_ = std::move(handler);
    }

    void set_on_close(CloseEventHandler handler)
    {
        on_close_ = std::move(handler);
    }

    void set_on_error(ErrorHandler handler)
    {
        on_error_ = std::move(handler);
    }

    IOEventType watching_events() const noexcept
    {
        return watching_events_;
    }

    bool WatchNoneEvent() const noexcept
    {
        return watching_events_ == IOEvent::None;
    }

    bool WatchReading() const noexcept
    {
        return (watching_events_ & IOEvent::Read) != IOEvent::None;
    }

    void EnableReading();

    void DisableReading();

    bool WatchWriting() const noexcept
    {
        return (watching_events_ & IOEvent::Write) != IOEvent::None;
    }

    void EnableWriting();

    void DisableWriting();

    void DisableAll();

    void Detach();

    // Prevent the associated object being destroyed during HandleEvent() or executing
    // HandleEvent() after the object being dead.
    void WeaklyBind(const std::shared_ptr<void>& obj);

    void HandleEvent(TimePoint receive_time, IOContext io_ctx) const;

    State state() const noexcept
    {
        return state_;
    }

    void set_state(State new_state) noexcept
    {
        state_ = new_state;
    }

    ScopedSocket::Handle socket() const noexcept
    {
        return socket_;
    }

private:
    void Update();

    void DoHandleEvent(TimePoint receive_time, IOContext io_ctx) const;

private:
    EventLoop* loop_;
    ScopedSocket::Handle socket_;

    std::weak_ptr<void> bound_object_;
    bool weakly_bound_;

    ReadEventHandler on_read_;
    WriteEventHandler on_write_;
    CloseEventHandler on_close_;
    ErrorHandler on_error_;

    State state_;

    IOEventType watching_events_;
};

}   // namespace ezio

#endif  // EZIO_NOTIFIER_H_
