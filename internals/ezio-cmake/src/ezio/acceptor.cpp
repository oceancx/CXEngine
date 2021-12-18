/*
 @ 0xCCCCCCCC
*/

#include "ezio/acceptor.h"

#include "kbase/error_exception_util.h"

#include "ezio/event_loop.h"
#include "ezio/socket_utils.h"

namespace ezio {

#if defined(OS_POSIX)

int MakeSentinelFD();

#endif

Acceptor::Acceptor(EventLoop* loop, const SocketAddress& addr)
    : loop_(loop),
      listening_sock_(socket::CreateNonBlockingSocket()),
      listening_notifier_(loop, listening_sock_),
#if defined(OS_POSIX)
      sentinel_fd_(MakeSentinelFD()),
#elif defined(OS_WIN)
      accept_conn_(socket::CreateNonBlockingSocket()),
      accept_req_(IOEvent::Read),
#endif
      listening_(false)
{
    socket::SetReuseAddr(listening_sock_, true);
    socket::BindOrThrow(listening_sock_, addr);

    listening_notifier_.set_on_read(std::bind(&Acceptor::HandleNewConnection, this));
}

Acceptor::~Acceptor()
{
    listening_notifier_.DisableAll();
    listening_notifier_.Detach();
}

void Acceptor::Listen()
{
    ENSURE(CHECK, loop_->BelongsToCurrentThread()).Require();

    listening_ = true;
    listening_notifier_.EnableReading();

    socket::ListenOrThrow(listening_sock_);

#if defined(OS_WIN)
    PostAccept();
#endif
}

}   // namespace ezio
