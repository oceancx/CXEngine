/*
 @ 0xCCCCCCCC
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef KBASE_LOGGING_H_
#define KBASE_LOGGING_H_

#include <sstream>
#include <string>

#include "kbase/basic_macros.h"
#include "kbase/basic_types.h"

namespace kbase {

// For keeping this facility independent as possible, try not to introduce other modules,
// such as `Path`, `PathService` etc.

// These log severities will be used as index into the array `log_severity_names`.
enum class LogSeverity : int {
    LogInfo = 0,
    LogWarning,
    LogError,
    LogFatal
};

namespace internal {

LogSeverity GetMinSeverityLevel() noexcept;

}   // namespace internal

enum LogItemOptions {
    EnableNone = 0,
    EnableProcessID = 1 << 0,
    EnableThreadID = 1 << 1,
    EnableTimestamp = 1 << 2,
    EnableAll = EnableProcessID | EnableThreadID | EnableTimestamp
};

enum LoggingDestination {
    LogNone = 0,
    LogToFile = 1 << 0,
    LogToSystemDebugLog = 1 << 1,
    LogToAll = LogToFile | LogToSystemDebugLog
};

enum OldFileDisposalOption {
    AppendToOldFile,
    DeleteOldFile
};

struct LoggingSettings {
    // Initializes to default values.
    // Note that, if `log_file_path` wasn't specified, use default path.
    LoggingSettings() noexcept;

    LogSeverity min_severity_level;
    LogItemOptions log_item_options;
    LoggingDestination logging_destination;
    OldFileDisposalOption old_file_disposal_option;
    PathString log_file_path;
};

// You should better configure these settings at the beginning of the program, or
// default settings are applied.
// Note that, calling this function during the logging in a multithreaded context
// is not safe.
void ConfigureLoggingSettings(const LoggingSettings& settings);

// Surprisingly, a macro `ERROR` is defined as 0 in file <wingdi.h>, which is
// included by <windows.h>, so we add a special macro to handle this peculiar
// chaos, in case the file was included.
#define COMPACT_LOG_INFO \
    kbase::LogMessage(__FILE__, __LINE__, kbase::LogSeverity::LogInfo)
#define COMPACT_LOG_WARNING \
    kbase::LogMessage(__FILE__, __LINE__, kbase::LogSeverity::LogWarning)
#define COMPACT_LOG_ERROR \
    kbase::LogMessage(__FILE__, __LINE__, kbase::LogSeverity::LogError)
#define COMPACT_LOG_0 \
    kbase::LogMessage(__FILE__, __LINE__, kbase::LogSeverity::LogError)
#define COMPACT_LOG_FATAL \
    kbase::LogMessage(__FILE__, __LINE__, kbase::LogSeverity::LogFatal)

#define LOG_SEVERITY_FOR_INFO kbase::LogSeverity::LogInfo
#define LOG_SEVERITY_FOR_WARNING kbase::LogSeverity::LogWarning
#define LOG_SEVERITY_FOR_ERROR kbase::LogSeverity::LogError
#define LOG_SEVERITY_FOR_0 kbase::LogSeverity::LogError
#define LOG_SEVERITY_FOR_FATAL kbase::LogSeverity::LogFatal

#define LOG_IS_ON(severity) \
    ((LOG_SEVERITY_FOR_##severity) >= kbase::internal::GetMinSeverityLevel())

#if !defined(NDEBUG)
#define DLOG_IS_ON(severity) LOG_IS_ON(severity)
#else
#define DLOG_IS_ON(severity) false
#endif

#define LAZY_STREAM(stream, condition) \
    !(condition) ? (void)0 : kbase::LogMessageVoidfy() & (stream)
#define LOG_STREAM(severity) \
    COMPACT_LOG_##severity.stream()

#define LOG(severity) \
    LAZY_STREAM(LOG_STREAM(severity), LOG_IS_ON(severity))
#define LOG_IF(severity, condition) \
    LAZY_STREAM(LOG_STREAM(severity), LOG_IS_ON(severity) && (condition))

#define DLOG(severity) \
    LAZY_STREAM(LOG_STREAM(severity), DLOG_IS_ON(severity))
#define DLOG_IF(severity, condition) \
    LAZY_STREAM(LOG_STREAM(severity), DLOG_IS_ON(severity) && (condition))

class LogMessage {
public:
    LogMessage(const char* file, int line, LogSeverity severity);

    ~LogMessage();

    DISALLOW_COPY(LogMessage);

    DISALLOW_MOVE(LogMessage);

    std::ostream& stream() noexcept
    {
        return stream_;
    }

private:
    // Writes the common message header into the stream.
    // The complete message header is in the following format:
    //   [YYYYmmdd HHMMSS,ms-part pid tid severity filename(line)]
    // in which, pid, tid and timestamp are optional, though timestamp by default
    // is enabled.
    void InitMessageHeader();

private:
    const char* file_name_;
    int line_;
    LogSeverity severity_;
    std::ostringstream stream_;
};

// Used to suppress compiler warning or intellisense error.
struct LogMessageVoidfy {
    void operator&(const std::ostream&) const noexcept
    {}
};

}   // namespace kbase

#endif  // KBASE_LOGGING_H_
