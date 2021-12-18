/*
 @ 0xCCCCCCCC
*/

#include "ezio/acceptor.h"

#include <cstring>

#include <fcntl.h>

#include "kbase/error_exception_util.h"
#include "kbase/logging.h"

namespace ezio {

int MakeSentinelFD()
{
    int fd = open("/dev/null", O_RDONLY | O_CLOEXEC);
    ENSURE(CHECK, fd != -1)(errno).Require();
    return fd;
}

void Acceptor::HandleNewConnection()
{
    while (true) {
        sockaddr_in peer_raw_addr {};
        socklen_t addr_len = sizeof(peer_raw_addr);
        int conn_fd = accept4(listening_sock_.get(), reinterpret_cast<sockaddr*>(&peer_raw_addr),
                              &addr_len, SOCK_NONBLOCK | SOCK_CLOEXEC);

        // Handle failures.
        if (conn_fd < 0) {
            // No more pending requests.
            if (errno == EAGAIN) {
                return;
            }

            // No looping if we encounterred an error.

            auto err = errno;
            LOG(ERROR) << "accept4() failed: " << err;

            if (err == EMFILE) {
                sentinel_fd_ = nullptr;
                sentinel_fd_.reset(accept(listening_sock_.get(), nullptr, nullptr));
                if (!sentinel_fd_) {
                    err = errno;
                    LOG(ERROR) << "Still failed for accept(): " << err;
                }

                // Restore.
                sentinel_fd_ = nullptr;
                sentinel_fd_.reset(MakeSentinelFD());
            }

            return;
        }

        ScopedSocket conn_sock(conn_fd);
        SocketAddress peer_addr(peer_raw_addr);

        if (on_new_connection_) {
            on_new_connection_(std::move(conn_sock), peer_addr);
        } else {
            conn_sock = nullptr;
            LOG(WARNING) << "No handler set for new connections!";
        }
    }
}

}   // namespace ezio
