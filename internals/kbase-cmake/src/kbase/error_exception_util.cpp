/*
 @ 0xCCCCCCCC
*/

#include "kbase/error_exception_util.h"

#include "kbase/debugger.h"
#include "kbase/scope_guard.h"
#include "kbase/stack_walker.h"

#if defined(OS_WIN)
#include <lmerr.h>

#include "kbase/string_format.h"
#include "kbase/string_util.h"
#endif

namespace {

bool g_always_check_first_in_debug = true;

bool ShouldCheckFirst() noexcept
{
#if defined(NDEBUG)
    return false;
#else
    return g_always_check_first_in_debug;
#endif
}

}   // namespace

namespace kbase {

void AlwaysCheckFirstInDebug(bool always_check) noexcept
{
    g_always_check_first_in_debug = always_check;
}

void Guarantor::Require(StringView msg)
{
    if (!msg.empty()) {
        exception_desc_ << "Extra Message: " << msg << "\n";
    }

    StackWalker callstack;
    callstack.DumpCallStack(exception_desc_);

    if (action_ == EnsureAction::Check || ShouldCheckFirst()) {
        DoCheck();
    }

    if (action_ == EnsureAction::Throw) {
        DoThrow();
    }
}

void Guarantor::DoCheck() const
{
    std::string description = exception_desc_.str();
#if defined(OS_WIN)
    std::wstring message = UTF8ToWide(description);
    MessageBoxW(nullptr, message.c_str(), L"Checking Failed", MB_OK | MB_TOPMOST | MB_ICONHAND);
#else
    fwrite(description.data(), sizeof(char), description.length(), stderr);
    fflush(stderr);
#endif
    if (IsDebuggerPresent()) {
        BreakDebugger();
    } else {
        _Exit(EXIT_FAILURE);
    }
}

void Guarantor::DoThrow()
{
    if (!exception_pump_) {
        exception_pump_ = std::make_unique<internal::ExceptionPumpImpl<EnsureFailure>>();
    }

    exception_pump_->Throw(exception_desc_.str());
}

#if defined(OS_WIN)

// -*- LastError implementation -*-

LastError::LastError() noexcept
    : error_code_(GetLastError())
{}

DWORD LastError::error_code() const noexcept
{
    return error_code_;
}

std::wstring LastError::GetDescriptiveMessage() const
{
    constexpr DWORD kErrorMessageBufSize = 256;
    wchar_t message_buf[kErrorMessageBufSize];

    HMODULE module_handle = nullptr;
    ON_SCOPE_EXIT { if (module_handle) FreeLibrary(module_handle); };

    DWORD flags = FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS;

    if (NERR_BASE <= error_code_ && error_code_ <= MAX_NERR) {
        module_handle = LoadLibraryExW(L"netmsg.dll", nullptr, LOAD_LIBRARY_AS_DATAFILE);
        if (module_handle) {
            flags |= FORMAT_MESSAGE_FROM_HMODULE;
        }
    }

    DWORD len = FormatMessageW(flags,
                               module_handle,
                               error_code_,
                               MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),
                               message_buf,
                               kErrorMessageBufSize,
                               nullptr);

    if (len) {
        std::wstring msg(message_buf);
        return TrimTailingString(msg, L"\r\n");
    }

    return StringPrintf(L"Error (0x%X) while retrieve message for 0x%X", GetLastError(),
                        error_code_);
}

std::ostream& operator<<(std::ostream& os, const LastError& last_error)
{
    std::string error_message = WideToASCII(last_error.GetDescriptiveMessage());
    os << last_error.error_code()
       << " (" + error_message + ")";

    return os;
}

#endif

}   // namespace kbase
