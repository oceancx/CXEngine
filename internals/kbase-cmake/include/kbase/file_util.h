/*
 @ 0xCCCCCCCC
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef KBASE_FILE_UTIL_H_
#define KBASE_FILE_UTIL_H_

#include <string>

#include "kbase/file_info.h"
#include "kbase/path.h"

namespace kbase {

// Returns an absolute or full path of the relative `path`.
// Returns an empty path on error.
// On POSIX, this function fails if the path does not exist.
Path MakeAbsolutePath(const Path& path);

// Returns true if the path exists.
// Returns false otherwise.
bool PathExists(const Path& path);

// Returns true if the path exists and is a directory.
// Returns false, otherwise.
bool DirectoryExists(const Path& path);

// Returns true if the directory is empty.
// Returns false otherwise.
bool IsDirectoryEmpty(const Path& path);

// Creates a directory, as well as creating any parent directories, if they don't exist.
// Returns true on successful creation, or if the directory already exists.
bool MakeDirectory(const Path& path);

// Retrieves the information of a given file or directory.
// Be wary of that the size-field of the `FileInfo` is valid if and only if the `path`
// indicates a file.
bool GetFileInfo(const Path& path, FileInfo& info);

// Removes a file or a directory indicated by the given `path`.
// If want to remove a directory non-recursively, the directory must be empty.
bool RemoveFile(const Path& path, bool recursive);

#if defined(OS_WIN)

// If the `path` refers to a directory, the system removes the directory only if the
// directory is empty.
// This function only marks the file or directory as should-be-deleted, it doesn't
// guarantee the deletion will be enforced.
bool RemoveFileAfterReboot(const Path& path);

#endif

// Copies a single file from `src` to `dest`.
// If a file specified by `dest` already exists, it will be overwritten.
// The file copied retains file attribute from the source.
bool DuplicateFile(const Path& src, const Path& dest);

// Copies all files from `src` to `dest` if `recursive` is false.
// Copies all content, including subfolders from `src` to `dest` if `recursive` is true.
// Always overwrites files that already exist.
bool DuplicateDirectory(const Path& src, const Path& dest, bool recursive);

// Moves a file or a directory along with its subfolders from `src` to `dest`.
// Always overwrites any that already exists.
// If paths are on different volumes, this function will attempt to copy and delete.
bool MakeFileMove(const Path& src, const Path& dest);

// Reads contents of whole file at `path` into a string.
// Data is read in binary mode, therefore no CRLF conversion involved.
// If failed to read from file, the string is empty.

std::string ReadFileToString(const Path& path);

void ReadFileToString(const Path& path, std::string& data);

enum class OpenMode {
    Binary,
    Text
};

// Writes `data` to a file at `path`.
// By default, the data are written in the binary-mode.
// If failed to create/open the file to write, the function does nothing.
void WriteStringToFile(const Path& path,
                       const std::string& data,
                       OpenMode mode = OpenMode::Binary);

// Similar to WriteStringToFile, but instead of overwritting the existing contents,
// this function has data appended.
// If failed to create/open the file to write, the function does nothing.
void AppendStringToFile(const Path& path,
                        const std::string& data,
                        OpenMode mode = OpenMode::Binary);

}   // namespace kbase

#endif  // KBASE_FILE_UTIL_H_
