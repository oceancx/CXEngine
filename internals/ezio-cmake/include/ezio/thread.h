/*
 @ 0xCCCCCCCC
*/

#ifndef EZIO_THREAD_H_
#define EZIO_THREAD_H_

#include <condition_variable>
#include <memory>
#include <mutex>
#include <string>
#include <thread>

#include "kbase/basic_macros.h"

namespace ezio {

class EventLoop;

// A Thread instance is a native thread running with an EventLoop.
class Thread {
public:
    explicit Thread(std::string name);

    ~Thread();

    DISALLOW_COPY(Thread);

    DISALLOW_MOVE(Thread);

    const std::string& name() const noexcept
    {
        return name_;
    }

    EventLoop* event_loop() const noexcept
    {
        return loop_.get();
    }

private:
    void ThreadMain();

private:
    std::string name_;
    std::mutex loop_init_mtx_;
    std::condition_variable loop_inited_;
    std::unique_ptr<std::thread> raw_thread_;
    std::unique_ptr<EventLoop> loop_;
};

}   // namespace ezio

#endif  // EZIO_THREAD_H_
