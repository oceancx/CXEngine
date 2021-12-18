/*
 @ 0xCCCCCCCC
*/

#include "ezio/event_loop.h"

#include <vector>

#include "kbase/error_exception_util.h"
#include "kbase/logging.h"
#include "kbase/scope_guard.h"

#include "ezio/notifier.h"

namespace {

constexpr auto kPumpTimeout = std::chrono::seconds(10);

}   // namespace

namespace ezio {

thread_local EventLoop* tls_loop_in_thread {nullptr};

EventLoop::EventLoop()
    : is_running_(false),
      owner_thread_id_(this_thread::GetID()),
      event_pump_(this),
      timer_queue_(this),
      executing_pending_task_(false)
{
    ENSURE(CHECK, tls_loop_in_thread == nullptr).Require();
    tls_loop_in_thread = this;
}

EventLoop::~EventLoop()
{
    ENSURE(CHECK, tls_loop_in_thread != nullptr).Require();
    tls_loop_in_thread = nullptr;
}

void EventLoop::Run()
{
    ENSURE(CHECK, !is_running_.load()).Require();
    ENSURE(CHECK, BelongsToCurrentThread()).Require();

    std::vector<IONotification> active_notifications;

    is_running_.store(true, std::memory_order_release);
    while (is_running_.load(std::memory_order_acquire)) {
        auto pumped_time = event_pump_.Pump(GetPumpTimeout(), active_notifications);

        // We handle expired timers right here on Windows, while they are handled on Linux
        // inside timer_fd notifier.
#if defined(OS_WIN)
        timer_queue_.ProcessExpiredTimers(pumped_time);
#endif

        for (const auto& item : active_notifications) {
            item.first->HandleEvent(pumped_time, item.second);
        }

        ProcessPendingTasks();

        active_notifications.clear();
    }
}

void EventLoop::Quit()
{
    is_running_.store(false, std::memory_order_release);

    if (!BelongsToCurrentThread()) {
        Wakeup();
    }
}

// static
EventLoop* EventLoop::current() noexcept
{
    return tls_loop_in_thread;
}

void EventLoop::RunTask(Task task)
{
    if (BelongsToCurrentThread()) {
        task();
    } else {
        QueueTask(std::move(task));
    }
}

TimerID EventLoop::RunTaskAfter(Task task, TimeDuration delay)
{
    return timer_queue_.AddTimer(std::move(task),
                                 ToTimePoint(std::chrono::system_clock::now()) + delay,
                                 TimeDuration::zero());
}

TimerID EventLoop::RunTaskAt(Task task, TimePoint when)
{
    return timer_queue_.AddTimer(std::move(task),
                                 when,
                                 TimeDuration::zero());
}

TimerID EventLoop::RunTaskEvery(Task task, TimeDuration interval)
{
    return timer_queue_.AddTimer(std::move(task),
                                 ToTimePoint(std::chrono::system_clock::now()) + interval,
                                 interval);
}

void EventLoop::CancelTimedTask(TimerID timer_id)
{
    timer_queue_.Cancel(timer_id);
}

void EventLoop::QueueTask(Task task)
{
    {
        std::lock_guard<std::mutex> lock(task_queue_mutex_);
        task_queue_.push_back(std::move(task));
    }

    if (!BelongsToCurrentThread() || executing_pending_task_) {
        Wakeup();
    }
}

void EventLoop::RegisterNotifier(Notifier* notifier)
{
    event_pump_.RegisterNotifier(notifier);
}

void EventLoop::UnregisterNotifier(Notifier* notifier)
{
    event_pump_.UnregisterNotifier(notifier);
}

std::chrono::milliseconds EventLoop::GetPumpTimeout() const
{
#if defined(OS_POSIX)
    return kPumpTimeout;
#elif defined(OS_WIN)
    auto expiration = timer_queue_.next_expiration();
    if (!expiration.first) {
        return kPumpTimeout;
    }

    auto now = ToTimePoint(std::chrono::system_clock::now());
    auto timeout = expiration.second - now;
    if (timeout < TimeDuration::zero()) {
        LOG(WARNING) << "Negative timeout; next expiration: "
                     << expiration.second.time_since_epoch().count()
                     << "; now: " << now.time_since_epoch().count();
        timeout = TimeDuration::zero();
    }

    return timeout;
#endif
}

void EventLoop::ProcessPendingTasks()
{
    executing_pending_task_ = true;
    ON_SCOPE_EXIT { executing_pending_task_ = false; };

    decltype(task_queue_) pending_tasks;

    {
        std::lock_guard<std::mutex> lock(task_queue_mutex_);
        pending_tasks.swap(task_queue_);
    }

    for (auto& task : pending_tasks) {
        task();
    }
}

}   // namespace ezio
