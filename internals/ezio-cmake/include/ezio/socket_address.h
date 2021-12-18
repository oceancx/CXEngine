/*
 @ 0xCCCCCCCC
*/

#ifndef EZIO_SOCKET_ADDRESS_H_
#define EZIO_SOCKET_ADDRESS_H_

#include <string>

#include "kbase/basic_macros.h"

#include "ezio/endian_utils.h"

#if defined(OS_POSIX)
#include <arpa/inet.h>
#elif defined(OS_WIN)
#include <WinSock2.h>
#endif

#if defined(COMPILER_MSVC)
#define NOT_NULL __notnull
#else
#define NOT_NULL
#endif

namespace ezio {

class SocketAddress {
public:
    explicit SocketAddress(const sockaddr_in& addr);

    explicit SocketAddress(unsigned short port);

    SocketAddress(NOT_NULL const char* ip, unsigned short port);

    DEFAULT_COPY(SocketAddress);

    DEFAULT_MOVE(SocketAddress);

    unsigned short port() const noexcept
    {
        return NetworkToHost(addr_.sin_port);
    }

    const sockaddr_in& raw() const noexcept
    {
        return addr_;
    }

    std::string ToHostPort() const;

private:
    sockaddr_in addr_;
};

}   // namespace ezio

#endif  // EZIO_SOCKET_ADDRESS_H_
