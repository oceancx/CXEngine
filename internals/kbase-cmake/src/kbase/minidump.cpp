/*
 @ 0xCCCCCCCC
*/

#include "kbase/minidump.h"

#include <windows.h>
#pragma warning(push)
#pragma warning(disable: 4091)
#include <DbgHelp.h>
#pragma warning(pop)

#include "kbase/path.h"
#include "kbase/scoped_handle.h"

#if !defined(NDEBUG)
#include "kbase/error_exception_util.h"
#include "kbase/logging.h"
#endif

namespace {

using kbase::Path;
using kbase::ScopedWinHandle;

constexpr auto kStatusDumping = static_cast<DWORD>(0xE0000001);

bool GenerateMiniDumpFile(const Path& dump_path, EXCEPTION_POINTERS* ex_ptrs)
{
    ScopedWinHandle dump_file(CreateFileW(dump_path.value().c_str(),
                                          GENERIC_WRITE,
                                          0,
                                          nullptr,
                                          CREATE_ALWAYS,
                                          FILE_ATTRIBUTE_NORMAL,
                                          nullptr));
    if (!dump_file) {
#if !defined(NDEBUG)
        kbase::LastError error;
        DLOG(ERROR) << "Failed to create dump file.\n\t" << error;
#endif
        return false;
    }

    MINIDUMP_EXCEPTION_INFORMATION exception_information { 0 };
    exception_information.ThreadId = GetCurrentThreadId();
    exception_information.ClientPointers = FALSE;
    exception_information.ExceptionPointers = ex_ptrs;

    BOOL rv = MiniDumpWriteDump(GetCurrentProcess(),
                                GetCurrentProcessId(),
                                dump_file.get(),
                                MiniDumpNormal,
                                &exception_information,
                                nullptr,
                                nullptr);
#if !defined(NDEBUG)
    // According to the MSDN, error code obtained actually is a HRESULT value.
    if (!rv) {
        kbase::LastError error;
        DLOG(ERROR) << "Writing minidump error. (" << error.error_code() << ")";
    }
#endif

    return !!rv;
}

int HandleException(const Path& dump_path, EXCEPTION_POINTERS* ex_ptrs, bool& succeeded)
{
    succeeded = GenerateMiniDumpFile(dump_path, ex_ptrs);
    return EXCEPTION_EXECUTE_HANDLER;
}

}   // namespace

namespace kbase {

bool CreateMiniDump(const Path& dump_path)
{
    bool succeeded = false;

    // To make sure we end up with a valid stack trace for the calling thread, we force to
    // issue an exception on-the-fly.
    __try {
        RaiseException(kStatusDumping, 0, 0, nullptr);
    } __except (HandleException(dump_path, GetExceptionInformation(), succeeded)) {
    }

    return succeeded;
}

}   // namespace kbase
