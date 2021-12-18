/*
 @ 0xCCCCCCCC
*/

#ifndef EZIO_CONNECTOR_WIN_H_
#define EZIO_CONNECTOR_WIN_H_

#include "kbase/basic_macros.h"

#include <Windows.h>

#include "ezio/connector_base.h"
#include "ezio/timer_id.h"

namespace ezio {

class ConnectorWin : public ConnectorBase {
public:
    ConnectorWin(EventLoop* loop, const SocketAddress& addr);

    ~ConnectorWin();

    DISALLOW_COPY(ConnectorWin);

    DISALLOW_MOVE(ConnectorWin);

    void Connect() override;

    void Cancel() override;

private:
    void TryCompleteConnect();

    void HandleNewConnection() override;

    void HandleError(bool restart) override;

private:
    OVERLAPPED ov;
    TimerID retry_timer_;
};

}   // namespace ezio

#endif  // EZIO_CONNECTOR_WIN_H_
