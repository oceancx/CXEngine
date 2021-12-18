/*
 @ 0xCCCCCCCC
*/

#include "kbase/string_util.h"

#include <algorithm>

#include "kbase/basic_types.h"
#include "kbase/error_exception_util.h"

namespace {

using kbase::BasicStringView;
using kbase::CaseMode;

enum TrimPosition : unsigned int {
    TrimNone = 0,
    TrimLeading = 1 << 0,
    TrimTailing = 1 << 1,
    TrimAll = TrimLeading | TrimTailing
};

template<typename StrT>
void EraseCharsT(StrT& str, BasicStringView<typename StrT::value_type> chars)
{
    auto new_end = std::remove_if(str.begin(), str.end(), [chars](auto ch) {
                                      return std::find(chars.cbegin(), chars.cend(), ch) !=
                                             chars.cend();
                                  });
    str.erase(new_end, str.end());
}

template<typename StrT>
void ReplaceStringT(StrT& str,
                    BasicStringView<typename StrT::value_type> find_with,
                    BasicStringView<typename StrT::value_type> replace_with,
                    typename StrT::size_type pos,
                    bool replace_all)
{
    if (pos == StrT::npos || pos + find_with.length() > str.length()) {
        return;
    }

    auto offset = pos;
    while ((offset = str.find(find_with.data(), offset, find_with.length())) != StrT::npos) {
        str.replace(offset, find_with.length(), replace_with.data(), replace_with.length());
        offset += replace_with.length();
        if (!replace_all) {
            break;
        }
    }
}

template<typename StrT>
void TrimStringT(StrT& str, BasicStringView<typename StrT::value_type> chars, TrimPosition pos)
{
    using size_type = typename StrT::size_type;

    size_type not_matched_first = (pos & TrimPosition::TrimLeading) ?
        str.find_first_not_of(chars.data(), 0, chars.length()) : 0;
    size_type not_matched_last = (pos & TrimPosition::TrimTailing) ?
        str.find_last_not_of(chars.data(), StrT::npos, chars.size()) : str.length() - 1;

    if (str.empty() || not_matched_first == StrT::npos || not_matched_last == StrT::npos) {
        str.clear();
        return;
    }

    str = str.substr(not_matched_first, not_matched_last - not_matched_first + 1);
}

template<typename CharT>
bool ContainsOnlyCharsT(BasicStringView<CharT> str, BasicStringView<CharT> chars)
{
    return str.find_first_not_of(chars.data(), 0, chars.size()) == decltype(str)::npos;
}

// tolower() and toupper() are local sensitive, we might don't want to use them in some
// situations.

template<typename CharT>
CharT ToLowerASCII(CharT ch)
{
    return (ch >= 'A' && ch <= 'Z') ? ch + ('a' - 'A') : ch;
}

template<typename CharT>
CharT ToUpperASCII(CharT ch)
{
    return (ch >= 'a' && ch <= 'z') ? ch - ('a' - 'A') : ch;
}

template<typename CharT>
int ASCIIStringCompareCaseInsensitiveT(BasicStringView<CharT> lhs, BasicStringView<CharT> rhs)
{
    for (size_t i = 0; i < lhs.length() && i < rhs.length(); ++i) {
        auto lower_l = ToLowerASCII(lhs[i]);
        auto lower_r = ToLowerASCII(rhs[i]);

        if (lower_l < lower_r) {
            return -1;
        }

        if (lower_l > lower_r) {
            return 1;
        }
    }

    if (lhs.length() == rhs.length()) {
        return 0;
    }

    return lhs.length() < rhs.length() ? -1 : 1;
}

template<typename CharT>
bool StartsWithT(BasicStringView<CharT> str, BasicStringView<CharT> token, CaseMode mode)
{
    if (str.length() < token.length()) {
        return false;
    }

    bool rv = false;
    switch (mode) {
        case CaseMode::Sensitive:
            rv = str.compare(0, token.length(), token.data()) == 0;
            break;
        case CaseMode::ASCIIInsensitive:
        {
#if defined(_MSC_VER)
            auto str_begin = stdext::make_checked_array_iterator(str.begin(), str.length());
#else
            auto str_begin = str.begin();
#endif
            rv = std::equal(token.begin(), token.end(), str_begin, [](auto lhs, auto rhs) {
                                return ToLowerASCII(lhs) == ToLowerASCII(rhs);
                            });
        }
            break;
        default:
            ENSURE(CHECK, kbase::NotReached())(mode).Require();
    }

    return rv;
}

template<typename CharT>
bool EndsWithT(BasicStringView<CharT> str, BasicStringView<CharT> token, CaseMode mode)
{
    if (str.length() < token.length()) {
        return false;
    }

    bool rv = false;
    auto offset = str.length() - token.length();
    switch (mode) {
        case CaseMode::Sensitive:
            rv = str.compare(offset, token.length(), token.data()) == 0;
            break;
        case CaseMode::ASCIIInsensitive:
        {
#if defined(_MSC_VER)
            auto str_begin = stdext::make_checked_array_iterator(str.begin(), str.length(), offset);
#else
            auto str_begin = std::next(str.begin(), offset);
#endif
            rv = std::equal(token.begin(), token.end(), str_begin, [](auto lhs, auto rhs) {
                                return ToLowerASCII(lhs) == ToLowerASCII(rhs);
                            });
        }
            break;
        default:
            ENSURE(CHECK, kbase::NotReached())(mode).Require();
    }

    return rv;
}

template<typename strT>
size_t SplitStringT(BasicStringView<typename strT::value_type> str,
                    BasicStringView<typename strT::value_type> delimiters,
                    std::vector<strT>& tokens)
{
    tokens.clear();

    using size_type = typename strT::size_type;
    size_type begin = 0;
    size_type end = 0;
    while ((begin = str.find_first_not_of(delimiters.data(), begin, delimiters.length()))
           != strT::npos) {
        end = str.find_first_of(delimiters.data(), begin, delimiters.length());
        if (end == strT::npos) {
            end = str.length();
        }

        tokens.push_back(str.substr(begin, end - begin).ToString());
        begin = end + 1;
    }

    return tokens.size();
}

template<typename strT>
strT JoinStringT(const std::vector<strT>& tokens, BasicStringView<typename strT::value_type> sep)
{
    if (tokens.empty()) {
        return strT();
    }

    strT str(tokens[0]);
    for (auto it = tokens.cbegin() + 1; it != tokens.cend(); ++it) {
        str.append(sep.data(), sep.length()).append(*it);
    }

    return str;
}

template<typename charT>
bool MatchPatternT(const charT* str, const charT* pat)
{
    bool on_star = false;
    const charT* s;
    const charT* p;

LoopStart:
    for (s = str, p = pat; *s; ++s, ++p) {
        switch (*p) {
            case '?':
                if (*s == '.') goto StarCheck;
                break;
            case '*':
                on_star = true;
                str = s;
                pat = p;
                do {
                    ++pat;
                } while (*pat == '*');
                if (!*pat) return true;
                goto LoopStart;
            default:
                if (*s != *p) goto StarCheck;
                break;
        }
    }

    while (*p == '*') {
        ++p;
    }

    return !*p;

StarCheck:
    if (!on_star) return false;
    ++str;
    goto LoopStart;
}

template<typename T>
struct ToUnsigned {
    using Unsigned = T;
};

template<>
struct ToUnsigned<char> {
    using Unsigned = unsigned char;
};

template<>
struct ToUnsigned<signed char> {
    using Unsigned = unsigned char;
};

template<>
struct ToUnsigned<wchar_t> {
    using Unsigned = unsigned short;
};

template<typename StringViewT>
bool StringASCIIOnlyCheck(StringViewT str)
{
    for (size_t i = 0; i < str.length(); ++i) {
        typename ToUnsigned<typename StringViewT::value_type>::Unsigned c = str[i];
        if (c > 0x7F) {
            return false;
        }
    }

    return true;
}

}   // namespace

