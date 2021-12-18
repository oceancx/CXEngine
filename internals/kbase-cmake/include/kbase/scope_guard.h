/*
 @ 0xCCCCCCCC
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef KBASE_SCOPE_GUARD_H_
#define KBASE_SCOPE_GUARD_H_

#include <functional>

#include "kbase/basic_macros.h"

#define ON_SCOPE_EXIT   \
    auto ANONYMOUS_VAR(_exit_) = ::kbase::internal::ScopeGuardDriver() + [&]() noexcept

#define MAKE_SCOPE_GUARD    \
    ::kbase::internal::ScopeGuardDriver() + [&]() noexcept

namespace kbase {

class ScopeGuard {
private:
    using ExitCallback = std::function<void()>;

public:
    template<typename F>
    explicit ScopeGuard(F&& fn) noexcept
        : exit_callback_(std::forward<F>(fn)), dismissed_(false)
    {}

    // Overloaded operator+ on ScopeGuardDriver relies move-ctor.
    ScopeGuard(ScopeGuard&& other) noexcept
        : exit_callback_(std::move(other.exit_callback_)), dismissed_(other.dismissed_)
    {
        other.dismissed_ = true;
    }

    ~ScopeGuard()
    {
        if (!dismissed_) {
            exit_callback_();
        }
    }

    ScopeGuard& operator=(ScopeGuard&&) = delete;

    DISALLOW_COPY(ScopeGuard);

    void Dismiss() noexcept
    {
        dismissed_ = true;
    }

private:
    ExitCallback exit_callback_;
    bool dismissed_;
};

namespace internal {

struct ScopeGuardDriver {};

template<typename F>
auto operator+(ScopeGuardDriver, F&& fn)
{
    return ScopeGuard(std::forward<F>(fn));
}

}   // namespace internal

}   // namespace kbase

#endif // KBASE_SCOPE_GUARD_H_
