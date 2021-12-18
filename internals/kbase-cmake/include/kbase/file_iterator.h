/*
 @ 0xCCCCCCCC
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef KBASE_FILE_ITERATOR_H_
#define KBASE_FILE_ITERATOR_H_

#include <memory>
#include <utility>
#include <vector>

#include "kbase/basic_macros.h"
#include "kbase/file_info.h"
#include "kbase/path.h"
#include "kbase/scoped_handle.h"
#include "kbase/string_view.h"

#if defined(OS_WIN)
#include <Windows.h>
#else
#include <dirent.h>
#endif

namespace kbase {

class FileIterator {
public:
    using iterator_category = std::input_iterator_tag;
    using value_type = FileInfo;
    using pointer = const FileInfo*;
    using reference = const FileInfo&;
    using difference_type = ptrdiff_t;

    FileIterator() noexcept;

    // If recursive mode is specified, the iterator does a BFS, so all files in one
    // directory will be returned before any files in a subdirectory.
    FileIterator(const Path& dir_path, bool recursive);

    ~FileIterator() = default;

    DEFAULT_COPY(FileIterator);

    DEFAULT_MOVE(FileIterator);

    reference operator*() const noexcept
    {
        return cur_entry_;
    }

    pointer operator->() const noexcept
    {
        return &cur_entry_;
    }

    FileIterator& operator++();

    FileIterator operator++(int);

    friend bool operator==(const FileIterator& lhs, const FileIterator& rhs) noexcept;

    friend bool operator!=(const FileIterator& lhs, const FileIterator& rhs) noexcept;

private:
    void GetNext();

    bool Equal(const FileIterator& other) const noexcept;

    bool ReachedEnd() const noexcept;

    // The special pathnames dot and dot-dot are skipped by default.
    static bool ShouldSkip(Path::string_view_type pathname) noexcept;

private:
    // <dir-path, pending-paths>
    using WalkingTrackData = std::pair<Path, std::vector<Path>>;
    std::shared_ptr<WalkingTrackData> walking_track_data_;

    // Platform-specific directory entry walking handle.
    struct WalkingHandleTraits;
    using DirWalkingHandle = GenericScopedHandle<WalkingHandleTraits>;

    std::shared_ptr<DirWalkingHandle> dir_walking_handle_;

#if defined(OS_WIN)
    WIN32_FIND_DATA find_data_;
#else
    dirent dir_entry_buf_;
#endif

    bool recursive_;
    value_type cur_entry_;
};

inline const FileIterator& begin(const FileIterator& iter) noexcept
{
    return iter;
}

inline FileIterator end(const FileIterator&) noexcept
{
    return FileIterator();
}

}   // namespace kbase

#endif  // KBASE_FILE_ITERATOR_H_
