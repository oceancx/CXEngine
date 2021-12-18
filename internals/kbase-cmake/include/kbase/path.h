/*
 @ 0xCCCCCCCC
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef KBASE_PATH_H_
#define KBASE_PATH_H_

#include <string>
#include <vector>

#include "kbase/basic_types.h"
#include "kbase/string_view.h"

namespace kbase {

#if defined(OS_WIN)
#define PATH_LITERAL(x) L##x
#else
#define PATH_LITERAL(x) x
#endif

class Path {
public:
    using value_type = PathChar;
    using string_type = std::basic_string<value_type>;
    using string_view_type = BasicStringView<value_type>;

#if defined(OS_WIN)
    static constexpr value_type kPreferredSeparator = PATH_LITERAL('\\');
#else
    static constexpr value_type kPreferredSeparator = PATH_LITERAL('/');
#endif
    static constexpr value_type kExtensionSeparator = PATH_LITERAL('.');
    static constexpr string_view_type kSeparators = PATH_LITERAL("\\/");
    static constexpr string_view_type kCurrentDir = PATH_LITERAL(".");
    static constexpr string_view_type kParentDir = PATH_LITERAL("..");

    Path() = default;

    explicit Path(string_type path);

    ~Path() = default;

    DEFAULT_COPY(Path);

    DEFAULT_MOVE(Path);

    const string_type& value() const noexcept
    {
        return path_value_;
    }

    bool empty() const  noexcept
    {
        return path_value_.empty();
    }

    void clear()
    {
        path_value_.clear();
    }

    // Checks if `ch` is one of separators.
    static bool IsSeparator(value_type ch);

    // Returns true, if the path ends with a path separator.
    bool EndsWithSeparator() const;

    // Removes redundant trailing separator.
    // Note that, this function will not remove separator in path like: C:\,
    // because the separator here is not redundant.
    Path& StripTrailingSeparators();

    // Converts all directory separators in path to the preferred directory separator.
    Path& MakePreferredSeparator();

    Path& MakePathSeparatorTo(value_type separator);

    // Returns the path to the parent directory.
    // Returns empty path if the path itself is empty or there is only a single
    // element in the path.
    Path parent_path() const;

    // Returns a Path corresponding to the file component of the path. If this
    // object already refers to the root, returns the path directly.
    Path filename() const;

    // Retrieves every components of the path, including the root slash.
    // Example: C:\foo\bar  ->  ["C:", "\\", "foo", "bar"]
    //          /home/kc/demo.txt -> ["/", "home", "kc", "demo.txt"]
    void GetComponents(std::vector<string_type>& components) const;

    // Returns true if it is a absolute path.
    bool IsAbsolute() const;

    // `components` must be a relative path.

    Path& Append(const string_type& components);

    Path& Append(const Path& components);

#if defined(OS_WIN)
    Path& AppendASCII(const std::string& components);
#endif

    Path AppendWith(const string_type& components) const;

    Path AppendWith(const Path& components) const;

#if defined(OS_WIN)
    Path AppendWithASCII(const std::string& components) const;
#endif

    // If current path is parent of the `child`, appends to `path` the relative
    // path to child, and returns true.
    // otherwise, returns false.
    // Example: current path: C:\user\kingsley chen
    //          child path:   C:\user\kingsley chen\app data\test
    //          *path:        C:\user\kingsley chen\documents
    // After the calling of this function, *path becomes
    // C:\user\kingsley chen\documents\app data\test
    bool AppendRelativePath(const Path& child, Path* path) const;

    // Returns true, if the path is the parent of the `child`.
    // Returns false, otherwise.
    // NOTE: This function may make a wrong judgement if paths that contain both '.'
    // '..' are involved.
    bool IsParent(const Path& child) const;

    // Returns true if the path has a component that is '..'.
    bool ReferenceParent() const;

    // Returns the extension of the path if there is any.
    // The extension starts with extension separator.
    // If there are multiple extensions, Windows only recognizes the last one.
    string_type extension() const;

    // Removes the extension of the path if there is any.
    // If there are multiple extensions, only last one will be removed.
    Path& RemoveExtension();

    // Adds extension (can with, or without extension separator) to the file name of the path.
    // If the file name of the path already has an extension, then the `extension` will
    // be the only extension recognized by Windows.
    Path& AddExtension(const string_type& extension);

    // Replaces the extension of the file name with `replacement`.
    // If `replacement` is empty or only contains separator, the extension of the file
    // name is removed.
    // If the file name does not have an extension, then `replacement` is added.
    Path& ReplaceExtension(const string_type& replacement);

#if defined(OS_WIN)

    // Returns the string in native encoding of the path.
    // If the path contains any non-ASCII character, the return value is an empty
    // object.
    std::string AsASCII() const;

    // If the `path_in_ascii` contains any non-ASCII character, the function returns
    // an empty Path.
    static Path FromASCII(const std::string& path_in_ascii);

#endif  // OS_WIN

    std::string AsUTF8() const;

    static Path FromUTF8(const std::string& path_in_utf8);

private:
    string_type path_value_;
};

// If two Path objects only differ in case, they are equal.
bool operator==(const Path& lhs, const Path& rhs);

bool operator!=(const Path& lhs, const Path& rhs);

// Define equivalent criteria for STL containers.
bool operator<(const Path& lhs, const Path& rhs);

}   // namespace kbase

namespace std {

template<>
struct hash<kbase::Path> {
    size_t operator()(const kbase::Path& file_path) const
    {
        return std::hash<kbase::Path::string_type>()(file_path.value());
    }
};

}   // namespace std

#endif  // KBASE_PATH_H_