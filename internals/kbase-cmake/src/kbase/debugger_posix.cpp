/*
 @ 0xCCCCCCCC
*/

#include "kbase/debugger.h"

#include <fcntl.h>
#include <unistd.h>

#include "kbase/handle_interruptible_system_call.h"
#include "kbase/string_view.h"

namespace kbase {

bool IsDebuggerPresent()
{
    int status_fd = open("/proc/self/status", O_RDONLY);
    if (status_fd == -1) {
        return false;
    }

    constexpr int kBufSize = 1024;
    char buf[kBufSize];

    auto bytes_read = HANDLE_EINTR(read(status_fd, buf, kBufSize));
    if (IGNORE_EINTR(close(status_fd)) < 0) {
        return false;
    }

    if (bytes_read <= 0) {
        return false;
    }

    StringView status_info(buf, bytes_read);
    StringView tracer("TracerPid:\t");

    auto pos = status_info.find(tracer);
    if (pos == StringView::npos) {
        return false;
    }

    auto pid_pos = pos + tracer.size();

    return pid_pos < status_info.size() && status_info[pid_pos] != '0';
}

void BreakDebugger()
{
    asm("int3");
}

}   // namespace kbase
