/*
 @ 0xCCCCCCCC
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef KBASE_FILE_INFO_H_
#define KBASE_FILE_INFO_H_

#include "kbase/basic_macros.h"
#include "kbase/chrono_util.h"
#include "kbase/path.h"

namespace kbase {

class FileTime {
public:
    using time_type = TimePoint;

    constexpr FileTime() = default;

    explicit FileTime(time_type time)
        : time_(time)
    {}

    explicit FileTime(time_t time)
        : time_(time_type::clock::from_time_t(time))
    {}

#if defined(OS_WIN)
    explicit FileTime(const FILETIME& winft)
        : time_(TimePointFromWindowsFileTime(winft))
    {}

    explicit FileTime(const SYSTEMTIME& winst)
        : time_(TimePointFromWindowsSystemTime(winst))
    {}
#else
    explicit FileTime(const timespec& timespec)
        : time_(TimePointFromTimespec(timespec))
    {}
#endif

    time_type value() const
    {
        return time_;
    }

private:
    time_type time_;
};

class FileInfo {
public:
    FileInfo() noexcept
        : size_(0), is_directory_(false)
    {}

    FileInfo(const Path& path,
             int64_t size,
             bool is_directory,
#if defined(OS_WIN)
             const FileTime& creation_time,
#else
             const FileTime& last_status_changed_time,
#endif
             const FileTime& last_modified_time,
             const FileTime& last_accessed_time)
        : file_path_(path),
          size_(size),
          is_directory_(is_directory),
#if defined(OS_WIN)
          creation_time_(creation_time),
#else
          last_status_changed_time_(last_status_changed_time),
#endif
          last_modified_time_(last_modified_time),
          last_accessed_time_(last_accessed_time)
    {}

    ~FileInfo() = default;

    const Path& file_path() const
    {
        return file_path_;
    }

    int64_t file_size() const
    {
        return size_;
    }

    bool is_directory() const
    {
        return is_directory_;
    }

#if defined(OS_WIN)
    FileTime creation_time() const
    {
        return creation_time_;
    }
#else
    FileTime last_status_changed_time() const
    {
        return last_status_changed_time_;
    }
#endif

    FileTime last_modified_time() const
    {
        return last_modified_time_;
    }

    FileTime last_accessed_time() const
    {
        return last_accessed_time_;
    }

private:
    Path file_path_;
    int64_t size_;
    bool is_directory_;
#if defined(OS_WIN)
    FileTime creation_time_;
#else
    FileTime last_status_changed_time_;
#endif
    FileTime last_modified_time_;
    FileTime last_accessed_time_;
};

}   // namespace kbase

#endif  // KBASE_FILE_INFO_H_