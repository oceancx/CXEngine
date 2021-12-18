/*
 @ 0xCCCCCCCC
*/

#ifndef EZIO_CONNECTOR_POSIX_H_
#define EZIO_CONNECTOR_POSIX_H_

#include <memory>

#include "kbase/basic_macros.h"

#include "ezio/connector_base.h"
#include "ezio/notifier.h"
#include "ezio/timer_id.h"

namespace ezio {

class ConnectorPosix : public ConnectorBase {
public:
    ConnectorPosix(EventLoop* loop, const SocketAddress& addr);

    ~ConnectorPosix();

    DISALLOW_COPY(ConnectorPosix);

    DISALLOW_MOVE(ConnectorPosix);

    void Connect() override;

    void Cancel() override;

private:
    void WaitForConnectingComplete();

    // This function should be called during notification handling.
    void ResetNotifier();

    void HandleNewConnection() override;

    void HandleError(bool restart) override;

private:
    std::unique_ptr<Notifier> sock_notifier_;
    TimerID retry_timer_;
    bool waiting_completion_;
};

}   // namespace ezio

#endif  // EZIO_CONNECTOR_POSIX_H_
