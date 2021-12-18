/*
 @ 0xCCCCCCCC
*/

#include "ezio/connector_posix.h"

#include <cstring>

#include <sys/socket.h>

#include "kbase/error_exception_util.h"
#include "kbase/logging.h"
#include "kbase/stack_walker.h"

#include "ezio/event_loop.h"
#include "ezio/socket_utils.h"

namespace ezio {

ConnectorPosix::ConnectorPosix(EventLoop* loop, const SocketAddress& addr)
    : ConnectorBase(loop, addr),
      waiting_completion_(false)
{}

ConnectorPosix::~ConnectorPosix()
{
    ENSURE(CHECK, !connecting_).Require();
    ENSURE(CHECK, !socket_).Require();
    ENSURE(CHECK, !sock_notifier_).Require();
    ENSURE(CHECK, !waiting_completion_).Require();
}

void ConnectorPosix::Connect()
{
    ENSURE(CHECK, loop_->BelongsToCurrentThread()).Require();
    ENSURE(CHECK, !socket_).Require();
    ENSURE(CHECK, !sock_notifier_).Require();

    socket_ = socket::CreateNonBlockingSocket();

    connecting_ = true;

    int rv = connect(socket_.get(), reinterpret_cast<const sockaddr*>(&remote_addr_.raw()),
                     sizeof(remote_addr_.raw()));

    auto last_err = (rv == 0) ? 0 : errno;
    switch (last_err) {
        case 0:
        case EINPROGRESS:
        case EISCONN:
        case EINTR:
            WaitForConnectingComplete();
            break;

        case EADDRINUSE:
        case EAGAIN:
        case ECONNREFUSED:
        case ENETUNREACH:
        case EADDRNOTAVAIL:
            LOG(WARNING) << "Currently cannot connect to the remote " << remote_addr_.ToHostPort()
                         << " due to " << last_err;
            HandleError(true);
            break;

        default:
            LOG(ERROR) << "Failed to connect to the remote " << remote_addr_.ToHostPort()
                       << " due to " << last_err;
            HandleError(false);
            break;
    }
}

void ConnectorPosix::Cancel()
{
    ENSURE(CHECK, loop_->BelongsToCurrentThread()).Require();

    if (connecting_) {
        if (waiting_completion_) {
            ResetNotifier();
            socket_ = nullptr;
            waiting_completion_ = false;
        } else {
            // Cancel retrying.
            ENSURE(CHECK, !!retry_timer_).Require();
            loop_->CancelTimedTask(retry_timer_);
            retry_timer_ = TimerID();
        }

        connecting_ = false;
    }
}

void ConnectorPosix::WaitForConnectingComplete()
{
    ENSURE(CHECK, loop_->BelongsToCurrentThread()).Require();
    ENSURE(CHECK, connecting_).Require();
    ENSURE(CHECK, !sock_notifier_).Require();
    ENSURE(CHECK, !waiting_completion_).Require();

    sock_notifier_ = std::make_unique<Notifier>(loop_, socket_);

    sock_notifier_->set_on_write(std::bind(&ConnectorPosix::HandleNewConnection, this));
    sock_notifier_->set_on_error(std::bind(&ConnectorPosix::HandleError, this, true));

    if (weakly_bound_) {
        sock_notifier_->WeaklyBind(bound_object_.lock());
    }

    sock_notifier_->EnableWriting();

    waiting_completion_ = true;
}

void ConnectorPosix::ResetNotifier()
{
    ENSURE(CHECK, waiting_completion_).Require();
    ENSURE(CHECK, !!socket_).Require();

    sock_notifier_->DisableAll();
    sock_notifier_->Detach();

    // Release the notifier first, in case the next call of WaitForConnectingComplete()
    // finds that the `sock_notifier_` is not empty.
    // But the last notifier is retained until to the next pump.
    loop_->QueueTask([notifier = std::shared_ptr<Notifier>(std::move(sock_notifier_))] {});
}

void ConnectorPosix::HandleNewConnection()
{
    ENSURE(CHECK, loop_->BelongsToCurrentThread()).Require();

    int err = socket::GetSocketErrorCode(socket_);
    if (err != 0) {
        // Because we dispatch error event (EPOLLERROR) prior to writable event(EPOLLOUT),
        // the error here, may have been handled by HandleError() already.
        if (waiting_completion_) {
            LOG(WARNING) << "Socket SO_ERROR " << err;
            HandleError(true);
        }
    } else {
        ENSURE(CHECK, waiting_completion_).Require();
        if (socket::IsSelfConnected(socket_)) {
            LOG(WARNING) << "Self connected encounterred! discard and start over again.";
            HandleError(true);
        } else {
            ResetNotifier();

            waiting_completion_ = false;
            connecting_ = false;
            retry_delay_ = kInitialRetryDelay;
            retry_timer_ = TimerID();

            sockaddr_in local_addr {};
            socklen_t addr_len = sizeof(local_addr);
            int rv = getsockname(socket_.get(), reinterpret_cast<sockaddr*>(&local_addr), &addr_len);
            LOG_IF(ERROR, rv != 0) << "getsockname() failed; Error: " << errno;

            on_new_connection_(std::move(socket_), SocketAddress(local_addr));
        }
    }
}

void ConnectorPosix::HandleError(bool restart)
{
    ENSURE(CHECK, loop_->BelongsToCurrentThread()).Require();

    // We are in the middle of notification handling.
    if (waiting_completion_) {
        ResetNotifier();
        waiting_completion_ = false;
    }

    socket_ = nullptr;

    if (restart) {
        DLOG(INFO) << "Cannot currently connect to the remote, schedule to start again.";
        retry_timer_ = loop_->RunTaskAfter(std::bind(&ConnectorPosix::Connect, this), retry_delay_);
        retry_delay_ = std::min<std::chrono::milliseconds>(retry_delay_ * 2, kMaxRetryDelay);
    } else {
        connecting_ = false;
        retry_timer_ = TimerID();
    }
}

}   // namespace ezio
