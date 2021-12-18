/*
 @ 0xCCCCCCCC
*/

#include "kbase/path.h"

#include <algorithm>
#include <functional>
#include <stdexcept>

#include "kbase/error_exception_util.h"
#include "kbase/string_util.h"
#include "kbase/string_encoding_conversions.h"

namespace {

using kbase::Path;

using value_type = Path::value_type;
using string_type = Path::string_type;

// If the `path` contains a drive letter specification, returns the position of the
// last character of the specification; otherwise, return npos.
string_type::size_type FindDriveLetter(const string_type& path)
{
    if (path.length() >= 2 && path[1] == PATH_LITERAL(':') &&
        ((path[0] >= PATH_LITERAL('A') && path[0] <= PATH_LITERAL('Z')) ||
         (path[0] >= PATH_LITERAL('a') && path[0] <= PATH_LITERAL('z')))) {
        return 1;
    }

    return string_type::npos;
}

#if defined(OS_WIN)
// Compares two path values in case-insensitive mode, which is default path equality
// mode on Windows.
// Also comparision is not local sensitive.
int ComparePathValue(const string_type& lhs, const string_type& rhs)
{
    int ret = CompareStringOrdinal(lhs.data(), static_cast<int>(lhs.length()),
                                   rhs.data(), static_cast<int>(rhs.length()), TRUE);
    ENSURE(CHECK, ret != 0)(lhs)(rhs).Require();

    return ret - CSTR_EQUAL;
}
#else
int ComparePathValue(const string_type& lhs, const string_type& rhs)
{
    return lhs.compare(rhs);
}
#endif

bool EqualPathValue(const string_type& lhs, const string_type& rhs)
{
    return ComparePathValue(lhs, rhs) == 0;
}

bool IsPathAbsolute(const string_type& path)
{
#if defined(OS_WIN)
    string_type::size_type drive_letter = FindDriveLetter(path);

    // Such as c:\foo or \\foo .etc
    if (drive_letter != string_type::npos) {
        return (drive_letter + 1 < path.length()) && (Path::IsSeparator(path[drive_letter + 1]));
    }

    return (path.length() > 1) && (Path::IsSeparator(path[0]) && Path::IsSeparator(path[1]));
#else
    return (path.length() > 0) && (Path::IsSeparator(path[0]));
#endif
}

string_type::size_type GetExtensionSeparatorPosition(const string_type& path)
{
    // Special cases for which path contains '.' but does not follow with an extension.
    if (path == Path::kCurrentDir || path == Path::kParentDir) {
        return string_type::npos;
    }

    return path.rfind(Path::kExtensionSeparator);
}

bool IsPathSpecialCase(const string_type& path)
{
    return (path == Path::kCurrentDir || path == Path::kParentDir);
}

}   // namespace

