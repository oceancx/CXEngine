/*
 @ 0xCCCCCCCC
*/

#include "ezio/tcp_connection.h"

#include "kbase/error_exception_util.h"
#include "kbase/logging.h"

#include "ezio/event_loop.h"

namespace ezio {

void TCPConnection::HandleRead(TimePoint timestamp, IOContext::Details)
{
    ENSURE(CHECK, loop_->BelongsToCurrentThread()).Require();

    ssize_t bytes_read = ReadFDInVec(conn_sock_.get(), input_buf_);
    if (bytes_read < 0) {
        auto err = errno;
        LOG(ERROR) << "Failed to read data from socket " << conn_sock_.get() << "; err: " << err;
        HandleError();
    } else if (bytes_read == 0) {
        HandleClose();
    } else {
        on_message_(shared_from_this(), input_buf_, timestamp);
    }
}

void TCPConnection::DoSend(kbase::StringView data)
{
    ENSURE(CHECK, loop_->BelongsToCurrentThread()).Require();

    // In case the connection is down during queuing the task.
    if (state() != State::Connected) {
        LOG(WARNING) << "Writing to a not-connected connection!";
        return;
    }

    // If no data queued in the output buffer, write to the socket directly.
    size_t remaining = data.size();
    if (output_buf_.readable_size() == 0) {
        ENSURE(CHECK, !conn_notifier_.WatchWriting())(conn_notifier_.watching_events()).Require();
        auto bytes_written = write(conn_sock_.get(), data.data(), data.size());
        if (bytes_written < 0) {
            if (errno != EAGAIN) {
                auto err = errno;
                LOG(ERROR) << "Failed to write to a socket: " << err;
                if (err == EPIPE) {
                    LOG(ERROR) << "Writing failure due to EPIPE; abandon unwritten data!";
                    return;
                }
            }
        } else {
            remaining -= bytes_written;
        }
    }

    ENSURE(CHECK, remaining <= data.size())(remaining)(data.size()).Require();

    if (remaining == 0) {
        return;
    }

    auto written_size = data.size() - remaining;
    output_buf_.Write(data.data() + written_size, remaining);

    if (!conn_notifier_.WatchWriting()) {
        conn_notifier_.EnableWriting();
    }
}

void TCPConnection::HandleWrite(IOContext::Details)
{
    ENSURE(CHECK, loop_->BelongsToCurrentThread()).Require();
    ENSURE(CHECK, output_buf_.readable_size() > 0).Require();

    if (!conn_notifier_.WatchWriting()) {
        LOG(INFO) << "The connection of the socket " << conn_sock_.get() << " is down!";
        return;
    }

    auto bytes_written = write(conn_sock_.get(), output_buf_.Peek(), output_buf_.readable_size());
    if (bytes_written < 0) {
        LOG(ERROR) << "Failed to write to the socket " << conn_sock_.get() << "; errno: " << errno;
        return;
    }

    output_buf_.Consume(static_cast<size_t>(bytes_written));
    if (output_buf_.readable_size() == 0) {
        conn_notifier_.DisableWriting();
        if (state() == State::Disconnecting) {
            DoShutdown();
        }
    }
}

}   // namespace ezio
