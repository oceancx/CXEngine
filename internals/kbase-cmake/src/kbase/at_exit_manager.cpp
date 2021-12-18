/*
 @ 0xCCCCCCCC
*/

#include "kbase/at_exit_manager.h"

#include "kbase/error_exception_util.h"

namespace {

kbase::AtExitManager* exit_manager = nullptr;

}   // namespace

namespace kbase {

AtExitManager::AtExitManager() noexcept
{
    // We allow only one AtExitManager instance in a module.
    ENSURE(CHECK, exit_manager == nullptr).Require();
    exit_manager = this;
}

AtExitManager::~AtExitManager()
{
    ENSURE(CHECK, exit_manager == this).Require();
    ProcessExitCallback();
    exit_manager = nullptr;
}

// static
void AtExitManager::RegisterCallback(ExitCallback callback)
{
    if (!exit_manager) {
        ENSURE(CHECK, NotReached()).Require();
        return;
    }

    std::lock_guard<std::mutex> lock(exit_manager->lock_);
    exit_manager->exit_callbacks_.push(std::move(callback));
}

// static
void AtExitManager::ProcessExitCallback()
{
    if (!exit_manager) {
        ENSURE(CHECK, NotReached()).Require();
        return;
    }

    std::lock_guard<std::mutex> lock(exit_manager->lock_);

    while (!exit_manager->exit_callbacks_.empty()) {
        auto& task = exit_manager->exit_callbacks_.top();
        task();
        exit_manager->exit_callbacks_.pop();
    }
}

}   // namespace kbase