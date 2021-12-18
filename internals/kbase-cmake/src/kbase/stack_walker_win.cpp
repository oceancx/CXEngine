/*
 @ 0xCCCCCCCC
*/

#include "kbase/stack_walker.h"

#include <mutex>
#include <sstream>

#pragma warning(push)
#pragma warning(disable: 4091)
#include <DbgHelp.h>
#pragma warning(pop)

#include "kbase/singleton.h"
#include "kbase/string_util.h"

#pragma comment(lib, "dbghelp.lib")

namespace {

using kbase::DefaultSingletonTraits;
using kbase::LeakySingletonTraits;
using kbase::Singleton;

std::wstring GetExecutableDirectory()
{
    std::wstring exe_path;
    wchar_t* data = kbase::WriteInto(exe_path, MAX_PATH);
    GetModuleFileNameW(nullptr, data, MAX_PATH);
    auto slash_pos = exe_path.rfind(L'\\');
    exe_path.resize(slash_pos);

    return exe_path;
}

// A helper class to resolve symbol information in callstack.
// Make the class a singleton to keep the symbol handler alive within the entire
// process lifecycle(initializing symbol handler is expensive). And use leaky
// traits just in case some components using this class during application exit.
class SymbolContext {
public:
    ~SymbolContext() = default;

    DISALLOW_COPY(SymbolContext);

    DISALLOW_MOVE(SymbolContext);

    static SymbolContext* GetInstance()
    {
        return Singleton<SymbolContext, LeakySingletonTraits<SymbolContext>>::instance();
    }

    void ResolveCallStackToStream(const void* const* stack_frames,
                                  size_t frame_count,
                                  std::ostream& os) const;

    bool SymbolHandlerInited() const
    {
        return init_error_code_ == ERROR_SUCCESS;
    }

    unsigned long error_code() const
    {
        return init_error_code_;
    }

private:
    SymbolContext()
        : init_error_code_(0)
    {
        InitSymbolHandler();
    }

    void InitSymbolHandler();

    friend LeakySingletonTraits<SymbolContext>;

private:
    DWORD init_error_code_;
    mutable std::mutex mutex_;
};

void SymbolContext::InitSymbolHandler()
{
    SymSetOptions(SYMOPT_DEFERRED_LOADS |
                  SYMOPT_FAIL_CRITICAL_ERRORS |
                  SYMOPT_LOAD_LINES |
                  SYMOPT_UNDNAME);

    if (!SymInitializeW(GetCurrentProcess(), nullptr, TRUE)) {
        init_error_code_ = GetLastError();
        return;
    }

    // The directory that contains the executable file is not on the list by default,
    // append it on our onw.
    constexpr size_t kMaxListSize = 1024;
    wchar_t search_path_buffer[kMaxListSize];

    if (!SymGetSearchPathW(GetCurrentProcess(), search_path_buffer, kMaxListSize)) {
        init_error_code_ = GetLastError();
        return;
    }

    std::wstring search_path_list(search_path_buffer);
    search_path_list.append(L";").append(GetExecutableDirectory());

    if (!SymSetSearchPathW(GetCurrentProcess(), search_path_list.c_str())) {
        init_error_code_ = GetLastError();
        return;
    }

    init_error_code_ = ERROR_SUCCESS;
}

void SymbolContext::ResolveCallStackToStream(const void* const* stack_frames, size_t frame_count,
                                             std::ostream& os) const
{
    std::lock_guard<std::mutex> lock(mutex_);

    constexpr int kMaxSymName = 512;
    constexpr int kSymbolInfoBufferSize = sizeof(SYMBOL_INFO) + kMaxSymName * sizeof(char);
    uint8_t buffer[kSymbolInfoBufferSize];

    for (size_t i = 0; i < frame_count; ++i) {
        DWORD64 frame = reinterpret_cast<DWORD64>(stack_frames[i]);
        memset(buffer, 0, sizeof(buffer));

        PSYMBOL_INFO symbol_info = reinterpret_cast<PSYMBOL_INFO>(buffer);
        symbol_info->SizeOfStruct = sizeof(SYMBOL_INFO);
        symbol_info->MaxNameLen = kMaxSymName;
        DWORD64 symbol_displacement = 0;
        BOOL symbol_resolved = SymFromAddr(GetCurrentProcess(), frame, &symbol_displacement,
                                           symbol_info);

        DWORD line_displacement = 0;
        IMAGEHLP_LINE64 line_info {sizeof(IMAGEHLP_LINE64)};
        BOOL line_resolved = SymGetLineFromAddr64(GetCurrentProcess(), frame, &line_displacement,
                                                  &line_info);

        os << "\t";
        if (symbol_resolved) {
            os << symbol_info->Name << " [0x" << stack_frames[i] << "+" << symbol_displacement
                << "]";
        } else {
            os << "(No Symbol) [0x" << stack_frames[i] << "]";
        }

        if (line_resolved) {
            os << " (" << line_info.FileName << ":" << line_info.LineNumber << ")";
        }

        os << "\n";
    }
}

}   // namespace

