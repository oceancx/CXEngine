/*
 @ 0xCCCCCCCC
*/

#include "ezio/socket_address.h"

#include <array>
#include <cstring>

#include "kbase/error_exception_util.h"
#include "kbase/string_format.h"

#include "ezio/socket_utils.h"

#if defined(OS_WIN)
#include <WS2tcpip.h>
#endif

namespace ezio {

SocketAddress::SocketAddress(const sockaddr_in& addr)
    : addr_(addr)
{}

SocketAddress::SocketAddress(unsigned short port)
{
    memset(&addr_, 0, sizeof(addr_));

    addr_.sin_family = AF_INET;
    addr_.sin_port = HostToNetwork(port);
    addr_.sin_addr.s_addr = INADDR_ANY;
}

SocketAddress::SocketAddress(NOT_NULL const char* ip, unsigned short port)
{
    ENSURE(CHECK, ip != nullptr).Require();

    memset(&addr_, 0, sizeof(addr_));

    addr_.sin_family = AF_INET;
    addr_.sin_port = HostToNetwork(port);
    int rv = inet_pton(AF_INET, ip, &addr_.sin_addr);
    ENSURE(THROW, rv > 0)(socket::GetLastErrorCode())(ip)(port).Require();
}

std::string SocketAddress::ToHostPort() const
{
    std::array<char, 16> ip {};
    auto ip_ptr = inet_ntop(AF_INET, (PVOID)&addr_.sin_addr, ip.data(), ip.size());
    ENSURE(CHECK, ip_ptr != nullptr)(socket::GetLastErrorCode()).Require();

    return kbase::StringPrintf("%s:%d", ip_ptr, NetworkToHost(addr_.sin_port));
}

}   // namespace ezio
