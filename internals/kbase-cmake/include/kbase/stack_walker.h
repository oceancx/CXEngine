/*
 @ 0xCCCCCCCC
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef KBASE_STACK_WALKER_H_
#define KBASE_STACK_WALKER_H_

#include <array>

#include "kbase/basic_macros.h"

#if defined(OS_WIN)
struct _CONTEXT;
using CONTEXT = _CONTEXT;
#endif

namespace kbase {

class StackWalker {
public:
    StackWalker() noexcept;

#if defined(OS_WIN)
    // Dumps a callstack for an exception case.
    explicit StackWalker(CONTEXT* context);
#endif

    ~StackWalker() = default;

    DEFAULT_COPY(StackWalker);

    DEFAULT_MOVE(StackWalker);

    void DumpCallStack(std::ostream& stream);

    std::string CallStackToString();

private:
    static constexpr size_t kMaxStackFrames = 64U;
    std::array<void*, kMaxStackFrames> stack_frames_ {{nullptr}};
    size_t valid_frame_count_ = 0;
};

}   // namespace kbase

#endif  // KBASE_STACK_WALKER_H_
