/*
 @ 0xCCCCCCCC
*/

#include "ezio/socket_utils.h"

#include "kbase/error_exception_util.h"
#include "kbase/logging.h"

namespace ezio {
namespace socket {

ScopedSocket CreateNonBlockingSocket()
{
    ScopedSocket sock(WSASocketW(AF_INET, SOCK_STREAM, 0, nullptr, 0, WSA_FLAG_OVERLAPPED));
    ENSURE(THROW, !!sock)(WSAGetLastError()).Require();
    return sock;
}

void SetReuseAddr(const ScopedSocket& sock, bool enable)
{
    char optval = enable ? 1 : 0;
    int rv = setsockopt(sock.get(), SOL_SOCKET, SO_EXCLUSIVEADDRUSE, &optval, sizeof(optval));
    if (rv < 0) {
        auto err = WSAGetLastError();
        LOG(ERROR) << "Set socket SO_EXCLUSIVEADDRUSE " << enable << " failed: " << err;
        ENSURE(CHECK, kbase::NotReached())(err)(enable).Require();
    }
}

void ShutdownWrite(const ScopedSocket& sock)
{
    if (shutdown(sock.get(), SD_SEND) < 0) {
        LOG(ERROR) << "Failed to shutdown write-side: " << WSAGetLastError();
    }
}

}   // namespace socket
}   // namespace ezio
