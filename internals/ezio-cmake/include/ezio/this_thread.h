/*
 @ 0xCCCCCCCC
*/

#ifndef EZIO_THIS_THREAD_H_
#define EZIO_THIS_THREAD_H_

#include "kbase/basic_macros.h"

#if defined(OS_POSIX)
#include <sys/types.h>
#elif defined(OS_WIN)
#include <Windows.h>
#endif

namespace ezio {

constexpr const char* kMainThreadName = "ezio-Main";

namespace this_thread {

#if defined(OS_POSIX)
using ThreadID = pid_t;
#elif defined(OS_WIN)
using ThreadID = DWORD;
#endif

extern thread_local const char* tls_thread_name;

#if defined(OS_POSIX)

extern thread_local ThreadID tls_thread_id;

void CacheThreadID();

#endif

// On Windows, GetCurrentThreadId() is quite cheap and doesn't involve a system call.
inline ThreadID GetID()
{
#if defined(OS_POSIX)
    if (tls_thread_id == 0) {
        CacheThreadID();
    }

    return tls_thread_id;
#elif defined(OS_WIN)
    return GetCurrentThreadId();
#endif
}

inline const char* GetName()
{
    return tls_thread_name;
}

// We use TLS to cache the pointer to the name string, therefore the user must ensure the
// lifetime of `name` is not shorter than the the thread itself.
// Try not to set native thread name for main thread, especially on Linux; doing so may
// cause some tools stop working.
void SetName(const char* name, bool skip_native_name = false);

}   // namespace this_thread
}   // namespace ezio

#endif  // EZIO_THIS_THREAD_H_
