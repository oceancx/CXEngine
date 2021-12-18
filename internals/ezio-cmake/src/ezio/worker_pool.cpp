/*
 @ 0xCCCCCCCC
*/

#include "ezio/worker_pool.h"

#include "kbase/error_exception_util.h"
#include "kbase/string_format.h"

#include "ezio/event_loop.h"
#include "ezio/thread.h"

namespace ezio {

WorkerPool::WorkerPool(EventLoop* main_loop, size_t worker_num, std::string name)
    : main_loop_(main_loop),
      name_(std::move(name)),
      next_loop_idx_(0)
{
    ENSURE(CHECK, main_loop_->BelongsToCurrentThread()).Require();
    ENSURE(CHECK, worker_num > 0).Require();

    for (size_t i = 0; i < worker_num; ++i) {
        auto worker_name = kbase::StringFormat("{0}-{1}", name_, i);
        auto worker = std::make_unique<Thread>(std::move(worker_name));

        loops_.push_back(worker->event_loop());
        workers_.push_back(std::move(worker));
    }
}

WorkerPool::~WorkerPool()
{}

EventLoop* WorkerPool::GetNextEventLoop()
{
    ENSURE(CHECK, main_loop_->BelongsToCurrentThread()).Require();
    ENSURE(CHECK, next_loop_idx_ < loops_.size())(next_loop_idx_).Require();

    auto loop = loops_[next_loop_idx_++];

    if (next_loop_idx_ >= loops_.size()) {
        next_loop_idx_ = 0;
    }

    return loop;
}

}   // namespace ezio
