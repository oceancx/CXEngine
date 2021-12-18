/*
 @ 0xCCCCCCCC
*/

#include "ezio/connector_win.h"

#include <cstring>

#include "kbase/error_exception_util.h"
#include "kbase/logging.h"

#include "ezio/event_loop.h"
#include "ezio/io_service_context.h"
#include "ezio/socket_utils.h"

namespace ezio {

ConnectorWin::ConnectorWin(EventLoop* loop, const SocketAddress& addr)
    : ConnectorBase(loop, addr)
{
    memset(&ov, 0, sizeof(ov));
}

ConnectorWin::~ConnectorWin()
{
    ENSURE(CHECK, !connecting_).Require();
    ENSURE(CHECK, !socket_).Require();
}

void ConnectorWin::Connect()
{
    ENSURE(CHECK, loop_->BelongsToCurrentThread()).Require();
    ENSURE(CHECK, !socket_).Require();

    socket_ = socket::CreateNonBlockingSocket();

    // ConnectEx() requires the socket to be initially bound, for which
    // port == 0 && sin_addr == INADDR_ANY.
    SocketAddress addr(0);
    socket::BindOrThrow(socket_, addr);

    connecting_ = true;

    IOServiceContext::current().AsWinsockContext().ConnectEx(
        socket_.get(),
        reinterpret_cast<const sockaddr*>(&remote_addr_.raw()),
        sizeof(remote_addr_.raw()),
        nullptr,
        0,
        nullptr,
        &ov);

    TryCompleteConnect();
}

void ConnectorWin::Cancel()
{
    ENSURE(CHECK, loop_->BelongsToCurrentThread()).Require();

    if (connecting_) {
        ENSURE(CHECK, !!retry_timer_).Require();
        loop_->CancelTimedTask(retry_timer_);
        retry_timer_ = TimerID();

        // Still trying to complete.
        if (socket_) {
            BOOL rv = CancelIo(reinterpret_cast<HANDLE>(socket_.get()));
            LOG_IF(WARNING, rv == 0) << "CancelIo() failed due to " << GetLastError();
            socket_ = nullptr;
        }

        connecting_ = false;
    }
}

void ConnectorWin::TryCompleteConnect()
{
    ENSURE(CHECK, loop_->BelongsToCurrentThread()).Require();
    ENSURE(CHECK, connecting_).Require();

    DWORD bytes = 0;
    DWORD flags = 0;
    BOOL ok = WSAGetOverlappedResult(socket_.get(), &ov, &bytes, FALSE, &flags);
    if (ok) {
        HandleNewConnection();
    } else {
        auto err = WSAGetLastError();
        switch (err) {
            case WSA_IO_INCOMPLETE:
                DLOG(INFO) << "Cannot complete connection yet, schedule to retry";
                retry_timer_ = loop_->RunTaskAfter(
                    std::bind(&ConnectorWin::TryCompleteConnect, this), retry_delay_);
                retry_delay_ = std::min<std::chrono::milliseconds>(retry_delay_ * 2,
                                                                   kMaxRetryDelay);
                break;

            case WSAEADDRINUSE:
            case WSAECONNREFUSED:
            case WSAENETUNREACH:
            case WSAEHOSTUNREACH:
                LOG(WARNING) << "Currently cannot connect to the remote "
                             << remote_addr_.ToHostPort() << "; Error: " << err;
                HandleError(true);
                break;

            default:
                LOG(ERROR) << "Failed to connect to the remote " << remote_addr_.ToHostPort()
                           << "; Error: " << err;
                HandleError(false);
                break;
        }
    }
}

void ConnectorWin::HandleNewConnection()
{
    ENSURE(CHECK, loop_->BelongsToCurrentThread()).Require();

    int rv = setsockopt(socket_.get(), SOL_SOCKET, SO_UPDATE_CONNECT_CONTEXT, nullptr, 0);
    if (rv != 0) {
        LOG(ERROR) << "Failed to SO_UPDATE_CONNECT_CONTEXT: " << WSAGetLastError();
        HandleError(true);
        return;
    }

    // Cleanup for future reuse.
    connecting_ = false;
    memset(&ov, 0, sizeof(ov));
    retry_delay_ = kInitialRetryDelay;
    retry_timer_ = TimerID();

    sockaddr_in local_addr;
    memset(&local_addr, 0, sizeof(local_addr));
    int addr_len = sizeof(local_addr);
    rv = getsockname(socket_.get(), reinterpret_cast<sockaddr*>(&local_addr), &addr_len);
    LOG_IF(ERROR, rv != 0) << "getsockname() failed; Error: " << WSAGetLastError();

    std::shared_ptr<void> binder;

    if (weakly_bound_) {
        binder = bound_object_.lock();
        if (!binder) {
            return;
        }
    }

    on_new_connection_(std::move(socket_), SocketAddress(local_addr));

    RETAIN_LIFETIME_TO_HERE(binder);
}

void ConnectorWin::HandleError(bool restart)
{
    ENSURE(CHECK, loop_->BelongsToCurrentThread()).Require();

    socket_ = nullptr;
    memset(&ov, 0, sizeof(ov));

    if (restart) {
        retry_timer_ = loop_->RunTaskAfter(std::bind(&ConnectorWin::Connect, this), retry_delay_);
        retry_delay_ = std::min<std::chrono::milliseconds>(retry_delay_ * 2, kMaxRetryDelay);
    } else {
        connecting_ = false;
        retry_timer_ = TimerID();
    }
}

}   // namespace ezio
