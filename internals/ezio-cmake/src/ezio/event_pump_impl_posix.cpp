/*
 @ 0xCCCCCCCC
*/

#include "ezio/event_pump_impl_posix.h"

#include <type_traits>

#include <sys/eventfd.h>

#include "kbase/basic_types.h"
#include "kbase/error_exception_util.h"
#include "kbase/logging.h"

#include "ezio/io_context.h"
#include "ezio/notifier.h"

namespace {

int CreateEpollFD()
{
    int epfd = epoll_create1(EPOLL_CLOEXEC);
    ENSURE(THROW, epfd != -1)(errno).Require();
    return epfd;
}

int CreateEventFD()
{
    int efd = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    ENSURE(THROW, efd != -1)(errno).Require();
    return efd;
}

}   // namespace

namespace ezio {

EventPump::Impl::Impl(EventLoop* loop)
    : epfd_(CreateEpollFD()),
      io_events_(kInitialEventNum),
      wakeup_fd_(CreateEventFD()),
      wakeup_notifier_(loop, wakeup_fd_)
{}

EventPump::Impl::~Impl()
{
    ENSURE(CHECK, wakeup_notifier_.state() == Notifier::State::Unused).Require();
}

TimePoint EventPump::Impl::Pump(std::chrono::milliseconds timeout,
                                std::vector<IONotification>& notifications)
{
    int count = epoll_wait(epfd_.get(), io_events_.data(), static_cast<int>(io_events_.size()),
                           static_cast<int>(timeout.count()));
    auto err = errno;

    TimePoint returned_time = ToTimePoint(std::chrono::system_clock::now());

    if (count == -1) {
        if (err != EINTR) {
            LOG(ERROR) << "epoll_wait() failed: " << err;
        }
    } else if (count > 0) {
        FillActiveNotifications(static_cast<size_t>(count), notifications);
        if (static_cast<size_t>(count) == io_events_.size()) {
            io_events_.resize(io_events_.size() * 2);
        }
    }

    return returned_time;
}

void EventPump::Impl::EnableWakeupNotification()
{
    wakeup_notifier_.set_on_read(std::bind(&Impl::OnWakeup, this));
    wakeup_notifier_.EnableReading();
}

void EventPump::Impl::DisableWakeupNotification()
{
    wakeup_notifier_.DisableAll();
    wakeup_notifier_.Detach();
}

void EventPump::Impl::Wakeup()
{
    if (eventfd_write(wakeup_fd_.get(), 1) < 0) {
        LOG(ERROR) << "eventfd_write() failed : " << errno;
        ENSURE(CHECK, kbase::NotReached())(errno).Require();
    }
}

void EventPump::Impl::OnWakeup()
{
    eventfd_t value = 1;
    if (eventfd_read(wakeup_fd_.get(), &value) < 0) {
        LOG(ERROR) << "eventfd_read() failed : " << errno;
        ENSURE(CHECK, kbase::NotReached())(errno).Require();
    }
}

void EventPump::Impl::RegisterNotifier(Notifier* notifier)
{
    auto cur_state = notifier->state();
    if (cur_state == Notifier::State::Unused || cur_state == Notifier::State::Inactive) {
        notifier->set_state(Notifier::State::Active);
        UpdateEpoll(EPOLL_CTL_ADD, notifier);
    } else {
        if (notifier->WatchNoneEvent()) {
            notifier->set_state(Notifier::State::Inactive);
            UpdateEpoll(EPOLL_CTL_DEL, notifier);
        } else {
            UpdateEpoll(EPOLL_CTL_MOD, notifier);
        }
    }
}

void EventPump::Impl::UnregisterNotifier(Notifier* notifier)
{
    if (notifier->state() == Notifier::State::Active) {
        UpdateEpoll(EPOLL_CTL_DEL, notifier);
    }

    notifier->set_state(Notifier::State::Unused);
}

void EventPump::Impl::UpdateEpoll(int operation, const Notifier* notifier)
{
    struct epoll_event ev {};
    static_assert(std::is_same<uint32_t, IOEventType>::value,
                  "IOEventType should be identical to uint32_t");
    ev.events = static_cast<uint32_t>(notifier->watching_events());
    ev.data.ptr = const_cast<Notifier*>(notifier);

    if (epoll_ctl(epfd_.get(), operation, notifier->socket(), &ev) < 0) {
        auto err = errno;
        LOG(WARNING) << "epoll_ctl() failed for operation " << operation
                     << " with fd" << notifier->socket() << " due to " << err;
        ENSURE(THROW, operation == EPOLL_CTL_DEL)(operation)(err).Require();
    }
}

void EventPump::Impl::FillActiveNotifications(size_t count,
                                              std::vector<IONotification>& notifications) const
{
    for (auto it = io_events_.cbegin(), end = std::next(it, count); it != end; ++it) {
        auto notifier = static_cast<Notifier*>(it->data.ptr);
        IOContext io_ctx(it->events);
        notifications.emplace_back(notifier, io_ctx);
    }
}

}   // namespace ezio
