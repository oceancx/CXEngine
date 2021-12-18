/*
 @ 0xCCCCCCCC
*/

#ifndef EZIO_IO_SERVICE_CONTEXT_H_
#define EZIO_IO_SERVICE_CONTEXT_H_

#include "kbase/basic_macros.h"

#if defined(OS_POSIX)
#include "ezio/ignore_sigpipe.h"
#elif defined(OS_WIN)
#include "ezio/winsock_context.h"
#endif

namespace ezio {

// Users of ezio must first call IOServiceContext::Init() to initialize the conceptual
// io-service-context, which wraps global-wide components needed by each platform.
// This class relies on kbase::AtExitManager for cleanup.
class IOServiceContext {
public:
    ~IOServiceContext() = default;

    DISALLOW_COPY(IOServiceContext);

    DISALLOW_MOVE(IOServiceContext);

    static void Init();

    static const IOServiceContext& current();

#if defined(OS_WIN)
    const WinsockContext& AsWinsockContext() const noexcept
    {
        return winsock_context_;
    }
#endif

private:
    IOServiceContext();

private:
#if defined(OS_WIN)
    WinsockContext winsock_context_;
#elif defined(OS_POSIX)
    IgnoreSigPipe ignore_sigpipe_;
#endif
};

}   // namespace ezio

#endif  // EZIO_IO_SERVICE_CONTEXT_H_
