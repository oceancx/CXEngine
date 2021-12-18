/*
 @ 0xCCCCCCCC
*/

#include "kbase/os_info.h"

#include "kbase/basic_macros.h"
#include "kbase/basic_types.h"

namespace {

using kbase::SystemVersion;
using kbase::WOW64Status;

using VersionNumber = kbase::OSInfo::VersionNumber;

constexpr VersionNumber kVersionTable[] {
    {6, 0},     // SystemVersion::WinVista
    {6, 1},     // SystemVersion::Win7
    {6, 2},     // SystemVersion::Win8
    {6, 3},     // SystemVersion::Win8_1
    {10, 0},    // SystemVersion::Win10
};

void GetSystemVersion(VersionNumber& version_number) noexcept
{
    constexpr NTSTATUS kStatusSuccess = 0L;
    DECLARE_DLL_FUNCTION(RtlGetVersion, NTSTATUS(WINAPI*)(PRTL_OSVERSIONINFOW), "ntdll.dll");
    if (!RtlGetVersion) {
        return;
    }

    RTL_OSVERSIONINFOW ovi { sizeof(ovi) };
    if (RtlGetVersion(&ovi) != kStatusSuccess) {
        return;
    }

    version_number.major_ver = ovi.dwMajorVersion;
    version_number.minor_ver = ovi.dwMinorVersion;
}

}   // namespace

namespace kbase {

OSInfo::OSInfo() noexcept
    : architecture_(SystemArchitecture::Unknown)
{
    GetSystemVersion(version_number_);

    SYSTEM_INFO system_info {0};
    GetNativeSystemInfo(&system_info);

    switch (system_info.wProcessorArchitecture) {
        case PROCESSOR_ARCHITECTURE_INTEL:
            architecture_ = SystemArchitecture::X86;
            break;

        case PROCESSOR_ARCHITECTURE_AMD64:
            architecture_ = SystemArchitecture::X86_64;
            break;

        default:
            break;
    }

    core_count_ = system_info.dwNumberOfProcessors;
    vm_granularity_ = system_info.dwAllocationGranularity;
}

// static
std::chrono::seconds OSInfo::UpTime()
{
    unsigned long long unbiased_interrupt_time = 0;
    QueryUnbiasedInterruptTime(&unbiased_interrupt_time);
    return std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::microseconds(unbiased_interrupt_time / 10));
}

// static
WOW64Status OSInfo::GetWOW64StatusForProcess(HANDLE process) noexcept
{
    DECLARE_DLL_FUNCTION(IsWow64Process, BOOL(WINAPI*)(HANDLE, BOOL*), "kernel32.dll");
    if (!IsWow64Process) {
        return WOW64Status::Disabled;
    }

    BOOL is_wow64 = FALSE;
    if (!IsWow64Process(process, &is_wow64)) {
        return WOW64Status::Unknown;
    }

    return is_wow64 ? WOW64Status::Enabled : WOW64Status::Disabled;
}

// static
bool OSInfo::RunningOn64BitSystem() noexcept
{
#if defined(_WIN64)
    return true;
#else
    auto wow64_status = GetWOW64StatusForProcess(GetCurrentProcess());
    switch (wow64_status) {
        case WOW64Status::Enabled:
            return true;

        case WOW64Status::Disabled:
            return false;

        default:
            return false;
    }
#endif
}

bool OSInfo::IsVersionOrGreater(SystemVersion version) const
{
    return !(version_number_ < kVersionTable[enum_cast(version)]);
}

std::string OSInfo::SystemName() const
{
    if (kVersionTable[enum_cast(SystemVersion::WinVista)] == version_number()) {
        return "Windows Vista";
    }

    if (kVersionTable[enum_cast(SystemVersion::Win7)] == version_number()) {
        return "Windows 7";
    }

    if (kVersionTable[enum_cast(SystemVersion::Win8)] == version_number()) {
        return "Windows 8";
    }

    if (kVersionTable[enum_cast(SystemVersion::Win8_1)] == version_number()) {
        return "Windows 8.1";
    }

    if (kVersionTable[enum_cast(SystemVersion::Win10)] == version_number()) {
       return "Windows 10";
    }

    return "Unknown";
}

}   // namespace kbase
