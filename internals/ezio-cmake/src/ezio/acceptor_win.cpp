/*
 @ 0xCCCCCCCC
*/

#include "ezio/acceptor.h"

#include "kbase/error_exception_util.h"
#include "kbase/logging.h"

#include "ezio/io_service_context.h"
#include "ezio/socket_utils.h"

namespace {

// This memory block is required by AcceptEx(), however, we don't use it.
constexpr DWORD kAddrLen = sizeof(sockaddr_in) + 16;
char dummy_addr_block[kAddrLen * 2];

}   // namespace

namespace ezio {

void Acceptor::PostAccept()
{
    DWORD recv_len = 0;
    IOServiceContext::current().AsWinsockContext().AcceptEx(listening_sock_.get(),
                                                            accept_conn_.get(),
                                                            dummy_addr_block,
                                                            0,
                                                            kAddrLen,
                                                            kAddrLen,
                                                            &recv_len,
                                                            &accept_req_);
}

void Acceptor::HandleNewConnection()
{
    auto listener = listening_sock_.get();
    int rv = setsockopt(accept_conn_.get(), SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT,
                        reinterpret_cast<const char*>(&listener), sizeof(listener));
    if (rv < 0) {
        auto err = WSAGetLastError();
        LOG(ERROR) << "Set accept-conn SO_UPDATE_ACCEPT_CONTEXT failed: " << err;
        ENSURE(CHECK, kbase::NotReached())(err).Require();
    } else {
        sockaddr_in addr {};
        int addr_len = sizeof(addr);
        rv = getpeername(accept_conn_.get(), reinterpret_cast<sockaddr*>(&addr), &addr_len);
        LOG_IF(ERROR, rv != 0) << "getpeername() failed: " << WSAGetLastError();

        if (on_new_connection_) {
            on_new_connection_(std::move(accept_conn_), SocketAddress(addr));
        } else {
            accept_conn_ = nullptr;
            LOG(WARNING) << "No handler set for new connections!";
        }
    }

    accept_req_.Reset();
    accept_conn_ = socket::CreateNonBlockingSocket();

    PostAccept();
}

}   // namespace ezio
