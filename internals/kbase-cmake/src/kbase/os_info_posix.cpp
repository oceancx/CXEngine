/*
 @ 0xCCCCCCCC
*/

#include "kbase/os_info.h"

#include <chrono>
#include <cstdio>

#if defined(OS_APPLE)
namespace {

using kbase::SystemArchitecture;

using VersionNumber = kbase::OSInfo::VersionNumber;

SystemArchitecture GetSystemArchitecture()
{
    auto sys_arch = SystemArchitecture::Unknown;
    return sys_arch;
}

void GetSystemVersion(VersionNumber& version_number)
{
}

long GetNumberOfCores()
{
    return 1;
}

}   // namespace

namespace kbase {

OSInfo::OSInfo() noexcept
    : architecture_(GetSystemArchitecture()),
      core_count_(GetNumberOfCores()),
      vm_granularity_(0)
{
    GetSystemVersion(version_number_);
}

// static
std::chrono::seconds OSInfo::UpTime()
{
    return std::chrono::seconds(0);
}

std::string OSInfo::SystemName() const
{
    return "Unknown";
}

}   // namespace kbase
#else
#include <sys/sysinfo.h>
#include <sys/utsname.h>
#include <unistd.h>

namespace {

using kbase::SystemArchitecture;

using VersionNumber = kbase::OSInfo::VersionNumber;

SystemArchitecture GetSystemArchitecture()
{
    auto sys_arch = SystemArchitecture::Unknown;

    utsname info;
    if (-1 == uname(&info)) {
        return sys_arch;
    }

    std::string arch(info.machine);
    if (arch == "i386" || arch == "i486" || arch == "i586" || arch == "i686") {
        sys_arch = SystemArchitecture::X86;
    } else if (arch == "amd64" || arch == "x86_64") {
        sys_arch = SystemArchitecture::X86_64;
    }

    return sys_arch;
}

void GetSystemVersion(VersionNumber& version_number)
{
    utsname info;
    if (-1 == uname(&info)) {
        return;
    }

    int major = 0, minor = 0;
    if (sscanf(info.release, "%d.%d", &major, &minor) < 2) {
        return;
    }

    version_number.major_ver = major;
    version_number.minor_ver = minor;
}

long GetNumberOfCores()
{
    long cores = sysconf(_SC_NPROCESSORS_CONF);
    return cores == -1 ? 1 : cores;
}

}   // namespace

namespace kbase {

OSInfo::OSInfo() noexcept
    : architecture_(GetSystemArchitecture()),
      core_count_(GetNumberOfCores()),
      vm_granularity_(getpagesize())
{
    GetSystemVersion(version_number_);
}

// static
std::chrono::seconds OSInfo::UpTime()
{
    struct sysinfo info;
    if (-1 == sysinfo(&info)) {
        return std::chrono::seconds(0);
    }

    return std::chrono::seconds(info.uptime);
}

std::string OSInfo::SystemName() const
{
    utsname info;
    if (-1 == uname(&info)) {
        return "Unknown";
    }

    return info.sysname;
}

}   // namespace kbase
#endif
