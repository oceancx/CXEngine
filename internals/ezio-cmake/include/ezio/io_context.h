/*
 @ 0xCCCCCCCC
*/

#ifndef EZIO_IO_CONTEXT_H_
#define EZIO_IO_CONTEXT_H_

#include <cstdint>
#include <type_traits>

#include "kbase/basic_macros.h"

#if defined(OS_APPLE)
#include <sys/poll.h>
#elif defined(OS_POSIX)
#include <sys/epoll.h>
#elif defined(OS_WIN)
#include <Windows.h>
#endif

namespace ezio {

enum IOEvent : uint32_t {
    None = 0,
#if defined(OS_APPLE)
    Read = POLLIN | POLLPRI,
    Write = POLLOUT
#elif defined(OS_POSIX)
    Read = EPOLLIN | EPOLLPRI,
    Write = EPOLLOUT
#elif defined(OS_WIN)
    Probe = 1 << 0,
    Read = 1 << 1,
    Write = 1 << 2
#endif
};

using IOEventType = std::underlying_type_t<IOEvent>;

#if defined(OS_POSIX)

struct IOContext {
    IOEventType events;

    // Details on Linux is currently no use.
    struct Details {
        constexpr Details() noexcept = default;
        ~Details() = default;
    };

    constexpr explicit IOContext(IOEventType epoll_events) noexcept
        : events(epoll_events)
    {}

    constexpr Details ToDetails() const noexcept
    {
        return Details{};
    }
};

#elif defined(OS_WIN)

struct IORequest : OVERLAPPED {
    IOEventType events;

    explicit IORequest(IOEventType events)
        : events(events)
    {
        Reset();
    }

    void Reset()
    {
        Offset = 0;
        OffsetHigh = 0;
        Internal = 0;
        InternalHigh = 0;
        hEvent = nullptr;
    }

    bool IsProbing() const noexcept
    {
        return (events & IOEvent::Probe) != 0;
    }

    void DisableProbing() noexcept
    {
        events &= ~IOEvent::Probe;
    }
};

struct IOContext {
    IORequest* io_req;
    DWORD bytes_transferred;

    struct Details {
        IOEventType events;
        DWORD bytes_transferred;

        Details(IOEventType events, DWORD bytes)
            : events(events), bytes_transferred(bytes)
        {}
    };

    IOContext(IORequest* req, DWORD bytes)
        : io_req(req), bytes_transferred(bytes)
    {}

    Details ToDetails() const noexcept
    {
        return {io_req->events, bytes_transferred};
    }
};

#endif

}   // namespace ezio

#endif  // EZIO_IO_CONTEXT_H_
