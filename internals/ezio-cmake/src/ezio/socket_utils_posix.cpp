/*
 @ 0xCCCCCCCC
*/

#include "ezio/socket_utils.h"

#include <cstring>

#include <netinet/tcp.h>

#include "kbase/error_exception_util.h"
#include "kbase/logging.h"

namespace ezio {
namespace socket {

#if defined(OS_APPLE)
ScopedSocket CreateNonBlockingSocket()
{
    ScopedSocket sock_fd(::socket(AF_INET, SOCK_STREAM, 0));
    ENSURE(THROW, sock_fd.get() > 0)(errno).Require();
    return sock_fd;
}
#else
ScopedSocket CreateNonBlockingSocket()
{
    ScopedSocket sock_fd(::socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, 0));
    ENSURE(THROW, sock_fd.get() > 0)(errno).Require();
    return sock_fd;
}
#endif

void SetReuseAddr(const ScopedSocket& sock, bool enable)
{
    int optval = enable ? 1 : 0;
    if (setsockopt(sock.get(), SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0) {
        auto err = errno;
        LOG(ERROR) << "Set socket SO_REUSEADDR " << enable << " failed: " << err;
        ENSURE(CHECK, kbase::NotReached())(err)(enable).Require();
    }
}

void EnableTCPQuickACK(const ScopedSocket& sock)
{
#if !defined(OS_APPLE)
    int optval = 1;
    if (setsockopt(sock.get(), IPPROTO_TCP, TCP_QUICKACK, &optval, sizeof(optval)) < 0) {
        auto err = errno;
        LOG(ERROR) << "Enable socket TCP_QUICKACK failed: " << err;
        ENSURE(CHECK, kbase::NotReached())(err).Require();
    }
#endif
}

bool IsSelfConnected(const ScopedSocket& sock)
{
    sockaddr_in local_addr, peer_addr;
    memset(&local_addr, 0, sizeof(local_addr));
    memset(&peer_addr, 0, sizeof(peer_addr));
    socklen_t local_len = sizeof(local_addr);
    socklen_t peer_len = sizeof(peer_addr);

    if (getsockname(sock.get(), reinterpret_cast<sockaddr*>(&local_addr), &local_len) < 0) {
        ENSURE(CHECK, kbase::NotReached()).Require();
        LOG(ERROR) << "getsockname() failed due to " << errno;
    }

    if (getpeername(sock.get(), reinterpret_cast<sockaddr*>(&peer_addr), &peer_len) < 0) {
        ENSURE(CHECK, kbase::NotReached()).Require();
        LOG(ERROR) << "getpeername() failed due to " << errno;
    }

    return local_addr.sin_port == peer_addr.sin_port &&
           memcpy(&local_addr.sin_addr, &peer_addr.sin_addr, sizeof(local_addr.sin_addr));
}

void ShutdownWrite(const ScopedSocket& sock)
{
    if (shutdown(sock.get(), SHUT_WR) < 0) {
        LOG(ERROR) << "Failed to shutdown write-side: " << errno;
    }
}

}   // namespace socket
}   // namespace socket
