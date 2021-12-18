/*
 @ 0xCCCCCCCC
*/

#ifndef EZIO_EVENT_PUMP_IMPL_WIN_H_
#define EZIO_EVENT_PUMP_IMPL_WIN_H_

#include <set>
#include <vector>

#include <Windows.h>

#include "kbase/basic_macros.h"
#include "kbase/scoped_handle.h"

#include "ezio/event_pump.h"

namespace ezio {

class EventLoop;

class EventPump::Impl {
public:
    explicit Impl(EventLoop*);

    ~Impl();

    DISALLOW_COPY(Impl);

    DISALLOW_MOVE(Impl);

    TimePoint Pump(std::chrono::milliseconds timeout, std::vector<IONotification>& notifications);

    void Wakeup();

    void RegisterNotifier(Notifier* notifier);

    void UnregisterNotifier(Notifier* notifier);

private:
    void AssociateWithNotifier(const Notifier* notifier) const;

    void FillActiveNotifications(size_t count, std::vector<IONotification>& notifications) const;

private:
    kbase::ScopedWinHandle io_port_;
    std::vector<OVERLAPPED_ENTRY> io_events_;

    // Although a socket is automatically removed from the completion port when it is
    // closed, outstanding requests issued by the socket still get queued to the port and
    // yet, any use of the dead notifier, which associated with the socket, would result
    // in illegal access.
    // Therefore, we need bookkeeping of notifiers in interest, skipping completion events
    // of dead sockets.
    std::set<Notifier*> managed_notifiers_;
};

}   // namespace ezio

#endif  // EZIO_EVENT_PUMP_IMPL_WIN_H_
