/*
 @ 0xCCCCCCCC
*/

#ifndef KBASE_HANDLE_INTERRUPTIBLE_SYSTEM_CALL_H_
#define KBASE_HANDLE_INTERRUPTIBLE_SYSTEM_CALL_H_

#include <errno.h>

#if defined(NDEBUG)

#define HANDLE_EINTR(x) ({                       \
    decltype(x) eintr_rv;                        \
    do {                                         \
        eintr_rv = (x);                          \
    } while (eintr_rv == -1 && errno == EINTR);  \
    eintr_rv;                                    \
})

#else

#define HANDLE_EINTR(x) ({                        \
    constexpr int kMaxAttempts = 100;             \
    int attempts = 0;                             \
    decltype(x) eintr_rv;                         \
    do {                                          \
        eintr_rv = (x);                           \
    } while (eintr_rv == -1 && errno == EINTR &&  \
             attempts < kMaxAttempts);            \
    eintr_rv;                                     \
})

#endif  // NDEBUG

#define IGNORE_EINTR(x) ({                       \
    decltype(x) eintr_rv;                        \
    do {                                         \
        eintr_rv = (x);                          \
        if (eintr_rv == -1 && errno == EINTR) {  \
            eintr_rv = 0;                        \
        }                                        \
    } while (0);                                 \
    eintr_rv;                                    \
})

#endif  // KBASE_HANDLE_INTERRUPTIBLE_SYSTEM_CALL_H_
