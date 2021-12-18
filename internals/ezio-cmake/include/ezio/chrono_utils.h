/*
 @ 0xCCCCCCCC
*/

#ifndef EZIO_CHRONO_UTILS_H_
#define EZIO_CHRONO_UTILS_H_

#include <chrono>

namespace ezio {

#if defined(OS_POSIX)
using TimeDuration = std::chrono::microseconds;
#elif defined(OS_WIN)
using TimeDuration = std::chrono::milliseconds;
#endif

using TimePoint = std::chrono::time_point<std::chrono::system_clock, TimeDuration>;

template<typename From>
TimePoint ToTimePoint(const From& tp)
{
    return std::chrono::time_point_cast<TimePoint::duration>(tp);
}

}   // namespace ezio

#endif  // EZIO_CHRONO_UTILS_H_
