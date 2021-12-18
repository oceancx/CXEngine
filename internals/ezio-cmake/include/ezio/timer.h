/*
 @ 0xCCCCCCCC
*/

#ifndef EZIO_TIMER_H_
#define EZIO_TIMER_H_

#include <functional>

#include "kbase/basic_macros.h"

#include "ezio/chrono_utils.h"

namespace ezio {

class Timer {
public:
    using TickEventHandler = std::function<void()>;

    Timer(TickEventHandler handler, TimePoint when, TimeDuration interval);

    ~Timer() = default;

    DISALLOW_COPY(Timer);

    DISALLOW_MOVE(Timer);

    void Tick() const;

    void Restart(TimePoint when);

    TimePoint expiration() const noexcept
    {
        return expiration_;
    }

    bool is_repeating() const noexcept
    {
        return interval_ > TimeDuration::zero();
    }

private:
    TickEventHandler on_tick_;
    TimePoint expiration_;
    TimeDuration interval_;
};

}   // namespace ezio

#endif  // EZIO_TIMER_H_
