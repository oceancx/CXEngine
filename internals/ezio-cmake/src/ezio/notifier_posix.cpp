/*
 @ 0xCCCCCCCC
*/

#include "ezio/notifier.h"

namespace ezio {
#if defined(OS_APPLE)
void Notifier::DoHandleEvent(TimePoint receive_time, IOContext io_ctx) const
{
    auto events = io_ctx.events;
    constexpr auto details = io_ctx.ToDetails();

    if ((events & POLLHUP) && !(events & POLLIN)) {
        if (on_close_) {
            on_close_();
        }
    }

    if (events & POLLERR) {
        if (on_error_) {
            on_error_();
        }
    }

    if (events & (POLLIN | POLLPRI | POLLHUP)) {
        if (on_read_) {
            on_read_(receive_time, details);
        }
    }

    if (events & POLLOUT) {
        if (on_write_) {
            on_write_(details);
        }
    }
}
#else
void Notifier::DoHandleEvent(TimePoint receive_time, IOContext io_ctx) const
{
    auto events = io_ctx.events;
    constexpr auto details = io_ctx.ToDetails();

    if ((events & EPOLLHUP) && !(events & EPOLLIN)) {
        if (on_close_) {
            on_close_();
        }
    }

    if (events & EPOLLERR) {
        if (on_error_) {
            on_error_();
        }
    }

    if (events & (EPOLLIN | EPOLLPRI | EPOLLRDHUP)) {
        if (on_read_) {
            on_read_(receive_time, details);
        }
    }

    if (events & EPOLLOUT) {
        if (on_write_) {
            on_write_(details);
        }
    }
}
#endif
}   // namespace ezio
