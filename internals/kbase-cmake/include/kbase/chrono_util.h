/*
 @ 0xCCCCCCCC
*/

#if defined _MSC_VER
#pragma once
#endif

#ifndef KBASE_CHRONO_UTIL_H_
#define KBASE_CHRONO_UTIL_H_

#include <chrono>
#include <ctime>
#include <utility>

#include "kbase/basic_macros.h"
#include "kbase/secure_c_runtime.h"

#if defined(OS_WIN)
#include <Windows.h>
#elif defined(OS_POSIX)
#include <time.h>
#endif

namespace kbase {
namespace internal {

template<typename Resolution = std::chrono::seconds, typename Clock, typename Duration, typename F>
std::pair<tm, Resolution> TimePointToDateTime(std::chrono::time_point<Clock, Duration> time_point,
                                              F cvt)
{
    namespace chrono = std::chrono;

    auto in_resolution =  chrono::duration_cast<Resolution>(time_point.time_since_epoch());
    auto remainder = in_resolution - chrono::duration_cast<chrono::seconds>(in_resolution);

    time_t raw_time = Clock::to_time_t(time_point);
    tm date_time;
    cvt(&raw_time, &date_time);

    return {date_time, remainder};
}

}   // namespace internal

using Clock = std::chrono::system_clock;
using TimePoint = Clock::time_point;

#if defined(OS_WIN)

TimePoint TimePointFromWindowsFileTime(const FILETIME& filetime);

FILETIME TimePointToWindowsFileTime(TimePoint time_point);

TimePoint TimePointFromWindowsSystemTime(const SYSTEMTIME& sys_time);

SYSTEMTIME TimePointToWindowsSystemTime(TimePoint time_point);

#elif defined(OS_POSIX)

TimePoint TimePointFromTimespec(const timespec& timespec);

#endif

template<typename Resolution = std::chrono::seconds, typename Clock, typename Duration>
std::pair<tm, Resolution> TimePointToLocalTime(std::chrono::time_point<Clock, Duration> time_point)
{
    return internal::TimePointToDateTime<Resolution>(time_point, kbase::SecureLocalTime);
}

template<typename Resolution = std::chrono::seconds, typename Clock, typename Duration>
std::pair<tm, Resolution> TimePointToUTCTime(std::chrono::time_point<Clock, Duration> time_point)
{
    return internal::TimePointToDateTime<Resolution>(time_point, kbase::SecureUTCTime);
}

}   // namespace kbase

#endif  // KBASE_CHRONO_UTIL_H_
