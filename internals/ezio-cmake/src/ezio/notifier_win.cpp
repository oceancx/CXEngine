/*
 @ 0xCCCCCCCC
*/

#include "ezio/notifier.h"

#include <utility>

#include <WinSock2.h>

#include "kbase/error_exception_util.h"
#include "kbase/logging.h"

namespace {

using Result = std::pair<bool, int>;

Result CheckIfOperationSucceed(SOCKET socket, OVERLAPPED* overlapped)
{
    DWORD bytes = 0;
    DWORD flags = 0;

    int error_code = ERROR_SUCCESS;

    BOOL rv = WSAGetOverlappedResult(socket, overlapped, &bytes, FALSE, &flags);
    ENSURE(CHECK, bytes == 0 || rv != TRUE)(bytes)(flags).Require();
    if (!rv) {
        error_code = WSAGetLastError();
    }

    return {rv == TRUE, error_code};
}

}   // namespace

namespace ezio {

void Notifier::DoHandleEvent(TimePoint receive_time, IOContext io_ctx) const
{
    auto events = io_ctx.io_req->events;
    auto details = io_ctx.ToDetails();

    if (details.bytes_transferred == 0) {
        auto result = CheckIfOperationSucceed(socket(), io_ctx.io_req);
        if (!result.first) {
            LOG(WARNING) << "Async operation " << events << " on socket " << socket() << " failed;"
                         << " error: " << result.second;
            on_error_();
        }
    }

    // An async-io operation can't be both read and write.
    // However, `on_read_` and `on_write_` must be error-aware, that is, they must be able
    // to handle async read/write errors.
    if (events & IOEvent::Read) {
        ENSURE(CHECK, WatchReading())(watching_events()).Require();
        on_read_(receive_time, details);
    } else if (events & IOEvent::Write) {
        ENSURE(CHECK, WatchWriting())(watching_events()).Require();
        on_write_(details);
    } else {
        ENSURE(CHECK, kbase::NotReached())(events).Require();
    }
}

}   // namespace ezio
