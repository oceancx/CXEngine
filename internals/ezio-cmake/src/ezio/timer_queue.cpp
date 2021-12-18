/*
 @ 0xCCCCCCCC
*/

#include "ezio/timer_queue.h"

#include <functional>

#include "kbase/error_exception_util.h"
#include "kbase/logging.h"
#include "kbase/scope_guard.h"

#include "ezio/event_loop.h"

#if defined(OS_POSIX) && !defined(OS_APPLE)
#include <sys/timerfd.h>
#endif

namespace {

#if defined(OS_POSIX) && !defined(OS_APPLE)

constexpr std::chrono::microseconds kMinDuration {100};

int CreateTimerFD()
{
    int tfd = timerfd_create(CLOCK_MONOTONIC, TFD_CLOEXEC | TFD_NONBLOCK);
    ENSURE(THROW, tfd != -1)(errno).Require();
    return tfd;
}

timespec ConvertDurationToTimespec(std::chrono::microseconds duration)
{
    if (duration < kMinDuration) {
        duration = kMinDuration;
    }

    auto sec_part = std::chrono::duration_cast<std::chrono::seconds>(duration);
    auto nano_part = std::chrono::duration_cast<std::chrono::nanoseconds>(duration - sec_part);

    timespec spec {};
    spec.tv_sec = sec_part.count();
    spec.tv_nsec = nano_part.count();

    return spec;
}

void ResetTimerFD(int timer_fd, ezio::TimePoint when)
{
    auto duration = when - ezio::ToTimePoint(std::chrono::system_clock::now());

    itimerspec new_spec {};
    new_spec.it_value = ConvertDurationToTimespec(duration);

    int rv = timerfd_settime(timer_fd, 0, &new_spec, nullptr);
    if (rv < 0) {
        auto err = errno;
        LOG(ERROR) << "timerfd_settime() failed errno: " << err;
        ENSURE(CHECK, kbase::NotReached())(err).Require();
    }
}

void ConsumeTimerFD(int timer_fd)
{
    uint64_t data;
    ssize_t n = read(timer_fd, &data, sizeof(data));
    LOG_IF(INFO, n != sizeof(data)) << "Failed to read from timer-fd!";
}

#endif

}   // namespace

namespace ezio {

TimerQueue::TimerQueue(EventLoop* loop)
    : loop_(loop),
#if defined(OS_POSIX) && !defined(OS_APPLE)
      timer_fd_(CreateTimerFD()),
      timer_notifier_(loop, timer_fd_),
#endif
      processing_expired_timers_(false)
{
#if defined(OS_POSIX) && !defined(OS_APPLE)
    using namespace std::placeholders;

    timer_notifier_.set_on_read(std::bind(&TimerQueue::OnTimerExpired, this, _1));
    timer_notifier_.EnableReading();
#endif
}

TimerQueue::~TimerQueue()
{
#if defined(OS_POSIX) && !defined(OS_APPLE)
    timer_notifier_.DisableAll();
    timer_notifier_.Detach();
#endif
}

TimerID TimerQueue::AddTimer(Timer::TickEventHandler handler, TimePoint when, TimeDuration interval)
{
    auto new_timer = std::make_unique<Timer>(std::move(handler), when, interval);
    TimerID timer_id(new_timer.get());

    loop_->RunTask(std::bind(&TimerQueue::AddTimerInLoop, this, new_timer.release()));

    return timer_id;
}

void TimerQueue::AddTimerInLoop(Timer* new_timer)
{
    ENSURE(CHECK, loop_->BelongsToCurrentThread()).Require();

    auto new_earliest = Insert(std::unique_ptr<Timer>(new_timer));
    if (new_earliest) {
#if defined(OS_POSIX) && !defined(OS_APPLE)
        ResetTimerFD(timer_fd_.get(), new_timer->expiration());
#elif defined(OS_WIN)
        loop_->Wakeup();
#endif
    }
}

void TimerQueue::Cancel(TimerID timer_id)
{
    loop_->RunTask(std::bind(&TimerQueue::CancelInLoop, this, timer_id));
}

void TimerQueue::CancelInLoop(TimerID timer_id)
{
    ENSURE(CHECK, loop_->BelongsToCurrentThread()).Require();

    auto range = timers_.equal_range(timer_id.timer()->expiration());
    auto it = std::find_if(range.first, range.second, [timer_id](const auto& timer) {
        return timer_id.timer() == timer.second.get();
    });

    // The function maybe called by a repeating timer to cancel itself.
    if (it == range.second) {
        if (processing_expired_timers_) {
            canceling_timers_.insert(timer_id.timer());
        }

        return;
    }

    timers_.erase(it);
}

void TimerQueue::ProcessExpiredTimers(TimePoint now)
{
    ENSURE(CHECK, loop_->BelongsToCurrentThread()).Require();

    auto end = timers_.upper_bound(now);
    ENSURE(CHECK, end == timers_.end() || now < end->first).Require();

    std::vector<std::unique_ptr<Timer>> expired;
    for (auto it = timers_.begin(); it != end; ++it) {
        expired.push_back(std::move(it->second));
    }

    timers_.erase(timers_.begin(), end);

    // Handle timer ticks.
    {
        canceling_timers_.clear();

        processing_expired_timers_ = true;
        ON_SCOPE_EXIT { processing_expired_timers_ = false; };

        for (const auto& timer : expired) {
            timer->Tick();
        }
    }

    // Restart timers or remove canceled timers.
    for (auto& timer : expired) {
        if (timer->is_repeating() && canceling_timers_.count(timer.get()) == 0) {
            timer->Restart(now);
            Insert(std::move(timer));
        }
    }
}

bool TimerQueue::Insert(std::unique_ptr<Timer> new_timer)
{
    auto when = new_timer->expiration();
    auto it = timers_.cbegin();

    bool earliest_changed = (it == timers_.cend() || when < it->first);

    timers_.emplace(std::make_pair(when, std::move(new_timer)));

    return earliest_changed;
}

std::pair<bool, TimePoint> TimerQueue::next_expiration() const
{
    ENSURE(CHECK, loop_->BelongsToCurrentThread()).Require();

    if (timers_.empty()) {
        return {false, TimePoint::max()};
    }

    return {true, timers_.begin()->first};
}

#if defined(OS_POSIX) && !defined(OS_APPLE)

void TimerQueue::OnTimerExpired(TimePoint timestamp)
{
    ENSURE(CHECK, loop_->BelongsToCurrentThread()).Require();

    ConsumeTimerFD(timer_fd_.get());

    ProcessExpiredTimers(timestamp);

    if (!timers_.empty()) {
        ResetTimerFD(timer_fd_.get(), timers_.begin()->first);
    }
}

#endif

}   // namespace ezio
