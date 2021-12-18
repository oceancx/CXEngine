/*
 @ 0xCCCCCCCC
*/

#include "ezio/notifier.h"

#include "kbase/error_exception_util.h"

#include "ezio/event_loop.h"

namespace ezio {

Notifier::Notifier(ezio::EventLoop* loop, const ezio::ScopedSocket& socket) noexcept
    : loop_(loop),
      socket_(socket.get()),
      weakly_bound_(false),
      state_(State::Unused),
      watching_events_(IOEvent::None)
{}

void Notifier::EnableReading()
{
    watching_events_ |= IOEvent::Read;
    Update();
}

void Notifier::DisableReading()
{
    watching_events_ &= ~IOEvent::Read;
    Update();
}

void Notifier::EnableWriting()
{
    watching_events_ |= IOEvent::Write;
    Update();
}

void Notifier::DisableWriting()
{
    watching_events_ &= ~IOEvent::Write;
    Update();
}

void Notifier::DisableAll()
{
    watching_events_ = IOEvent::None;
    Update();
}

void Notifier::Detach()
{
    ENSURE(CHECK, WatchNoneEvent())(watching_events_).Require();
    loop_->UnregisterNotifier(this);
}

void Notifier::WeaklyBind(const std::shared_ptr<void>& obj)
{
    bound_object_ = obj;
    weakly_bound_ = true;
}

void Notifier::HandleEvent(ezio::TimePoint receive_time, ezio::IOContext io_ctx) const
{
    std::shared_ptr<void> object;

    if (weakly_bound_) {
        object = bound_object_.lock();
        if (!object) {
            return;
        }
    }

    DoHandleEvent(receive_time, io_ctx);

    RETAIN_LIFETIME_TO_HERE(object);
}

void Notifier::Update()
{
    loop_->RegisterNotifier(this);
}

}   // namespace ezio
