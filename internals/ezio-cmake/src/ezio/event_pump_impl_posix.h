/*
 @ 0xCCCCCCCC
*/

#ifndef EZIO_EVENT_PUMP_IMPL_POSIX_H_
#define EZIO_EVENT_PUMP_IMPL_POSIX_H_

#include <chrono>
#include <vector>

#include <sys/epoll.h>

#include "kbase/basic_macros.h"
#include "kbase/scoped_handle.h"

#include "ezio/event_pump.h"
#include "ezio/notifier.h"

namespace ezio {

class EventLoop;
class Notifier;

class EventPump::Impl {
public:
    explicit Impl(EventLoop* loop);

    ~Impl();

    DISALLOW_COPY(Impl);

    DISALLOW_MOVE(Impl);

    TimePoint Pump(std::chrono::milliseconds timeout, std::vector<IONotification>& notifications);

    void EnableWakeupNotification();

    void DisableWakeupNotification();

    void Wakeup();

    void RegisterNotifier(Notifier* notifier);

    void UnregisterNotifier(Notifier* notifier);

private:
    void UpdateEpoll(int operation, const Notifier* notifier);

    void FillActiveNotifications(size_t count, std::vector<IONotification>& notifications) const;

    void OnWakeup();

private:
    kbase::ScopedFD epfd_;
    std::vector<epoll_event> io_events_;
    kbase::ScopedFD wakeup_fd_;
    Notifier wakeup_notifier_;
};

}   // namespace ezio

#endif  // EZIO_EVENT_PUMP_IMPL_POSIX_H_
