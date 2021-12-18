/*
 @ 0xCCCCCCCC
*/

#ifndef EZIO_WORKER_POOL_H_
#define EZIO_WORKER_POOL_H_

#include <memory>
#include <string>
#include <vector>

#include "kbase/basic_macros.h"

namespace ezio {

class EventLoop;
class Thread;

class WorkerPool {
public:
    // WorkerPool's name will be the prefix of every worker thread's name.
    WorkerPool(EventLoop* main_loop, size_t worker_num, std::string name);

    ~WorkerPool();

    DISALLOW_COPY(WorkerPool);

    DISALLOW_MOVE(WorkerPool);

    const std::string& name() const noexcept
    {
        return name_;
    }

    EventLoop* GetNextEventLoop();

private:
    EventLoop* main_loop_;
    std::string name_;
    std::vector<std::unique_ptr<Thread>> workers_;
    std::vector<EventLoop*> loops_;
    size_t next_loop_idx_;
};

}   // namespace ezio

#endif  // EZIO_WORKER_POOL_H_
