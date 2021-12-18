/*
 @ 0xCCCCCCCC
*/

#include "kbase/file_iterator.h"

#include <cstring>

#include <sys/stat.h>

namespace kbase {

struct FileIterator::WalkingHandleTraits {
    using Handle = DIR*;

    WalkingHandleTraits() = delete;

    ~WalkingHandleTraits() = delete;

    static Handle NullHandle() noexcept
    {
        return nullptr;
    }

    static bool IsValid(Handle handle) noexcept
    {
        return handle != nullptr;
    }

    static void Close(Handle handle) noexcept
    {
        closedir(handle);
    }
};

FileIterator::FileIterator() noexcept
    : dir_walking_handle_(std::make_shared<DirWalkingHandle>()),
      recursive_(false)
{}

FileIterator::FileIterator(const Path& dir_path, bool recursive)
    : walking_track_data_(std::make_shared<WalkingTrackData>()),
      dir_walking_handle_(std::make_shared<DirWalkingHandle>()),
      recursive_(recursive)
{
    walking_track_data_->second.push_back(dir_path);

    GetNext();
}

void FileIterator::GetNext()
{
    while (!walking_track_data_->second.empty() || *dir_walking_handle_) {
        if (!*dir_walking_handle_) {
            // Pick a directory entry from pending paths.
            walking_track_data_->first = walking_track_data_->second.back();
            walking_track_data_->second.pop_back();

            const auto& dir = walking_track_data_->first;

            dir_walking_handle_->reset(opendir(dir.value().c_str()));

            if (!*dir_walking_handle_) {
                continue;
            }
        }

        dirent* entry = nullptr;
        int rv = readdir_r(dir_walking_handle_->get(), &dir_entry_buf_, &entry);
        if (rv != 0 || !entry) {
            dir_walking_handle_->reset();
            continue;
        }

        if (ShouldSkip(entry->d_name)) {
            continue;
        }

        auto entry_path = walking_track_data_->first.AppendWith(entry->d_name);

        struct stat entry_stat;
        rv = lstat(entry_path.value().c_str(), &entry_stat);
        if (rv < 0) {
            // Clear stat info on failure.
            memset(&entry_stat, 0, sizeof(entry_stat));
        }

        if (recursive_ && S_ISDIR(entry_stat.st_mode)) {
            walking_track_data_->second.push_back(entry_path);
        }

        // Update the entry info.
       cur_entry_ = FileInfo(entry_path,
                             entry_stat.st_size,
                             S_ISDIR(entry_stat.st_mode),
                             FileTime(entry_stat.st_ctime),
                             FileTime(entry_stat.st_mtime),
                             FileTime(entry_stat.st_atime));

        return;
    }
}

bool FileIterator::ReachedEnd() const noexcept
{
    return !*dir_walking_handle_;
}

bool FileIterator::Equal(const FileIterator& other) const noexcept
{
    return dir_walking_handle_ != nullptr && other.dir_walking_handle_!= nullptr ?
            dir_walking_handle_->get() == other.dir_walking_handle_->get() :
            dir_walking_handle_ == nullptr && other.dir_walking_handle_ == nullptr;
}

}   // namespace kbase