namespace kbase {

std::string& EraseChars(std::string& str, StringView chars)
{
    EraseCharsT(str, chars);
    return str;
}

std::wstring& EraseChars(std::wstring& str, WStringView chars)
{
    EraseCharsT(str, chars);
    return str;
}

std::string RemoveChars(const std::string& str, StringView chars)
{
    std::string new_str(str);
    EraseChars(new_str, chars);
    return new_str;
}

std::wstring RemoveChars(const std::wstring& str, WStringView chars)
{
    std::wstring new_str(str);
    EraseChars(new_str, chars);
    return new_str;
}

std::string& ReplaceString(std::string& str,
                           StringView find_with,
                           StringView replace_with,
                           std::string::size_type pos,
                           bool replace_all)
{
    ReplaceStringT(str, find_with, replace_with, pos, replace_all);
    return str;
}

std::wstring& ReplaceString(std::wstring& str,
                            WStringView find_with,
                            WStringView replace_with,
                            std::wstring::size_type pos,
                            bool replace_all)
{
    ReplaceStringT(str, find_with, replace_with, pos, replace_all);
    return str;
}

std::string& TrimString(std::string& str, StringView chars)
{
    TrimStringT(str, chars, TrimPosition::TrimAll);
    return str;
}

std::wstring& TrimString(std::wstring& str, WStringView chars)
{
    TrimStringT(str, chars, TrimPosition::TrimAll);
    return str;
}

std::string& TrimLeadingString(std::string& str, StringView chars)
{
    TrimStringT(str, chars, TrimPosition::TrimLeading);
    return str;
}

std::wstring& TrimLeadingString(std::wstring& str, WStringView chars)
{
    TrimStringT(str, chars, TrimPosition::TrimLeading);
    return str;
}

std::string& TrimTailingString(std::string& str, StringView chars)
{
    TrimStringT(str, chars, TrimPosition::TrimTailing);
    return str;
}

std::wstring& TrimTailingString(std::wstring& str, WStringView chars)
{
    TrimStringT(str, chars, TrimPosition::TrimTailing);
    return str;
}

bool ContainsOnlyChars(StringView str, StringView chars)
{
    return ContainsOnlyCharsT(str, chars);
}

bool ContainsOnlyChars(WStringView str, WStringView chars)
{
    return ContainsOnlyCharsT(str, chars);
}

std::string& ASCIIStringToLower(std::string& str)
{
    std::transform(str.begin(), str.end(), str.begin(), ToLowerASCII<char>);
    return str;
}

std::wstring& ASCIIStringToLower(std::wstring& str)
{
    std::transform(str.begin(), str.end(), str.begin(), ToLowerASCII<wchar_t>);
    return str;
}

std::string& ASCIIStringToUpper(std::string& str)
{
    std::transform(str.begin(), str.end(), str.begin(), ToUpperASCII<char>);
    return str;
}

std::wstring& ASCIIStringToUpper(std::wstring& str)
{
    std::transform(str.begin(), str.end(), str.begin(), ToUpperASCII<wchar_t>);
    return str;
}

int ASCIIStringCompareCaseInsensitive(StringView lhs, StringView rhs)
{
    return ASCIIStringCompareCaseInsensitiveT(lhs, rhs);
}

int ASCIIStringCompareCaseInsensitive(WStringView lhs, WStringView rhs)
{
    return ASCIIStringCompareCaseInsensitiveT(lhs, rhs);
}

bool ASCIIStringEqualCaseInsensitive(StringView lhs, StringView rhs)
{
    return lhs.length() == rhs.length() &&
           ASCIIStringCompareCaseInsensitive(lhs, rhs) == 0;
}

bool ASCIIStringEqualCaseInsensitive(WStringView lhs, WStringView rhs)
{
    return lhs.length() == rhs.length() &&
           ASCIIStringCompareCaseInsensitive(lhs, rhs) == 0;
}

bool StartsWith(StringView str, StringView token, CaseMode mode)
{
    return StartsWithT(str, token, mode);
}

bool StartsWith(WStringView str, WStringView token, CaseMode mode)
{
    return StartsWithT(str, token, mode);
}

bool EndsWith(StringView str, StringView token, CaseMode mode)
{
    return EndsWithT(str, token, mode);
}

bool EndsWith(WStringView str, WStringView token, CaseMode mode)
{
    return EndsWithT(str, token, mode);
}

size_t SplitString(StringView str, StringView delimiters, std::vector<std::string>& tokens)
{
    return SplitStringT(str, delimiters, tokens);
}

size_t SplitString(WStringView str, WStringView delimiters, std::vector<std::wstring>& tokens)
{
    return SplitStringT(str, delimiters, tokens);
}

std::string JoinString(const std::vector<std::string>& tokens, StringView sep)
{
    return JoinStringT(tokens, sep);
}

std::wstring JoinString(const std::vector<std::wstring>& tokens, WStringView sep)
{
    return JoinStringT(tokens, sep);
}

bool MatchPattern(const std::string& str, const std::string& pat)
{
    return MatchPatternT(str.c_str(), pat.c_str());
}

bool MatchPattern(const std::wstring& str, const std::wstring& pat)
{
    return MatchPatternT(str.c_str(), pat.c_str());
}

bool IsStringASCIIOnly(StringView str)
{
    return StringASCIIOnlyCheck(str);
}

bool IsStringASCIIOnly(WStringView str)
{
    return StringASCIIOnlyCheck(str);
}

}   // namespace kbase
