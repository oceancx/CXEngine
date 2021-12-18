/*
 @ 0xCCCCCCCC
*/

#include "ezio/thread.h"

#include "kbase/error_exception_util.h"

#include "ezio/event_loop.h"
#include "ezio/this_thread.h"

namespace ezio {

Thread::Thread(std::string name)
    : name_(std::move(name)),
      raw_thread_(std::make_unique<std::thread>(std::bind(&Thread::ThreadMain, this)))
{
    std::unique_lock<std::mutex> lock(loop_init_mtx_);
    loop_inited_.wait(lock, [this] { return loop_.get() != nullptr; });
}

Thread::~Thread()
{
    // In case the loop quits before the destruction.
    if (loop_) {
        loop_->Quit();
    }

    raw_thread_->join();
}

void Thread::ThreadMain()
{
    // EventLoop must be constructed on the target thread, and must die before the thread
    // exits.

    auto loop = std::make_unique<EventLoop>();

    {
        std::lock_guard<std::mutex> lock(loop_init_mtx_);
        loop_ = std::move(loop);
        loop_inited_.notify_one();
    }

    this_thread::SetName(name_.c_str());

    loop_->Run();

    loop_ = nullptr;
}

}   // namespace ezio
