/*
 @ 0xCCCCCCCC
*/

#ifndef EZIO_EVENT_PUMP_H_
#define EZIO_EVENT_PUMP_H_

#include <chrono>
#include <memory>
#include <utility>
#include <vector>

#include "kbase/basic_macros.h"

#include "ezio/chrono_utils.h"
#include "ezio/io_context.h"

namespace ezio {

class EventLoop;
class Notifier;

using IONotification = std::pair<Notifier*, IOContext>;

class EventPump {
public:
    explicit EventPump(EventLoop* loop);

    ~EventPump();

    DISALLOW_COPY(EventPump);

    DISALLOW_MOVE(EventPump);

    TimePoint Pump(std::chrono::milliseconds timeout, std::vector<IONotification>& notifications);

    void Wakeup();

    void RegisterNotifier(Notifier* notifier);

    void UnregisterNotifier(Notifier* notifier);

private:
    static constexpr size_t kInitialEventNum = 16;

    class Impl;
    std::unique_ptr<Impl> impl_;
};

}   // namespace ezio

#endif  // EZIO_EVENT_PUMP_H_
