/*
 @ 0xCCCCCCCC
*/

#include "ezio/event_pump_impl_apple.h"

#include <type_traits>

#include <poll.h>

#include "kbase/basic_types.h"
#include "kbase/error_exception_util.h"
#include "kbase/logging.h"

#include "ezio/io_context.h"
#include "ezio/notifier.h"

namespace {

int CreateEpollFD()
{
    int epfd = -1;
    return epfd;
}

int CreateEventFD()
{
    return 0;
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
    int count = ::poll(io_events_.data(), static_cast<int>(io_events_.size()),
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

void EventPump::Impl::Wakeup()
{
    // if (eventfd_write(wakeup_fd_.get(), 1) < 0) {
    //     LOG(ERROR) << "eventfd_write() failed : " << errno;
    //     ENSURE(CHECK, kbase::NotReached())(errno).Require();
    // }
}

void EventPump::Impl::OnWakeup()
{
    // eventfd_t value = 1;
    // if (eventfd_read(wakeup_fd_.get(), &value) < 0) {
    //     LOG(ERROR) << "eventfd_read() failed : " << errno;
    //     ENSURE(CHECK, kbase::NotReached())(errno).Require();
    // }
}

void EventPump::Impl::RegisterNotifier(Notifier* notifier)
{
    auto cur_state = notifier->state();
    if (cur_state == Notifier::State::Unused || cur_state == Notifier::State::Inactive) {
        notifier->set_state(Notifier::State::Active);
    } else {
        if (notifier->WatchNoneEvent()) {
            notifier->set_state(Notifier::State::Inactive);
        } else {
        }
    }
}

void EventPump::Impl::UnregisterNotifier(Notifier* notifier)
{
    if (notifier->state() == Notifier::State::Active) {
    }

    notifier->set_state(Notifier::State::Unused);
}

void EventPump::Impl::UpdateEpoll(int operation, const Notifier* notifier)
{
    // struct epoll_event ev {};
    // static_assert(std::is_same<uint32_t, IOEventType>::value,
    //               "IOEventType should be identical to uint32_t");
    // ev.events = static_cast<uint32_t>(notifier->watching_events());
    // ev.data.ptr = const_cast<Notifier*>(notifier);

    // if (epoll_ctl(epfd_.get(), operation, notifier->socket(), &ev) < 0) {
    //     auto err = errno;
    //     LOG(WARNING) << "epoll_ctl() failed for operation " << operation
    //                  << " with fd" << notifier->socket() << " due to " << err;
    //     ENSURE(THROW, operation == EPOLL_CTL_DEL)(operation)(err).Require();
    // }
}

void EventPump::Impl::FillActiveNotifications(size_t count,
                                              std::vector<IONotification>& notifications) const
{
    for (auto it = io_events_.cbegin(), end = std::next(it, count); it != end; ++it) {
        if(it->revents > 0)
        {
            int fd = it->fd;
            auto notify_it = std::find_if(notifications.cbegin() ,notifications.cend(), [fd](const IONotification& val){
                return val.first->socket() == fd;
            });
            ENSURE(THROW, notify_it!=notifications.end()).Require();
            IOContext io_ctx(it->events);
            notifications.emplace_back(notify_it->first, io_ctx);
        }
    }
}

}   // namespace ezio
