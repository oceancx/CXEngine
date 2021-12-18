/*
 @ 0xCCCCCCCC
*/

#ifndef EZIO_TIMER_ID_H_
#define EZIO_TIMER_ID_H_

#include "kbase/basic_macros.h"

namespace ezio {

class Timer;

class TimerID {
public:
    TimerID() noexcept
        : timer_(nullptr)
    {}

    explicit TimerID(Timer* timer) noexcept
        : timer_(timer)
    {}

    DEFAULT_COPY(TimerID);

    DEFAULT_MOVE(TimerID);

    explicit operator bool() const noexcept
    {
        return timer_ != nullptr;
    }

    Timer* timer() const noexcept
    {
        return timer_;
    }

private:
    Timer* timer_;
};

}   // namespace ezio

#endif  // EZIO_TIMER_ID_H_
