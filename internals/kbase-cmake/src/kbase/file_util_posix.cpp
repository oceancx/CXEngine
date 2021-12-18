/*
 @ 0xCCCCCCCC
*/

#include "kbase/file_util.h"

#include <array>
#include <stack>

#include <limits.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

#include "kbase/error_exception_util.h"
#include "kbase/file_iterator.h"
#include "kbase/logging.h"
#include "kbase/scoped_handle.h"

namespace kbase {

Path MakeAbsolutePath(const Path& path)
{
    constexpr int kBufSize = PATH_MAX + 1;
    char buf[kBufSize] {0};

    auto full_path = realpath(path.value().c_str(), buf);

    if (!full_path) {
        DLOG(WARNING) << "realpath() failed for " << path.AsUTF8() << "; " << errno;
        return Path();
    }

    return Path(full_path);
}

bool PathExists(const Path& path)
{
    return access(path.value().c_str(), F_OK) == 0;
}

bool DirectoryExists(const Path& path)
{
    struct stat file_info;
    if (stat(path.value().c_str(), &file_info) != 0) {
        return false;
    }

    return S_ISDIR(file_info.st_mode);
}

bool MakeDirectory(const Path& path)
{
    if (path.empty()) {
        return false;
    }

    struct stat info;
    if (stat(path.value().c_str(), &info) == 0) {
        if (S_ISDIR(info.st_mode)) {
            return true;
        }

        DLOG(WARNING) << "Creating directory on " << path.AsUTF8()
                      << " conflicts with existing file!";
        return false;
    }

    if (!MakeDirectory(path.parent_path())) {
        return false;
    }

    if (mkdir(path.value().c_str(), 0700) != 0) {
        auto error = errno;
        if (!DirectoryExists(path)) {
            DLOG(WARNING) << "Failed to create directory " << path.AsUTF8() << "; " << error;
            return false;
        }
    }

    return true;
}

bool GetFileInfo(const Path& path, FileInfo& info)
{
    struct stat file_info;

    if (stat(path.value().c_str(), &file_info) != 0) {
        return false;
    }

    info = FileInfo(path,
                    file_info.st_size,
                    S_ISDIR(file_info.st_mode),
                    FileTime(file_info.st_ctime),
                    FileTime(file_info.st_mtime),
                    FileTime(file_info.st_atime));

    return true;
}

bool RemoveFile(const Path& path, bool recursive)
{
    struct stat file_info;

    if (stat(path.value().c_str(), &file_info) != 0) {
        return false;
    }

    if (!S_ISDIR(file_info.st_mode)) {
        return (unlink(path.value().c_str()) == 0);
    }

    if(!recursive) {
        return (rmdir(path.value().c_str()) == 0);
    }

    std::stack<Path> directories({path});

    for (const auto& entry : FileIterator(path, true)) {
        if (entry.is_directory()) {
            directories.push(entry.file_path());
        } else {
            if (unlink(entry.file_path().value().c_str()) != 0) {
                return false;
            }
        }
    }

    while (!directories.empty()) {
        auto dir_path = directories.top();
        directories.pop();
        if (rmdir(dir_path.value().c_str()) != 0) {
            return false;
        }
    }

    return true;
}

bool DuplicateFile(const Path& src, const Path& dest)
{
    ScopedFILEHandle src_file(fopen(src.value().c_str(), "r"));
    if (!src_file) {
        auto error = errno;
        DLOG(WARNING) << "Failed to open file to read on " << src.AsUTF8() << "; " << error;
        return false;
    }

    ScopedFILEHandle dest_file(fopen(dest.value().c_str(), "w"));
    if (!dest_file) {
        auto error = errno;
        DLOG(WARNING) << "Failed to open file to write on " << dest.AsUTF8() << "; " << error;
        return false;
    }

    constexpr size_t kBufSize = 32 * 1024;
    std::array<char, kBufSize> buf;
    bool success = true;
    do {
        auto size_read = fread(buf.data(), sizeof(decltype(buf)::value_type), buf.size(),
                               src_file.get());
        if (size_read < buf.size() && ferror(src_file.get())) {
            success = false;
            break;
        }

        auto size_written = fwrite(buf.data(), sizeof(decltype(buf)::value_type),
                                   size_read, dest_file.get());
        if (size_written != size_read) {
            success = false;
            break;
        }
    } while (!feof(src_file.get()));

    return success;
}

bool DuplicateDirectory(const Path& src, const Path& dest, bool recursive)
{
    if (!DirectoryExists(src)) {
        return false;
    }

    Path full_src = src.IsAbsolute() ? src : MakeAbsolutePath(src);

    // It is possible that `dest` doesn't exist, but we require its parent must exist.
    Path full_dest = dest;
    if (!full_dest.IsAbsolute()) {
        auto full_dest_parent = MakeAbsolutePath(dest.parent_path());
        if (full_dest_parent.empty()) {
            return false;
        }

        full_dest = full_dest_parent.AppendWith(dest.filename());
    }

    // The destination cannot be a subfolder of the source in recursive mode.
    ENSURE(CHECK, !(recursive && full_src.IsParent(full_dest)))
          (full_src.value())(full_dest.value()).Require();

    // Treats this condition as succeeded.
    if (full_src == full_dest) {
        return true;
    }

    if (!DirectoryExists(full_dest)) {
        if (mkdir(full_dest.value().c_str(), 0700) != 0) {
            return false;
        }
    }

    // Copy every files and sub-directories.
    bool success = true;
    for (const auto& src_entry : FileIterator(full_src, recursive)) {
        Path dest_entry_path = full_dest;
        if (!full_src.AppendRelativePath(src_entry.file_path(), &dest_entry_path)) {
            success = false;
            break;
        }

        if (src_entry.is_directory()) {
            if (!DirectoryExists(dest_entry_path) &&
                mkdir(dest_entry_path.value().c_str(), 0700) != 0) {
                success = false;
                break;
            }
        } else {
            if (!DuplicateFile(src_entry.file_path(), dest_entry_path)) {
                success = false;
                break;
            }
        }
    }

    return success;
}

bool MakeFileMove(const Path& src, const Path& dest)
{
    if (rename(src.value().c_str(), dest.value().c_str()) == 0) {
        return true;
    }

    bool success = false;
    if (DuplicateDirectory(src, dest, true) && RemoveFile(src, true)) {
        success = true;
    }

    return success;
}

}   // namespace kbase
