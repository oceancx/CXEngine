/*
 @ 0xCCCCCCCC
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef KBASE_OS_INFO_H_
#define KBASE_OS_INFO_H_

#include <chrono>
#include <string>

#include "kbase/basic_macros.h"

#if defined(OS_WIN)
#include <Windows.h>
#endif

namespace kbase {

enum class SystemArchitecture {
    X86,
    X86_64,
    Unknown
};

#if defined(OS_WIN)
enum class WOW64Status {
    Disabled,
    Enabled,
    Unknown
};

// Doesn't support versions prior to Windows Vista.
enum class SystemVersion : unsigned int {
    WinVista,      // Also includes Server 2008.
    Win7,          // Also includes Server 2008 R2.
    Win8,          // Also includes Server 2012.
    Win8_1,        // Also includes Server 2012 R2.
    Win10,
};
#endif

class OSInfo {
public:
    struct VersionNumber {
        constexpr VersionNumber() noexcept
            : major_ver(0), minor_ver(0)
        {}

        constexpr VersionNumber(int major, int minor) noexcept
            : major_ver(major), minor_ver(minor)
        {}

        int major_ver;
        int minor_ver;
    };

    ~OSInfo() = default;

    DISALLOW_COPY(OSInfo);

    DISALLOW_MOVE(OSInfo);

    static OSInfo* GetInstance();

    // Returns the uptime of the system.
    // On Windows, this duration does not include time the system spends in sleep
    // or hibernation.
    static std::chrono::seconds UpTime();

#if defined(OS_WIN)
    // Returns Enabled, if the process is running under WOW64.
    // Returns Disabled, if the process is either a 64-bit application, or the process
    // is running on 32-bit system.
    // Returns Unknown, if an error occurs.
    // The handle to a process must have PROCESS_QUERY_INFORMATION access right.
    static WOW64Status GetWOW64StatusForProcess(HANDLE process) noexcept;

    // Returns true, if the host system is 64-bit system;
    // Returns false, otherwise.
    static bool RunningOn64BitSystem() noexcept;

    bool IsVersionOrGreater(SystemVersion version) const;
#endif

    std::string SystemName() const;

    unsigned long number_of_cores() const noexcept
    {
        return core_count_;
    }

    unsigned long vm_granularity() const noexcept
    {
        return vm_granularity_;
    }

    SystemArchitecture architecture() const noexcept
    {
        return architecture_;
    }

    VersionNumber version_number() const noexcept
    {
        return version_number_;
    }

private:
    OSInfo() noexcept;

private:
    SystemArchitecture architecture_;
    VersionNumber version_number_;
    unsigned long core_count_;
    unsigned long vm_granularity_;
};

inline bool operator==(const OSInfo::VersionNumber& lhs, const OSInfo::VersionNumber& rhs) noexcept
{
    return lhs.major_ver == rhs.major_ver && lhs.minor_ver == rhs.minor_ver;
}

inline bool operator!=(const OSInfo::VersionNumber& lhs, const OSInfo::VersionNumber& rhs) noexcept
{
    return !(lhs == rhs);
}

inline bool operator<(const OSInfo::VersionNumber& lhs, const OSInfo::VersionNumber& rhs) noexcept
{
    return (lhs.major_ver < rhs.major_ver) ||
           (lhs.major_ver == rhs.major_ver && lhs.minor_ver < rhs.minor_ver);
}

}   // namespace kbase

#endif  // KBASE_OS_INFO_H_
