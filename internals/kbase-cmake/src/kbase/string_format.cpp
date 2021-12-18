/*
 @ 0xCCCCCCCC
*/

#include "kbase/string_format.h"

#include "kbase/basic_macros.h"
#include "kbase/scope_guard.h"

#if defined(OS_POSIX)
#include <cstdarg>
#endif

namespace {

using kbase::FormatError;
using kbase::NotReached;
using kbase::internal::FormatTraits;
using kbase::internal::Placeholder;
using kbase::internal::PlaceholderList;
using kbase::internal::IsDigit;
using kbase::internal::StrToUL;

enum class FormatParseState {
    InText,
    InFormat
};

constexpr char kEscapeBegin = '{';
constexpr char kEscapeEnd = '}';
constexpr char kSpecifierDelimeter = ':';
constexpr char kPlaceholderMark = '@';

void AppendPrintfT(std::string& str, char* buf, size_t max_count_including_null, const char* fmt,
                   va_list args)
{
    va_list args_copy;
    va_copy(args_copy, args);
    int real_size = vsnprintf(buf, max_count_including_null, fmt, args_copy);
    va_end(args_copy);

    ENSURE(THROW, real_size >= 0)(real_size).Require();
    if (static_cast<size_t>(real_size) < max_count_including_null) {
        // vsnprintf() guarantees the resulting string will be terminated with a null-terminator.
        str.append(buf);
        return;
    }

    std::vector<char> backup_buf(static_cast<size_t>(real_size) + 1);
    va_copy(args_copy, args);
    vsnprintf(backup_buf.data(), backup_buf.size(), fmt, args_copy);
    va_end(args_copy);
    str.append(backup_buf.data());
}

void AppendPrintfT(std::wstring& str, wchar_t* buf, size_t max_count_including_null,
                   const wchar_t* fmt, va_list args)
{
    va_list args_copy;
    va_copy(args_copy, args);
    int rv = vswprintf(buf, max_count_including_null, fmt, args_copy);
    va_end(args_copy);
    if (rv >= 0) {
        str.append(buf, rv);
        return;
    }

    constexpr size_t kMaxAllowed = 16U * 1024 * 1024;
    size_t tentative_count = max_count_including_null;
    std::vector<wchar_t> backup_buf;
    while (true) {
        tentative_count <<= 1;
        ENSURE(THROW, tentative_count <= kMaxAllowed)(tentative_count)(kMaxAllowed).Require();
        backup_buf.resize(tentative_count);
        va_copy(args_copy, args);
        rv = vswprintf(backup_buf.data(), backup_buf.size(), fmt, args_copy);
        va_end(args_copy);
        if (rv > 0) {
            str.append(backup_buf.data(), rv);
            break;
        }
    }
}

template<typename StrT>
void StringAppendPrintfT(StrT& str, const typename StrT::value_type* fmt, va_list args)
{
    using CharT = typename StrT::value_type;

    constexpr size_t kDefaultCount = 1024U;
    CharT buf[kDefaultCount];

    AppendPrintfT(str, buf, kDefaultCount, fmt, args);
}

template<typename CharT>
size_t ExtractPlaceholderIndex(const CharT* first_digit, CharT*& last_digit)
{
    auto index = StrToUL(first_digit, last_digit);
    --last_digit;

    return index;
}

template<typename CharT>
typename FormatTraits<CharT>::String AnalyzeFormatT(const CharT* fmt, PlaceholderList<CharT>& placeholders)
{
    constexpr size_t kInitialCapacity = 32;
    typename FormatTraits<CharT>::String analyzed_fmt;
    analyzed_fmt.reserve(kInitialCapacity);

    placeholders.clear();
    Placeholder<CharT> placeholder;

    auto state = FormatParseState::InText;
    for (auto ptr = fmt; *ptr != '\0'; ++ptr) {
        if (*ptr == kEscapeBegin) {
            // `{` is an invalid token for in-format state.
            ENSURE(THROW, state != FormatParseState::InFormat).ThrowIn<FormatError>().Require();
            if (*(ptr + 1) == kEscapeBegin) {
                // Use `{{` to represent literal `{`.
                analyzed_fmt += kEscapeBegin;
                ++ptr;
            } else if (IsDigit(*(ptr + 1))) {
                CharT* last_digit;
                placeholder.index = ExtractPlaceholderIndex(ptr + 1, last_digit);
                ptr = last_digit;
                ENSURE(THROW, (*(ptr + 1) == kEscapeEnd) ||
                              (*(ptr + 1) == kSpecifierDelimeter)).ThrowIn<FormatError>()
                                                                  .Require();
                if (*(ptr + 1) == kSpecifierDelimeter) {
                    ++ptr;
                }

                // Turn into in-format state.
                state = FormatParseState::InFormat;
            } else {
                ENSURE(THROW, NotReached()).ThrowIn<FormatError>().Require();
            }
        } else if (*ptr == kEscapeEnd) {
            if (state == FormatParseState::InText) {
                ENSURE(THROW, *(ptr + 1) == kEscapeEnd).ThrowIn<FormatError>().Require();
                analyzed_fmt += kEscapeEnd;
                ++ptr;
            } else {
                placeholder.pos = analyzed_fmt.length();
                analyzed_fmt += kPlaceholderMark;
                placeholders.push_back(placeholder);
                placeholder.format_specifier.clear();

                // Now we turn back into in-text state.
                state = FormatParseState::InText;
            }
        } else {
            if (state == FormatParseState::InText) {
                analyzed_fmt += *ptr;
            } else {
                placeholder.format_specifier += *ptr;
            }
        }
    }

    ENSURE(THROW, state == FormatParseState::InText).ThrowIn<FormatError>().Require();

    std::sort(std::begin(placeholders), std::end(placeholders),
              [](const auto& lhs, const auto& rhs) {
                  return lhs.index < rhs.index;
              });

    return analyzed_fmt;
}

} // namespace

namespace kbase {

void StringAppendPrintf(std::string& str, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    ON_SCOPE_EXIT { va_end(args); };

    StringAppendPrintfT(str, fmt, args);
}

void StringAppendPrintf(std::wstring& str, const wchar_t* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    ON_SCOPE_EXIT { va_end(args); };

    StringAppendPrintfT(str, fmt, args);
}

std::string StringPrintf(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    ON_SCOPE_EXIT { va_end(args); };

    std::string str;
    StringAppendPrintfT(str, fmt, args);

    return str;
}

std::wstring StringPrintf(const wchar_t* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    ON_SCOPE_EXIT { va_end(args); };

    std::wstring str;
    StringAppendPrintfT(str, fmt, args);

    return str;
}

void StringPrintf(std::string& str, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    ON_SCOPE_EXIT { va_end(args); };

    str.clear();
    StringAppendPrintfT(str, fmt, args);
}

void StringPrintf(std::wstring& str, const wchar_t* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    ON_SCOPE_EXIT { va_end(args); };

    str.clear();
    StringAppendPrintfT(str, fmt, args);
}

namespace internal {

std::string AnalyzeFormat(const char* fmt, PlaceholderList<char>& placeholders)
{
    return AnalyzeFormatT(fmt, placeholders);
}

std::wstring AnalyzeFormat(const wchar_t* fmt, PlaceholderList<wchar_t>& placeholders)
{
    return AnalyzeFormatT(fmt, placeholders);
}

}   // namespace internal

}   // namespace kbase
