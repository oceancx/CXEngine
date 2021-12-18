/*
 @ 0xCCCCCCCC
*/

#ifndef EZIO_WINSOCK_CONTEXT_H_
#define EZIO_WINSOCK_CONTEXT_H_

#include <WinSock2.h>
#include <MSWSock.h>

namespace ezio {

struct WinsockContext {
    WinsockContext();

    ~WinsockContext();

    LPFN_ACCEPTEX AcceptEx;
    LPFN_CONNECTEX ConnectEx;
};

}   // namespace ezio

#endif  // EZIO_WINSOCK_CONTEXT_H_
