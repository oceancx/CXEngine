/*
 @ 0xCCCCCCCC
*/

#include "kbase/stack_walker.h"

#include <cxxabi.h>
#include <execinfo.h>

#include <sstream>

#include "kbase/scope_guard.h"

namespace {

void ResolveCallstackSymbols(void* const* stack_frames, int frame_count, std::ostream& stream)
{
    char** symbol_list = backtrace_symbols(stack_frames, frame_count);
    ON_SCOPE_EXIT { free(symbol_list); };
    if (!symbol_list) {
        stream << "Failed to resolve stack frames. Dumping unresolved callstack:\n";
        for (int i = 0; i < frame_count; ++i) {
            stream << "\t" << stack_frames[i] << "\n";
        }

        return;
    }

    size_t demangle_buf_size = 256U;
    char* demangle_buf = static_cast<char*>(malloc(demangle_buf_size));
    ON_SCOPE_EXIT { free(demangle_buf); };

    for (int i = 0; i < frame_count; ++i) {
        std::string symbol_info(symbol_list[i]);

        auto begin_parenthesis = symbol_info.find('(');
        auto begin_func_name = begin_parenthesis + 1;
        auto end_func_name = symbol_info.find('+', begin_func_name);

        // No function name found. Maybe a sole module frame.
        if (begin_parenthesis == std::string::npos || end_func_name == std::string::npos) {
            stream << "\t" << symbol_info << "\n";
            continue;
        }

        int demangle_result;
        symbol_info[end_func_name] = '\0';

        char* demangled_name = abi::__cxa_demangle(&symbol_info[begin_func_name], demangle_buf,
                                                   &demangle_buf_size, &demangle_result);

        symbol_info[end_func_name] = '+';
        if (demangle_result == 0) {
            demangle_buf = demangled_name;
            symbol_info.replace(begin_func_name, end_func_name - begin_func_name, demangled_name);
        } else {
            symbol_info.append(" (Not demangled)");
        }

        stream << "\t" << symbol_info << "\n";
    }
}

}    // namespace

namespace kbase {

StackWalker::StackWalker() noexcept
{
    auto captured_frame_count = backtrace(stack_frames_.data(),
                                          static_cast<int>(stack_frames_.size()));
    valid_frame_count_ = captured_frame_count;
}

void StackWalker::DumpCallStack(std::ostream& stream)
{
    if (valid_frame_count_ == 0) {
        stream << "Empty stack frame, possibily corrupted.\n";
        return;
    }

    ResolveCallstackSymbols(stack_frames_.data(), static_cast<int>(valid_frame_count_), stream);
}

std::string StackWalker::CallStackToString()
{
    std::ostringstream callstack_stream;
    DumpCallStack(callstack_stream);

    return callstack_stream.str();
}

}   // namespace kbase
