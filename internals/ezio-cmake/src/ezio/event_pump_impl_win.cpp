/*
 @ 0xCCCCCCCC
*/

#include "ezio/event_pump_impl_win.h"

#include <WinSock2.h>

#include "kbase/basic_macros.h"
#include "kbase/error_exception_util.h"
#include "kbase/logging.h"

#include "ezio/notifier.h"

namespace {

ULONG_PTR kWakeupCompletionKey = 0x1;

kbase::ScopedWinHandle CreateIOCP()
{
    kbase::ScopedWinHandle iocp(CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, 0, 1));
    ENSURE(THROW, !!iocp)(kbase::LastError()).Require();
    return iocp;
}

}   // namespace

namespace ezio {

EventPump::Impl::Impl(EventLoop*)
    : io_port_(CreateIOCP()),
      io_events_(kInitialEventNum)
{}

EventPump::Impl::~Impl()
{}

TimePoint EventPump::Impl::Pump(std::chrono::milliseconds timeout,
                                std::vector<IONotification>& notifications)
{
    unsigned long dequeued_num = 0;
    auto succeed = GetQueuedCompletionStatusEx(io_port_.get(),
                                               io_events_.data(),
                                               static_cast<ULONG>(io_events_.size()),
                                               &dequeued_num,
                                               static_cast<DWORD>(timeout.count()),
                                               FALSE);

    TimePoint returned_time = ToTimePoint(std::chrono::system_clock::now());

    if (!succeed) {
        auto err = WSAGetLastError();
        LOG_IF(ERROR, err != WAIT_TIMEOUT) << "Dequeue for completion failure: " << err;
    } else if (dequeued_num > 0) {
        FillActiveNotifications(dequeued_num, notifications);
        if (dequeued_num == io_events_.size()) {
            io_events_.resize(io_events_.size() * 2);
        }
    }

    return returned_time;
}

void EventPump::Impl::Wakeup()
{
    FORCE_AS_NON_CONST_FUNCTION();

    if (!PostQueuedCompletionStatus(io_port_.get(), 0, kWakeupCompletionKey, nullptr)) {
        kbase::LastError error;
        LOG(ERROR) << "PostQueuedCompletionStatus() failed: " << error;
        ENSURE(CHECK, kbase::NotReached())(error).Require();
    }
}

void EventPump::Impl::RegisterNotifier(Notifier* notifier)
{
    if (notifier->state() == Notifier::State::Unused) {
        AssociateWithNotifier(notifier);
        notifier->set_state(Notifier::State::Active);

        ENSURE(CHECK, managed_notifiers_.count(notifier) == 0).Require();
        managed_notifiers_.insert(notifier);
    }
}

void EventPump::Impl::UnregisterNotifier(Notifier* notifier)
{
    notifier->set_state(Notifier::State::Inactive);

    ENSURE(CHECK, managed_notifiers_.count(notifier) != 0).Require();
    managed_notifiers_.erase(notifier);
}

void EventPump::Impl::AssociateWithNotifier(const Notifier* notifier) const
{
    auto iocp = io_port_.get();
    bool succeeded = CreateIoCompletionPort(reinterpret_cast<HANDLE>(notifier->socket()),
                                            iocp,
                                            reinterpret_cast<ULONG_PTR>(notifier),
                                            0) == iocp;
    ENSURE(THROW, succeeded)(kbase::LastError()).Require();
}

void EventPump::Impl::FillActiveNotifications(size_t count,
                                              std::vector<IONotification>& notifications) const
{
    for (auto it = io_events_.cbegin(), end = std::next(it, count); it != end; ++it) {
        if (it->lpCompletionKey == kWakeupCompletionKey) {
            continue;
        }

        auto notifier = reinterpret_cast<Notifier*>(it->lpCompletionKey);

        if (managed_notifiers_.count(notifier) == 0) {
            LOG(WARNING) << "Skip dead notifier of a dead socket";
            continue;
        }

        auto io_req = static_cast<IORequest*>(it->lpOverlapped);
        IOContext io_ctx(io_req, it->dwNumberOfBytesTransferred);

        notifications.emplace_back(notifier, io_ctx);
    }
}

}   // namespace ezio