namespace kbase {

constexpr Path::string_view_type Path::kSeparators;
constexpr Path::string_view_type Path::kCurrentDir;
constexpr Path::string_view_type Path::kParentDir;

Path::Path(string_type path)
    : path_value_(std::move(path))
{}

// static
bool Path::IsSeparator(value_type ch)
{
    using std::placeholders::_1;
    using std::equal_to;

    return std::any_of(std::begin(kSeparators), std::end(kSeparators),
                       std::bind(equal_to<value_type>(), ch, _1));
}

bool Path::EndsWithSeparator() const
{
    if (empty()) {
        return false;
    }

    return IsSeparator(path_value_.back());
}

Path& Path::StripTrailingSeparators()
{
    // `start` always points to the position one-offset past the leading separator
    size_t start = FindDriveLetter(path_value_) + 2;

    size_t last_stripped = string_type::npos;
    size_t pos = path_value_.length();
    for (; pos > start && IsSeparator(path_value_[pos - 1]); --pos) {
        if (pos != start + 1 ||
            !IsSeparator(path_value_[start - 1]) ||
            last_stripped != string_type::npos) {
            last_stripped = pos;
        } else {
            break;
        }
    }

    path_value_.resize(pos);

    return *this;
}

Path& Path::MakePathSeparatorTo(value_type separator)
{
    auto pos = path_value_.find_first_of(kSeparators.data());
    if (pos != string_type::npos) {
        value_type old_sep = path_value_[pos];
        if (old_sep != separator) {
            std::replace(path_value_.begin() + pos, path_value_.end(), old_sep, separator);
        }
    }

    return *this;
}

Path& Path::MakePreferredSeparator()
{
    return MakePathSeparatorTo(kPreferredSeparator);
}

Path Path::parent_path() const
{
    Path parent(path_value_);

    parent.StripTrailingSeparators();
    auto letter = FindDriveLetter(parent.path_value_);
    auto last_separator = parent.path_value_.find_last_of(kSeparators.data(), string_type::npos,
                                                          kSeparators.length());

    // Note that in special cases, where the current path is a single element,
    // its parent path is an empty path.
    if (last_separator == string_type::npos) {
        // There might be a drive letter in the path, i.e. C:tmp.txt,
        // meaning file tmp.txt in the current directory on drive C.
        if (letter != string_type::npos && parent.path_value_.back() == PATH_LITERAL(':')) {
            parent.path_value_ = PATH_LITERAL("");
        } else {
            parent.path_value_.resize(letter + 1);
        }
    } else if (last_separator == letter + 1) {
        // in root dir
        if (IsSeparator(parent.path_value_.back())) {
            parent.path_value_ = PATH_LITERAL("");
        } else {
            parent.path_value_.resize(letter + 2);
        }
    } else if (last_separator == letter + 2 && IsSeparator(parent.path_value_[letter+1])) {
        // preserves the leading double-separator
        if (IsSeparator(parent.path_value_.back())) {
            parent.path_value_ = PATH_LITERAL("");
        } else {
            parent.path_value_.resize(letter + 3);
        }
    } else {
        parent.path_value_.resize(last_separator);
    }

    return parent;
}

Path Path::filename() const
{
    Path filename(path_value_);
    filename.StripTrailingSeparators();
    auto last_separator = filename.path_value_.find_last_of(kSeparators.data(), string_type::npos,
                                                            kSeparators.length());
    if (last_separator != string_type::npos &&
        last_separator < filename.path_value_.length() - 1) {
        filename.path_value_.erase(0, last_separator + 1);
    }

    // Deal with the case like C:tmp.txt.
    auto letter_pos = FindDriveLetter(filename.path_value_);
    if (letter_pos != string_type::npos &&
        letter_pos + 1 < filename.path_value_.length() &&
        !IsSeparator(filename.path_value_[letter_pos + 1])) {
        filename.path_value_.erase(0, letter_pos + 1);
    }

    return filename;
}

void Path::GetComponents(std::vector<string_type>& components) const
{
    if (path_value_.empty()) {
        return;
    }

    components.clear();
    Path current(path_value_);

    auto AreAllSeparators = [](const string_type& path) -> bool {
        return std::all_of(path.cbegin(), path.cend(), Path::IsSeparator);
    };

    // Extracts components in main path.
    while (!current.parent_path().empty()) {
        Path filename = current.filename();
        if (!AreAllSeparators(filename.value())) {
            components.push_back(filename.value());
        }
        current = current.parent_path();
    }

    // root or a single parent dir mark.
    auto letter_pos = FindDriveLetter(current.value());
    if (!current.empty() &&
        current.value() != kCurrentDir &&
        letter_pos + 1 < current.value().size()) {
        components.push_back(current.value().substr(letter_pos + 1));
    }

    // drive part.
    if (letter_pos != string_type::npos) {
        components.push_back(current.value().substr(0, letter_pos + 1));
    }

    std::reverse(components.begin(), components.end());
}

bool Path::IsAbsolute() const
{
    return IsPathAbsolute(path_value_);
}

Path& Path::Append(const string_type& components)
{
    ENSURE(CHECK, !IsPathAbsolute(components)).Require();

    // If appends to the current dir, just set the path as the components.
    if (path_value_ == kCurrentDir) {
        path_value_ = components;
        return *this;
    }

    StripTrailingSeparators();

    // If the path is empty, that indicates current directory.
    // If the path component is empty, that indicates nothing to append.
    if (!components.empty() && !path_value_.empty()) {
        // Don't append separator, if there is already one.
        if (!IsSeparator(path_value_[path_value_.length()-1])) {
            // Don't append separator, if the path is a drive letter,
            // which is a valid relative path.
            if (FindDriveLetter(path_value_) + 1 != path_value_.length()) {
                path_value_.append(1, kPreferredSeparator);
            }
        }
    }

    path_value_.append(components);

    return *this;
}

Path& Path::Append(const Path& components)
{
    return Append(components.value());
}

#if defined(OS_WIN)
Path& Path::AppendASCII(const std::string& components)
{
    return Append(ASCIIToWide(components));
}
#endif

Path Path::AppendWith(const string_type& components) const
{
    Path new_path(*this);
    new_path.Append(components);

    return new_path;
}

Path Path::AppendWith(const Path& components) const
{
    return AppendWith(components.value());
}

#if defined(OS_WIN)
Path Path::AppendWithASCII(const std::string& components) const
{
    Path new_path(*this);
    new_path.AppendASCII(components);

    return new_path;
}
#endif

bool Path::AppendRelativePath(const Path& child, Path* path) const
{
    std::vector<string_type> current_components;
    std::vector<string_type> child_components;
    GetComponents(current_components);
    child.GetComponents(child_components);

    if (current_components.empty() ||
        current_components.size() >= child_components.size()) {
        return false;
    }

    auto current_iter = current_components.cbegin();
    auto child_iter = child_components.cbegin();
    for (; current_iter != current_components.cend(); ++current_iter, ++child_iter) {
        if (!EqualPathValue(*current_iter, *child_iter)) {
            return false;
        }
    }

    if (path) {
        for (; child_iter != child_components.cend(); ++child_iter) {
            path->Append(*child_iter);
        }
    }

    return true;
}

bool Path::IsParent(const Path& child) const
{
    return AppendRelativePath(child, nullptr);
}

string_type Path::extension() const
{
    Path base(filename());
    auto separator_pos = GetExtensionSeparatorPosition(base.path_value_);
    if (separator_pos == string_type::npos) {
        return string_type();
    }

    return base.path_value_.substr(separator_pos);
}

Path& Path::RemoveExtension()
{
    if (!extension().empty()) {
        auto separator_pos = GetExtensionSeparatorPosition(path_value_);
        if (separator_pos != string_type::npos) {
            path_value_.erase(separator_pos);
        }
    }

    return *this;
}

Path& Path::AddExtension(const string_type& extension)
{
    if (extension.empty() || extension == string_type(1, kExtensionSeparator)) {
        return *this;
    }

    if (IsPathSpecialCase(filename().value())) {
        path_value_ += kPreferredSeparator;
    }

    // If neither the path nor the extension contains the extension separator, adds
    // one manually.
    if (path_value_.back() != kExtensionSeparator &&
        extension.front() != kExtensionSeparator) {
        path_value_ += kExtensionSeparator;
    }

    path_value_.append(extension);

    return *this;
}

Path& Path::ReplaceExtension(const string_type& extension)
{
    RemoveExtension();

    if (extension.empty() || extension == string_type(1, kExtensionSeparator)) {
        return *this;
    }

    if (IsPathSpecialCase(filename().value())) {
        path_value_ += kPreferredSeparator;
    }

    if (extension[0] != kExtensionSeparator) {
        path_value_ += kExtensionSeparator;
    }

    path_value_.append(extension);

    return *this;
}

bool Path::ReferenceParent() const
{
    std::vector<string_type> components;
    GetComponents(components);

    // It seems redundant spaces at the tail of '..' can be ignored by Windows.
    for (const string_type& component : components) {
        string_type trimed_component = component;
        kbase::TrimTailingString(trimed_component, PATH_LITERAL(" "));
        if (trimed_component == kParentDir) {
            return true;
        }
    }

    return false;
}

#if defined(OS_WIN)

std::string Path::AsASCII() const
{
    if (!kbase::IsStringASCIIOnly(path_value_)) {
        ENSURE(CHECK, NotReached())(path_value_).Require();
        return std::string();
    }

    return kbase::WideToASCII(path_value_);
}

// static
Path Path::FromASCII(const std::string& path_in_ascii)
{
    if (!kbase::IsStringASCIIOnly(path_in_ascii)) {
        ENSURE(CHECK, NotReached())(path_in_ascii).Require();
        return Path();
    }

    return Path(kbase::ASCIIToWide(path_in_ascii));
}

#endif  // OS_WIN

std::string Path::AsUTF8() const
{
#if defined(OS_WIN)
    return kbase::WideToUTF8(path_value_);
#else
    return value();
#endif
}

// static
Path Path::FromUTF8(const std::string& path_in_utf8)
{
#if defined(OS_WIN)
    return Path(kbase::UTF8ToWide(path_in_utf8));
#else
    return Path(path_in_utf8);
#endif
}

bool operator==(const Path& lhs, const Path& rhs)
{
    return EqualPathValue(lhs.value(), rhs.value());
}

bool operator!=(const Path& lhs, const Path& rhs)
{
    return !EqualPathValue(lhs.value(), rhs.value());
}

bool operator<(const Path& lhs, const Path& rhs)
{
    return (ComparePathValue(lhs.value(), rhs.value()) < 0);
}

}   // namespace kbase