/*
 @ 0xCCCCCCCC
*/

#include "kbase/file_iterator.h"

namespace kbase {

struct FileIterator::WalkingHandleTraits {
    using Handle = HANDLE;

    WalkingHandleTraits() = delete;

    ~WalkingHandleTraits() = delete;

    static Handle NullHandle() noexcept
    {
        return INVALID_HANDLE_VALUE;
    }

    static bool IsValid(Handle handle) noexcept
    {
        return handle != INVALID_HANDLE_VALUE && handle != nullptr;
    }

    static void Close(Handle handle) noexcept
    {
        FindClose(handle);
    }
};

FileIterator::FileIterator() noexcept
    : dir_walking_handle_(std::make_shared<DirWalkingHandle>()),
      find_data_(),
      recursive_(false)
{}

FileIterator::FileIterator(const Path& dir_path, bool recursive)
    : walking_track_data_(std::make_shared<WalkingTrackData>()),
      dir_walking_handle_(std::make_shared<DirWalkingHandle>()),
      recursive_(recursive)
{
    walking_track_data_->second.push_back(dir_path);
    memset(&find_data_, 0, sizeof(find_data_));

    GetNext();
}

void FileIterator::GetNext()
{
    while (!walking_track_data_->second.empty() || *dir_walking_handle_) {
        if (!*dir_walking_handle_) {
            // Pick a directory entry from pending paths.
            walking_track_data_->first = walking_track_data_->second.back();
            walking_track_data_->second.pop_back();

            auto dir = walking_track_data_->first.AppendWith(L"*");

            dir_walking_handle_->reset(FindFirstFileExW(dir.value().c_str(),
                                                        FindExInfoBasic,
                                                        &find_data_,
                                                        FindExSearchNameMatch,
                                                        nullptr,
                                                        FIND_FIRST_EX_LARGE_FETCH));
        } else {
            if (!FindNextFileW(dir_walking_handle_->get(), &find_data_)) {
                dir_walking_handle_->reset();
            }
        }

        if (!*dir_walking_handle_) {
            continue;
        }

        if (ShouldSkip(find_data_.cFileName)) {
            continue;
        }

        auto entry_path = walking_track_data_->first.AppendWith(find_data_.cFileName);

        // Ignores reparse point directories in case we are stuck in an infinite cycle.
        if (recursive_ &&
            (find_data_.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) &&
            !(find_data_.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT)) {
            walking_track_data_->second.push_back(entry_path);
        }

        // Update the entry.

        ULARGE_INTEGER file_size;
        file_size.LowPart = find_data_.nFileSizeLow;
        file_size.HighPart = find_data_.nFileSizeHigh;

        cur_entry_ = FileInfo(entry_path,
                              file_size.QuadPart,
                              !!(find_data_.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY),
                              FileTime(find_data_.ftCreationTime),
                              FileTime(find_data_.ftLastWriteTime),
                              FileTime(find_data_.ftLastAccessTime));

        return;
    }
}

bool FileIterator::ReachedEnd() const noexcept
{
    return !*dir_walking_handle_;
}

bool FileIterator::Equal(const FileIterator& other) const noexcept
{
    return dir_walking_handle_ != nullptr && other.dir_walking_handle_ != nullptr ?
            dir_walking_handle_->get() == other.dir_walking_handle_->get() :
            dir_walking_handle_ == nullptr && other.dir_walking_handle_ == nullptr;
}

}   // namespace kbase
