/*
 @ 0xCCCCCCCC
*/

#include "ezio/socket_utils.h"

#include "kbase/error_exception_util.h"
#include "kbase/logging.h"

#if defined(OS_POSIX)
#include <netinet/tcp.h>
#endif

namespace ezio {
namespace socket {

int GetSocketErrorCode(const ScopedSocket& sock)
{
    optval_t optval;
    optlen_t optlen = sizeof(optval);

    if (getsockopt(sock.get(), SOL_SOCKET, SO_ERROR, &optval, &optlen) < 0) {
        return GetLastErrorCode();
    }

    return optval;
}

void SetTCPNoDelay(const ScopedSocket& sock, bool enable)
{
    optval_t optval = enable ? 1 : 0;
    if (setsockopt(sock.get(), IPPROTO_TCP, TCP_NODELAY, &optval, sizeof(optval)) < 0) {
        auto err = GetLastErrorCode();
        LOG(ERROR) << "Set socket TCP_NODELAY " << enable << " failed: " << err;
        ENSURE(CHECK, kbase::NotReached())(err)(enable).Require();
    }
}

void BindOrThrow(const ScopedSocket& sock, const SocketAddress& listening_addr)
{
    const auto& raw_addr = listening_addr.raw();
    int rv = bind(sock.get(), reinterpret_cast<const sockaddr*>(&raw_addr), sizeof(raw_addr));
    ENSURE(THROW, rv == 0)(GetLastErrorCode())(listening_addr.ToHostPort()).Require();
}

void ListenOrThrow(const ScopedSocket& sock)
{
    int rv = listen(sock.get(), SOMAXCONN);
    ENSURE(THROW, rv == 0)(GetLastErrorCode()).Require();
}

}   // namespace socket
}   // namespace ezio
