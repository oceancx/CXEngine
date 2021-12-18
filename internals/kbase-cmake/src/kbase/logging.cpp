/*
 @ 0xCCCCCCCC
*/

#include "kbase/logging.h"

#include <cstdio>
#include <chrono>
#include <thread>

#include "kbase/basic_macros.h"
#include "kbase/chrono_util.h"
#include "kbase/stack_walker.h"

#if defined(OS_WIN)
#include <Windows.h>
#elif defined(OS_POSIX)
#include <fcntl.h>
#include <unistd.h>
#endif

namespace {

using kbase::LogSeverity;
using kbase::LogItemOptions;
using kbase::LoggingDestination;
using kbase::OldFileDisposalOption;

using kbase::PathChar;
using kbase::PathString;

#if defined(OS_WIN)
using FileHandle = HANDLE;
const FileHandle kInvalidFileHandle = INVALID_HANDLE_VALUE;
#else
using FileHandle = int;
constexpr FileHandle kInvalidFileHandle = -1;
#endif

constexpr const char* kLogSeverityNames[] {"INFO", "WARNING", "ERROR", "FATAL"};

constexpr LogSeverity kAlwaysPrintErrorMinLevel = LogSeverity::LogError;

LogSeverity g_min_severity_level = LogSeverity::LogInfo;
LogItemOptions g_log_item_options = LogItemOptions::EnableTimestamp;
LoggingDestination g_logging_dest = LoggingDestination::LogToFile;
OldFileDisposalOption g_old_file_option = OldFileDisposalOption::AppendToOldFile;

PathString g_log_file_path;
FileHandle g_log_file = kInvalidFileHandle;

// Ouputs timestamp in the form like "20160126 09:14:38,456".
void OutputNowTimestamp(std::ostream& stream)
{
    auto timestamp = kbase::TimePointToLocalTime<std::chrono::milliseconds>(
        std::chrono::system_clock::now());
    const auto& tm_time = timestamp.first;
    char buf[32] {0};
    snprintf(buf, sizeof(buf), "%4d%02d%02d %02d:%02d:%02d,%03d",
             tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
             tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec,
             static_cast<int>(timestamp.second.count()));
    stream << buf;
}

template<typename charT>
constexpr const charT* ExtractFileName(const charT* file_path)
{
    const charT* p = file_path;
    const charT* last_pos = nullptr;
    for (; *p != '\0'; ++p) {
        if (*p == '/' || *p == '\\') {
            last_pos = p;
        }
    }

    return last_pos ? last_pos + 1 : file_path;
}

#if defined(OS_WIN)
using ProcessID = DWORD;
#else
using ProcessID = pid_t;
#endif

ProcessID GetCurrentProcessID()
{
#if defined(OS_WIN)
    return GetCurrentProcessId();
#else
    return getpid();
#endif
}

bool IsFileHandleValid(FileHandle handle)
{
#if defined(OS_WIN)
    return handle != INVALID_HANDLE_VALUE && handle != nullptr;
#else
    return handle != -1;
#endif
}

void DeleteFilePath(const PathString& file_path)
{
#if defined(OS_WIN)
    DeleteFileW(file_path.c_str());
#else
    unlink(file_path.c_str());
#endif
}

// Returns the default path for the log file.
// We use the same path as the EXE file on Windows; and on POSIX systems, we simply
// use current directory.
PathString GetDefaultLogFilePath()
{
#if defined(OS_WIN)
    constexpr size_t kMaxPath = MAX_PATH + 1;
    PathChar exe_path[kMaxPath];
    GetModuleFileNameW(nullptr, exe_path, kMaxPath);

    PathString default_path(exe_path);

    auto dot_pos = default_path.rfind(L".exe");
    if (dot_pos != PathString::npos) {
        default_path.erase(dot_pos);
    }

    default_path += L"_debug.log";

    return default_path;
#else
    return PathString("debug.log");
#endif
}

#if defined(OS_WIN)

// Returns the fallback path for the log file on Windows.
// We use the path in current directory as the fallback path, when using the
// default path is not possible, such as on which we don't have the access to
// write the file.
PathString GetFallbackLogFilePath()
{
    constexpr size_t kMaxPath = MAX_PATH + 1;
    PathChar cur_path[kMaxPath];
    GetCurrentDirectoryW(kMaxPath, cur_path);

    PathString fallback_path(cur_path);
    fallback_path.append(L"\\").append(L"debug.log");

    return fallback_path;
}

#endif

void CloseLogFile()
{
    if (IsFileHandleValid(g_log_file)) {
#if defined(OS_WIN)
        CloseHandle(g_log_file);
#else
        close(g_log_file);
#endif
        g_log_file = kInvalidFileHandle;
    }
}

// Once this function succeed, `g_log_file` refers to a valid and writable file.
// Returns true, if we initialized the log file successfully, false otherwise.
bool InitLogFile()
{
    if (IsFileHandleValid(g_log_file)) {
        return true;
    }

    if (g_log_file_path.empty()) {
        g_log_file_path = GetDefaultLogFilePath();
    }

    if (g_old_file_option == OldFileDisposalOption::DeleteOldFile) {
        DeleteFilePath(g_log_file_path);
    }

#if defined(OS_WIN)
    // Surprisingly, we need neither a local nor a global lock here, on Windows.
    // Because if we opened a file with `FILE_APPEND_DATA` flag only, the system
    // will ensure that each appending is atomic.
    // See https://msdn.microsoft.com/en-us/library/windows/hardware/ff548289(v=vs.85).aspx.
    g_log_file = CreateFileW(g_log_file_path.c_str(),
                             FILE_APPEND_DATA,
                             FILE_SHARE_READ | FILE_SHARE_WRITE,
                             nullptr,
                             OPEN_ALWAYS,
                             FILE_ATTRIBUTE_NORMAL,
                             nullptr);
    if (g_log_file == INVALID_HANDLE_VALUE) {
        g_log_file_path = GetFallbackLogFilePath();
        g_log_file = CreateFileW(g_log_file_path.c_str(),
                                 FILE_APPEND_DATA,
                                 FILE_SHARE_READ | FILE_SHARE_WRITE,
                                 nullptr,
                                 OPEN_ALWAYS,
                                 FILE_ATTRIBUTE_NORMAL,
                                 nullptr);
    }
#else
    // Similarly, we make atomic appending on POSIX systems, which saves us from using
    // a global lock.
    g_log_file = open(g_log_file_path.c_str(), O_CREAT | O_WRONLY | O_APPEND, 0666);
#endif

    return IsFileHandleValid(g_log_file);
}

}   // namespace

