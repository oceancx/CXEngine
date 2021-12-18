/*
 @ 0xCCCCCCCC
*/

#include "ezio/this_thread.h"

#include "kbase/debugger.h"
#include "kbase/error_exception_util.h"
#include "kbase/logging.h"
#include "kbase/string_encoding_conversions.h"

#if defined(OS_POSIX)
#if !defined(OS_APPLE)
#include <sys/prctl.h>
#endif
#include <sys/syscall.h>
#include <unistd.h>
#endif

namespace {

#if defined(OS_POSIX)

pid_t gettid()
{
#if !defined(OS_APPLE)
    return static_cast<pid_t>(syscall(SYS_gettid));
#else
    return 0;
#endif
}

void SetNativeThreadName(const char* name)
{
#if !defined(OS_APPLE)
    ENSURE(CHECK, gettid() != getpid()).Require("DO NOT change main thread's name!");

    int rv = prctl(PR_SET_NAME, name);
    if (rv < 0) {
        auto err = errno;
        LOG(WARNING) << "prctl(PR_SET_NAME) failed: " << err;
    }
#endif
}

#elif defined(OS_WIN)

struct win10_description {};
struct seh_tradition {};

const DWORD kVCThreadNameException = 0x406D1388;

typedef struct tagTHREADNAME_INFO {
    DWORD dwType;  // Must be 0x1000.
    LPCSTR szName;  // Pointer to name (in user addr space).
    DWORD dwThreadID;  // Thread ID (-1=caller thread).
    DWORD dwFlags;  // Reserved for future use, must be zero.
} THREADNAME_INFO;

bool SetThreadNameInternal(const char* name, win10_description)
{
    DECLARE_DLL_FUNCTION(SetThreadDescription, HRESULT(WINAPI*)(HANDLE, PCWSTR), "Kernel32.dll");
    if (!SetThreadDescription) {
        return false;
    }

    auto hr = SetThreadDescription(GetCurrentThread(), kbase::ASCIIToWide(name).c_str());
    if (FAILED(hr)) {
        kbase::LastError err;
        LOG(WARNING) << "SetThreadDescription() failed: " << err;
        return false;
    }

    return true;
}

// See @ https://goo.gl/ovx1L6.
void SetThreadNameInternal(const char* name, seh_tradition)
{
    THREADNAME_INFO info;
    info.dwType = 0x1000;
    info.szName = name;
    info.dwThreadID = static_cast<DWORD>(-1);
    info.dwFlags = 0;

    __try {
        RaiseException(kVCThreadNameException, 0, sizeof(info) / sizeof(ULONG_PTR),
                       reinterpret_cast<ULONG_PTR*>(&info));
    } __except(EXCEPTION_EXECUTE_HANDLER) {
    }
}

void SetNativeThreadName(const char* name)
{
    if (SetThreadNameInternal(name, win10_description{})) {
        return;
    }

    // The old way is necessary only when the debugger is present.
    if (kbase::IsDebuggerPresent()) {
        SetThreadNameInternal(name, seh_tradition{});
    }
}

#endif

}   // namespace

namespace ezio {
namespace this_thread {

#if defined(OS_POSIX)

thread_local ThreadID tls_thread_id {0};

void CacheThreadID()
{
    if (tls_thread_id == 0) {
        tls_thread_id = gettid();
    }
}

#endif

thread_local const char* tls_thread_name {"Unknown"};

void SetName(const char* name, bool skip_native_name)
{
    tls_thread_name = name;

    if (!skip_native_name) {
        SetNativeThreadName(name);
    }
}

}   // namespace this_thread
}   // namespace ezio
