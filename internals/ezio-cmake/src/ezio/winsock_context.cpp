/*
 @ 0xCCCCCCCC
*/

#include "ezio/winsock_context.h"

#include "kbase/error_exception_util.h"
#include "kbase/scope_guard.h"

#pragma comment(lib, "ws2_32.lib")

namespace {

template<typename P>
void GetExtensionFunctionPointer(SOCKET sock, P& pfn, GUID guid)
{
    DWORD recv_bytes = 0;
    auto rv = WSAIoctl(sock, SIO_GET_EXTENSION_FUNCTION_POINTER, &guid, sizeof(guid), &pfn,
                       sizeof(pfn), &recv_bytes, nullptr, nullptr);
    ENSURE(CHECK, rv == 0)(rv)(WSAGetLastError()).Require();
}

}   // namespace

namespace ezio {

WinsockContext::WinsockContext()
    : AcceptEx(nullptr)
{
    WSADATA data {0};
    int rv = WSAStartup(MAKEWORD(2, 2), &data);
    ENSURE(THROW, rv == 0)(rv).Require();

    auto was_guard = MAKE_SCOPE_GUARD { WSACleanup(); };

    auto sock = socket(AF_INET, SOCK_STREAM, 0);
    ENSURE(THROW, !!sock)(WSAGetLastError()).Require();

    // Avoid to introduce scoped-socket concept here.
    ON_SCOPE_EXIT { closesocket(sock); };

    GetExtensionFunctionPointer(sock, AcceptEx, WSAID_ACCEPTEX);
    GetExtensionFunctionPointer(sock, ConnectEx, WSAID_CONNECTEX);

    was_guard.Dismiss();
}

WinsockContext::~WinsockContext()
{
    WSACleanup();
}

}   // namespace ezio
