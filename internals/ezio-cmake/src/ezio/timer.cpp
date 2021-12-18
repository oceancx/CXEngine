/*
 @ 0xCCCCCCCC
*/

#include "ezio/timer.h"

namespace ezio {

Timer::Timer(TickEventHandler handler, TimePoint when, TimeDuration interval)
    : on_tick_(std::move(handler)),
      expiration_(when),
      interval_(interval)
{}

void Timer::Tick() const
{
    on_tick_();
}

void Timer::Restart(TimePoint when)
{
    expiration_ = when + interval_;
}

}   // namespace ezio
