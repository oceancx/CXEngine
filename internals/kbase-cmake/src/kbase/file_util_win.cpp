/*
 @ 0xCCCCCCCC
*/

#include "kbase/file_util.h"

#include <Windows.h>

#include "kbase/error_exception_util.h"
#include "kbase/file_iterator.h"
#include "kbase/logging.h"

namespace kbase {

Path MakeAbsolutePath(const Path& path)
{
    constexpr int kMaxPath = MAX_PATH;
    wchar_t buffer[kMaxPath];
    if (!_wfullpath(buffer, path.value().c_str(), kMaxPath)) {
        return Path();
    }

    return Path(buffer);
}

bool PathExists(const Path& path)
{
    return GetFileAttributesW(path.value().c_str()) != INVALID_FILE_ATTRIBUTES;
}

bool DirectoryExists(const Path& path)
{
    DWORD attr = GetFileAttributesW(path.value().c_str());
    return attr != INVALID_FILE_ATTRIBUTES && (attr & FILE_ATTRIBUTE_DIRECTORY);
}

bool MakeDirectory(const Path& path)
{
    if (path.empty()) {
        return false;
    }

    DWORD attr = GetFileAttributesW(path.value().c_str());
    if (attr != INVALID_FILE_ATTRIBUTES) {
        if (attr & FILE_ATTRIBUTE_DIRECTORY) {
            return true;
        }

        DLOG(WARNING) << "Creating directory on " << path.AsUTF8()
                      << " conflicts with existing file!";
        return false;
    }

    if (!MakeDirectory(path.parent_path())) {
        return false;
    }

    if (!CreateDirectoryW(path.value().c_str(), nullptr)) {
        LastError error;
        if (error.error_code() != ERROR_ALREADY_EXISTS) {
            DLOG(WARNING) << "Failed to create directory " << path.AsUTF8() << "; " << error;
            return false;
        }
    }

    return true;
}

bool GetFileInfo(const Path& path, FileInfo& info)
{
    WIN32_FILE_ATTRIBUTE_DATA attr_data;
    if (!GetFileAttributesExW(path.value().c_str(), GetFileExInfoStandard, &attr_data)) {
        return false;
    }

    ULARGE_INTEGER file_size;
    file_size.HighPart = attr_data.nFileSizeHigh;
    file_size.LowPart = attr_data.nFileSizeLow;

    info =  FileInfo(path,
                     static_cast<int64_t>(file_size.QuadPart),
                     !!(attr_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY),
                     FileTime(attr_data.ftCreationTime),
                     FileTime(attr_data.ftLastWriteTime),
                     FileTime(attr_data.ftLastAccessTime));

    return true;
}

bool RemoveFile(const Path& path, bool recursive)
{
    if (!recursive) {
        BOOL rv = FALSE;
        if (DirectoryExists(path)) {
            rv = RemoveDirectoryW(path.value().c_str());
        } else {
            rv = DeleteFileW(path.value().c_str());
        }

        if (!rv) {
            DLOG(WARNING) << "Failed to remove file at " << path.AsUTF8() << "; " << LastError();
            return false;
        }

        return true;
    }

    // SHFileOperationW requires that path must end with double null-terminators.
    // Moreover, if the path passed to SHFileOperationW is not a full path, the
    // invocation of SHFileOperationW is not thread safe.
    // Both wcscpy_s and wcsncpy_s fill buffer after the first null-terminator with
    // dirty characters.

    wchar_t path_with_double_null[MAX_PATH + 1] {0};
    Path full_path = path.IsAbsolute() ? path : MakeAbsolutePath(path);
    std::copy_n(full_path.value().begin(), full_path.value().size(), path_with_double_null);

    SHFILEOPSTRUCT file_op;
    memset(&file_op, 0, sizeof(file_op));
    file_op.fFlags = FOF_NO_UI;
    file_op.wFunc = FO_DELETE;
    file_op.pFrom = path_with_double_null;

    int rv = SHFileOperationW(&file_op);
    if (rv) {
        DLOG(WARNING) << "Call SHFileOperation failed: " << rv;
        return false;
    }

    return true;
}

bool RemoveFileAfterReboot(const Path& path)
{
    BOOL rv = MoveFileExW(path.value().c_str(), nullptr, MOVEFILE_DELAY_UNTIL_REBOOT);
    if (!rv) {
        DLOG(WARNING) << "Call MoveFileEx() with MOVEFILE_DELAY_UNTIL_REBOOT failed! "
                      << LastError();
        return false;
    }

    return true;
}

bool DuplicateFile(const Path& src, const Path& dest)
{
    BOOL rv = CopyFileW(src.value().c_str(), dest.value().c_str(), false);
    if (!rv) {
        DLOG(WARNING) << "Call CopyFile failed on " << src.AsUTF8() << " -> " << dest.AsUTF8()
                      << "; " << LastError();
        return false;
    }

    return true;
}

bool DuplicateDirectory(const Path& src, const Path& dest, bool recursive)
{
    if (!DirectoryExists(src)) {
        return false;
    }

    Path full_src = src.IsAbsolute() ? src : MakeAbsolutePath(src);
    Path full_dest = dest.IsAbsolute() ? dest : MakeAbsolutePath(dest);

    // The destination cannot be a subfolder of the source in recursive mode.
    ENSURE(CHECK, !(recursive && full_src.IsParent(full_dest)))
          (full_src.value())(full_dest.value()).Require();

    // Treats this condition as succeeded.
    if (full_src == full_dest) {
        return true;
    }

    if (!DirectoryExists(full_dest)) {
        if (!CreateDirectoryW(full_dest.value().c_str(), nullptr)) {
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
                !CreateDirectoryW(dest_entry_path.value().c_str(), nullptr)) {
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
    if (MoveFileExW(src.value().c_str(), dest.value().c_str(),
                    MOVEFILE_COPY_ALLOWED | MOVEFILE_REPLACE_EXISTING)) {
        return true;
    }

    bool success = false;
    // Only empty directory can be moved across volume.
    // We here use an explicit copy-and-remove strategy.
    if (DuplicateDirectory(src, dest, true) && RemoveFile(src, true)) {
        success = true;
    }

    return success;
}

}   // namespace kbase
