/*
 @ 0xCCCCCCCC
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef KBASE_AT_EXIT_MANAGER_H_
#define KBASE_AT_EXIT_MANAGER_H_

#include <functional>
#include <mutex>
#include <stack>

#include "kbase/basic_macros.h"

namespace kbase {

// We can control the execution time of cleanup callbacks via `AtExitManager`, especially in
// a DLL on Windows.
// Only one instance is allowed in a module.

class AtExitManager {
public:
    using ExitCallback = std::function<void()>;

    AtExitManager() noexcept;

    ~AtExitManager();

    DISALLOW_COPY(AtExitManager);

    DISALLOW_MOVE(AtExitManager);

    static void RegisterCallback(ExitCallback callback);

private:
    static void ProcessExitCallback();

private:
    std::mutex lock_;
    std::stack<ExitCallback> exit_callbacks_;
};

}   // namespace kbase

#endif  // KBASE_AT_EXIT_MANAGER_H_