namespace kbase {

// Prevent stack frames getting messed by FPO.
#pragma optimize("", off)
StackWalker::StackWalker() noexcept
{
    auto captured_frame_count = RtlCaptureStackBackTrace(0,
                                                         static_cast<DWORD>(stack_frames_.size()),
                                                         stack_frames_.data(),
                                                         nullptr);
    valid_frame_count_ = captured_frame_count;
}
#pragma optimize("", on)

StackWalker::StackWalker(CONTEXT* context)
{
    // Force initializing symbo handler.
    // It's important to call SymInitialize() on x64, before calling StackWalk64();
    // Otherwise, StackWalk64() would get incorrect stack frames.
    SymbolContext::GetInstance();
    context->ContextFlags = CONTEXT_FULL;
    STACKFRAME64 stack_frame {0};
#if defined(_WIN64)
    DWORD machine_arch = IMAGE_FILE_MACHINE_AMD64;
    stack_frame.AddrPC.Offset = context->Rip;
    stack_frame.AddrFrame.Offset = context->Rsp;
    stack_frame.AddrStack.Offset = context->Rsp;
#else
    DWORD machine_arch = IMAGE_FILE_MACHINE_I386;
    stack_frame.AddrPC.Offset = context->Eip;
    stack_frame.AddrFrame.Offset = context->Ebp;
    stack_frame.AddrStack.Offset = context->Esp;
#endif
    stack_frame.AddrPC.Mode = AddrModeFlat;
    stack_frame.AddrFrame.Mode = AddrModeFlat;
    stack_frame.AddrStack.Mode = AddrModeFlat;

    while (valid_frame_count_ < stack_frames_.size() &&
           StackWalk64(machine_arch,
                       GetCurrentProcess(),
                       GetCurrentThread(),
                       &stack_frame,
                       context,
                       nullptr,
                       &SymFunctionTableAccess64,
                       &SymGetModuleBase64,
                       nullptr)) {
        if (stack_frame.AddrPC.Offset == 0) {
            break;
        }

        stack_frames_[valid_frame_count_++] = reinterpret_cast<void*>(stack_frame.AddrPC.Offset);
    }
}

void StackWalker::DumpCallStack(std::ostream& stream)
{
    if (!SymbolContext::GetInstance()->SymbolHandlerInited()) {
        stream << "Failed to initialize symbols ("
               << SymbolContext::GetInstance()->error_code()
               << "). Dumping unresolved callstack:\n";
        for (size_t i = 0; i < valid_frame_count_; ++i) {
            stream << "\t" << stack_frames_[i] << "\n";
        }
    } else {
        stream << "Dumping callstack:\n";
        SymbolContext::GetInstance()->ResolveCallStackToStream(stack_frames_.data(),
                                                               valid_frame_count_,
                                                               stream);
    }
}

std::string StackWalker::CallStackToString()
{
    std::ostringstream callstack;
    DumpCallStack(callstack);

    return callstack.str();
}

}   // namespace kbase
