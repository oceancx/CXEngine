/*
 @ 0xCCCCCCCC
*/

#include "kbase/chrono_util.h"
#include "kbase/secure_c_runtime.h"

namespace {

#if defined(OS_WIN)

using WinFileTimeTickRate = std::ratio<1, 10'000'000>;
using WinFileTimeTick = std::chrono::duration<long long, WinFileTimeTickRate>;

constexpr WinFileTimeTick kEpochDifference(11644473600'000'000'0);

#endif

}   // namespace

namespace kbase {

#if defined(OS_WIN)

TimePoint TimePointFromWindowsFileTime(const FILETIME& filetime)
{
    ULARGE_INTEGER filetime_ticks {0};
    filetime_ticks.LowPart = filetime.dwLowDateTime;
    filetime_ticks.HighPart = filetime.dwHighDateTime;

    return TimePoint(WinFileTimeTick(filetime_ticks.QuadPart - kEpochDifference.count()));
}

FILETIME TimePointToWindowsFileTime(TimePoint time_point)
{
    auto ticks = std::chrono::duration_cast<WinFileTimeTick>(time_point.time_since_epoch()) +
                 kEpochDifference;

    ULARGE_INTEGER filetime_ticks {0};
    filetime_ticks.QuadPart = ticks.count();

    FILETIME filetime {0};
    filetime.dwLowDateTime = filetime_ticks.LowPart;
    filetime.dwHighDateTime = filetime_ticks.HighPart;

    return filetime;
}

TimePoint TimePointFromWindowsSystemTime(const SYSTEMTIME& sys_time)
{
    // It is OK to ignore tm::tm_yday when calling mktime().
    tm time {0};
    time.tm_year = static_cast<int>(sys_time.wYear) - 1900;
    time.tm_mon = static_cast<int>(sys_time.wMonth) - 1;
    time.tm_mday = static_cast<int>(sys_time.wDay);
    time.tm_hour = static_cast<int>(sys_time.wHour);
    time.tm_min = static_cast<int>(sys_time.wMinute);
    time.tm_sec = static_cast<int>(sys_time.wSecond);
    time.tm_wday = sys_time.wDayOfWeek;
    time.tm_isdst = -1;

    return Clock::from_time_t(mktime(&time)) + std::chrono::milliseconds(sys_time.wMilliseconds);
}

SYSTEMTIME TimePointToWindowsSystemTime(TimePoint time_point)
{
    auto t = Clock::to_time_t(time_point);
    tm time {0};
    SecureLocalTime(&t, &time);

    SYSTEMTIME sys_time {0};
    sys_time.wYear = static_cast<WORD>(time.tm_year) + 1900;
    sys_time.wMonth = static_cast<WORD>(time.tm_mon) + 1;
    sys_time.wDay = static_cast<WORD>(time.tm_mday);
    sys_time.wDayOfWeek = static_cast<WORD>(time.tm_wday);
    sys_time.wHour = static_cast<WORD>(time.tm_hour);
    sys_time.wMinute = static_cast<WORD>(time.tm_min);
    sys_time.wSecond = static_cast<WORD>(time.tm_sec);
    auto in_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        time_point.time_since_epoch());
    sys_time.wMilliseconds = static_cast<WORD>(
        (in_ms - std::chrono::duration_cast<std::chrono::seconds>(in_ms)).count());

    return sys_time;
}

#elif defined(OS_POSIX)

#if defined(OS_APPLE) 
TimePoint TimePointFromTimespec(const timespec& timespec)
{
    return Clock::from_time_t(0);
}
#else 
TimePoint TimePointFromTimespec(const timespec& timespec)
{
    return Clock::from_time_t(timespec.tv_sec) + std::chrono::nanoseconds(timespec.tv_nsec);
}
#endif

#endif

}   // namespace kbase