namespace kbase {

namespace internal {

LogSeverity GetMinSeverityLevel() noexcept
{
    return g_min_severity_level;
}

}   // namespace internal

LoggingSettings::LoggingSettings() noexcept
 : min_severity_level(LogSeverity::LogInfo),
   log_item_options(LogItemOptions::EnableTimestamp),
   logging_destination(LoggingDestination::LogToFile),
   old_file_disposal_option(OldFileDisposalOption::AppendToOldFile)
{}

void ConfigureLoggingSettings(const LoggingSettings& settings)
{
    g_min_severity_level = settings.min_severity_level;
    g_log_item_options = settings.log_item_options;
    g_logging_dest = settings.logging_destination;
    g_old_file_option = settings.old_file_disposal_option;

    if (!(g_logging_dest & LoggingDestination::LogToFile)) {
        return;
    }

    if (!settings.log_file_path.empty()) {
        g_log_file_path = settings.log_file_path;
    }

    CloseLogFile();

    InitLogFile();
}

LogMessage::LogMessage(const char* file, int line, LogSeverity severity)
    : file_name_(ExtractFileName(file)), line_(line), severity_(severity)
{
    InitMessageHeader();
}

LogMessage::~LogMessage()
{
    if (severity_ == LogSeverity::LogFatal) {
        stream_ << "\n";
        StackWalker walker;
        walker.DumpCallStack(stream_);
    }

    stream_ << std::endl;
    std::string msg = stream_.str();

    if ((g_logging_dest & LoggingDestination::LogToSystemDebugLog) ||
        severity_ >= kAlwaysPrintErrorMinLevel) {
#if defined(OS_WIN)
        OutputDebugStringA(msg.c_str());
#endif
        // Log to standard error stream.
        fwrite(msg.data(), sizeof(char), msg.length(), stderr);
        fflush(stderr);
    }

    // If `InitLogFile` wasn't called at the start of the program, do it on the fly.
    // However, if we unfortunately failed to initialize the log file, just skip the writting.
    // Note that, if more than one thread in here try to call `InitLogFile`, there will be a
    // race condition. This is why you should call `ConfigureLoggingSettings` at start.
    if ((g_logging_dest & LoggingDestination::LogToFile) && InitLogFile()) {
#if defined(OS_WIN)
        DWORD bytes_written = 0;
        WriteFile(g_log_file, msg.data(), static_cast<DWORD>(msg.length()), &bytes_written,
                  nullptr);
#else
        IGNORE_RESULT(write(g_log_file, msg.data(), msg.length()));
#endif
    }
}

void LogMessage::InitMessageHeader()
{
    stream_ << "[";

    if (g_log_item_options & LogItemOptions::EnableTimestamp) {
        OutputNowTimestamp(stream_);
    }

    if (g_log_item_options & LogItemOptions::EnableProcessID) {
        stream_ << " " << GetCurrentProcessID();
    }

    if (g_log_item_options & LogItemOptions::EnableThreadID) {
        stream_ << " " << std::this_thread::get_id();
    }

    stream_ << " " << kLogSeverityNames[enum_cast(severity_)]
            << " " << file_name_ << '(' << line_ << ")]";
}

}   // namespace kbase
