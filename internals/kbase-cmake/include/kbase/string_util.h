/*
 @ 0xCCCCCCCC
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef KBASE_STRING_UTIL_H_
#define KBASE_STRING_UTIL_H_

#include <vector>

#include "kbase/error_exception_util.h"
#include "kbase/string_view.h"

namespace kbase {

// Eliminate any given characters from string `str`.

std::string& EraseChars(std::string& str, StringView chars);
std::wstring& EraseChars(std::wstring& str, WStringView chars);

std::string RemoveChars(const std::string& str, StringView chars);
std::wstring RemoveChars(const std::wstring& str, WStringView chars);

// Replace `find_with` with `replace_with` in `str`.
// `pos` indicates where the search begins. if `pos` equals to `npos` or is greater
// than the length of `str`, these functions do nothing.
// If `relace_all` is not true, then only the first occurrence would be replaced.

std::string& ReplaceString(std::string& str,
                           StringView find_with,
                           StringView replace_with,
                           std::string::size_type pos = 0,
                           bool replace_all = true);
std::wstring& ReplaceString(std::wstring& str,
                            WStringView find_with,
                            WStringView replace_with,
                            std::wstring::size_type pos = 0,
                            bool replace_all = true);

// Eliminate characters in `chars` in a certain range of `str`.

std::string& TrimString(std::string& str, StringView chars);
std::wstring& TrimString(std::wstring& str, WStringView chars);

std::string& TrimLeadingString(std::string& str, StringView chars);
std::wstring& TrimLeadingString(std::wstring& str, WStringView chars);

std::string& TrimTailingString(std::string& str, StringView chars);
std::wstring& TrimTailingString(std::wstring& str, WStringView chars);

// Return true, if the `str` is empty or contains only characters in `chars`;
// Return false, otherwise.

bool ContainsOnlyChars(StringView str, StringView chars);
bool ContainsOnlyChars(WStringView str, WStringView chars);

// Toggle the string to it's ASCII-lowercase or ASCII-uppercase equivalent.

std::string& ASCIIStringToLower(std::string& str);
std::wstring& ASCIIStringToLower(std::wstring& str);

std::string& ASCIIStringToUpper(std::string& str);
std::wstring& ASCIIStringToUpper(std::wstring& str);

// Compares two strings for case-insensitive ASCII characters only.
// And results are
//  < 0 (lhs < rhs)
//  = 0 (lhs == rhs)
//  > 0 (lhs > rhs)

int ASCIIStringCompareCaseInsensitive(StringView lhs, StringView rhs);
int ASCIIStringCompareCaseInsensitive(WStringView lhs, WStringView rhs);

bool ASCIIStringEqualCaseInsensitive(StringView lhs, StringView rhs);
bool ASCIIStringEqualCaseInsensitive(WStringView lhs, WStringView rhs);

enum class CaseMode {
    Sensitive,
    ASCIIInsensitive
};

// Return true, if `str` starts with `token`.
// Return false, otherwise.

bool StartsWith(StringView str,
                StringView token,
                CaseMode mode = CaseMode::Sensitive);
bool StartsWith(WStringView str,
                WStringView token,
                CaseMode mode = CaseMode::Sensitive);

// Return true, if `str` ends with `token`.
// Return false, otherwise.

bool EndsWith(StringView str,
              StringView token,
              CaseMode mode = CaseMode::Sensitive);
bool EndsWith(WStringView str,
              WStringView token,
              CaseMode mode = CaseMode::Sensitive);

// Set up enough memory in `str` to accomodate a c-style string with length
// of `length_including_null`. Be wary of that real size of the string data
// does not count the null-terminate character. This function is useful when
// interaction between a string object and a legacy API is required.
// Returns pointer to the underlying data of the string object.
template<typename strT>
typename strT::value_type* WriteInto(strT& str, size_t length_including_null)
{
    // If the length is equal to 1, the underlying string size is 0,
    // operator[] may then result in an undefined behavior in this situation.
    ENSURE(CHECK, length_including_null > 1U).Require();
    str.reserve(length_including_null);
    str.resize(length_including_null - 1);

    return &str[0];
}

// Split a string, delimieted by any of the characters in `delimiters`, into fields.
// Fields are added into `tokens`.
// Returns the number of tokens found.

size_t SplitString(StringView str, StringView delimiters, std::vector<std::string>& tokens);
size_t SplitString(WStringView str, WStringView delimiters, std::vector<std::wstring>& tokens);

// Combines string parts in `tokens` by using `sep` as the separator.
// Returns combined string.

std::string JoinString(const std::vector<std::string>& tokens, StringView sep);
std::wstring JoinString(const std::vector<std::wstring>& tokens, WStringView sep);

// Pattern matching algorithm, also supports wildcards, in case-sensitive mode.
// metacharacter `?` matches exactly one character unless the character is a `.`
// metacharacter `*` matches any sequence of zero or more characters.
bool MatchPattern(const std::string& str, const std::string& pat);
bool MatchPattern(const std::wstring& str, const std::wstring& pat);

// Determines if all characters in string are defined in ASCII code page.
bool IsStringASCIIOnly(StringView str);
bool IsStringASCIIOnly(WStringView str);

}   // namespace kbase

#endif  // KBASE_STRING_UTIL_H_
