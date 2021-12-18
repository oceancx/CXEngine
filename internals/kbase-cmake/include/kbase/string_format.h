/*
 @ 0xCCCCCCCC
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef KBASE_STRING_FORMAT_H_
#define KBASE_STRING_FORMAT_H_

#include <algorithm>
#include <iomanip>
#include <stdexcept>
#include <string>
#include <vector>

#include "kbase/error_exception_util.h"

namespace kbase {

class FormatError : public std::invalid_argument {
public:
    explicit FormatError(const std::string& what)
        : invalid_argument(what)
    {}

    explicit FormatError(const char* what)
        : invalid_argument(what)
    {}
};

// All Printf-series functions may throw an exception, if the size of the buffer that stores
// the formatted data exceeds the threshold.

// Return a string in given format.

std::string StringPrintf(const char* fmt, ...);
std::wstring StringPrintf(const wchar_t* fmt, ...);

// Modify a string in given format.

void StringPrintf(std::string& str, const char* fmt, ...);
void StringPrintf(std::wstring& str, const wchar_t* fmt, ...);

// Append a string in given format.

void StringAppendPrintf(std::string& str, const char* fmt, ...);
void StringAppendPrintf(std::wstring& str, const wchar_t* fmt, ...);

namespace internal {

template<typename CharT>
struct FormatTraits {
    using String = std::basic_string<CharT>;
    using Stream = std::basic_ostringstream<CharT>;
};

template<typename CharT>
struct Placeholder {
    using String = typename FormatTraits<CharT>::String;

    size_t index = static_cast<size_t>(-1);
    size_t pos = static_cast<size_t>(-1);
    String format_specifier;
    String formatted;

    // For std::equal_range.
    struct RangeCompare {
        bool operator()(size_t self_index, const Placeholder& rhs) const noexcept
        {
            return self_index < rhs.index;
        }

        bool operator()(const Placeholder& lhs, size_t self_index) const noexcept
        {
            return lhs.index < self_index;
        }
    };
};

template<typename CharT>
using PlaceholderList = std::vector<Placeholder<CharT>>;

// Return a simplified/analyzed format string, and store every specifiers into
// `placeholders`.
// Be aware of that elements in `placeholders` are sorted in the ascending order
// of index.

std::string AnalyzeFormat(const char* fmt, PlaceholderList<char>& placeholders);
std::wstring AnalyzeFormat(const wchar_t* fmt, PlaceholderList<wchar_t>& placeholders);

enum class SpecifierCategory {
    None = 0,
    PaddingAlign,
    Sign,
    Width,
    Precision,
    Type
};

inline bool IsDigit(char ch)
{
    return isdigit(ch) != 0;
}

inline bool IsDigit(wchar_t ch)
{
    return iswdigit(ch) != 0;
}

inline unsigned long StrToUL(const char* str, char*& end_ptr)
{
    return strtoul(str, &end_ptr, 10);
}

inline unsigned long StrToUL(const wchar_t* str, wchar_t*& end_ptr)
{
    return wcstoul(str, &end_ptr, 10);
}

inline bool IsTypeSpecifier(char ch)
{
    return ch == 'b' || ch == 'x' || ch == 'X' || ch == 'o' || ch == 'e' || ch == 'E';
}

inline bool IsTypeSpecifier(wchar_t ch)
{
    return ch == L'b' || ch == L'x' || ch == L'X' || ch == L'o' || ch == L'e' || ch == L'E';
}

template<typename CharT>
SpecifierCategory GuessNextSpecCategory(const CharT* spec)
{
    // Maybe we have finished parsing.
    if (*spec == '\0') {
        return SpecifierCategory::None;
    }

    if (*(spec + 1) == '<' || *(spec + 1) == '>') {
        return SpecifierCategory::PaddingAlign;
    }

    if (*spec == '+') {
        return SpecifierCategory::Sign;
    }

    if (IsDigit(*spec)) {
        return SpecifierCategory::Width;
    }

    if (*spec == '.') {
        return SpecifierCategory::Precision;
    }

    if (IsTypeSpecifier(*spec)) {
        return SpecifierCategory::Type;
    }

    ENSURE(THROW, NotReached()).ThrowIn<FormatError>().Require();
    return SpecifierCategory::None;
}

template<typename CharT>
void ApplyPaddingAlignFormat(const CharT* spec,
                             SpecifierCategory last_spec_type,
                             typename FormatTraits<CharT>::Stream& stream,
                             const CharT*& spec_end)
{
    ENSURE(THROW, last_spec_type < SpecifierCategory::PaddingAlign).ThrowIn<FormatError>()
                                                                   .Require();

    typename FormatTraits<CharT>::Stream& os = stream;

    CharT fill_ch = *spec++;
    os << std::setfill(fill_ch);
    if (*spec == '<') {
        os << std::left;
    } else {
        os << std::right;
    }

    spec_end = spec + 1;
}

template<typename CharT>
void ApplySignFormat(const CharT* spec,
                     SpecifierCategory last_spec_type,
                     typename FormatTraits<CharT>::Stream& stream,
                     const CharT*& spec_end)
{
    ENSURE(THROW, last_spec_type < SpecifierCategory::Sign).ThrowIn<FormatError>().Require();

    typename FormatTraits<CharT>::Stream& os = stream;

    os << std::showpos;

    spec_end = spec + 1;
}

template<typename CharT>
void ApplyWidthFormat(const CharT* spec,
                      SpecifierCategory last_spec_type,
                      typename FormatTraits<CharT>::Stream& stream,
                      const CharT*& spec_end)
{
    ENSURE(THROW, last_spec_type < SpecifierCategory::Width).ThrowIn<FormatError>().Require();

    typename FormatTraits<CharT>::Stream& os = stream;

    CharT* digit_end = nullptr;
    auto width = StrToUL(spec, digit_end);
    os << std::setw(width);

    spec_end = digit_end;
}

template<typename CharT>
void ApplyPrecisionFormat(const CharT* spec,
                          SpecifierCategory last_spec_type,
                          typename FormatTraits<CharT>::Stream& stream,
                          const CharT*& spec_end)
{
    ENSURE(THROW, last_spec_type < SpecifierCategory::Precision).ThrowIn<FormatError>().Require();

    typename FormatTraits<CharT>::Stream& os = stream;

    CharT* digit_end = nullptr;
    auto precision_size = StrToUL(spec + 1, digit_end);
    os << std::fixed << std::setprecision(precision_size);

    spec_end = digit_end;
}

template<typename CharT>
void ApplyTypeFormat(const CharT* spec,
                     SpecifierCategory last_spec_type,
                     typename FormatTraits<CharT>::Stream& stream,
                     const CharT*& spec_end)
{
    ENSURE(THROW, last_spec_type < SpecifierCategory::Type).ThrowIn<FormatError>().Require();

    typename FormatTraits<CharT>::Stream& os = stream;

    CharT type_mark = *spec;
    switch (type_mark) {
        case 'b':
            os << std::boolalpha;
            break;

        case 'x':
            os << std::hex;
            break;

        case 'X':
            os << std::hex << std::uppercase;
            break;

        case 'o':
            os << std::oct;
            break;

        case 'e':
            os << std::scientific;
            break;

        case 'E':
            os << std::scientific << std::uppercase;
            break;

        default:
            ENSURE(CHECK, NotReached()).Require();
            break;
    }

    spec_end = spec + 1;
}

template<typename CharT, typename Arg>
void FormatArgWithSpecifier(Arg&& arg,
                            const typename FormatTraits<CharT>::String& specifier,
                            typename FormatTraits<CharT>::Stream& stream,
                            typename FormatTraits<CharT>::String& formatted)
{
    if (specifier.empty()) {
        stream << arg;
        formatted = stream.str();
        return;
    }

    auto spec = specifier.data();
    auto last_spec_type = SpecifierCategory::None;
    auto next_spec_type = SpecifierCategory::None;
    while ((next_spec_type = GuessNextSpecCategory(spec)) != SpecifierCategory::None) {
        switch (next_spec_type) {
            case SpecifierCategory::PaddingAlign:
                ApplyPaddingAlignFormat(spec, last_spec_type, stream, spec);
                last_spec_type = SpecifierCategory::PaddingAlign;
                break;

            case SpecifierCategory::Sign:
                ApplySignFormat(spec, last_spec_type, stream, spec);
                last_spec_type = SpecifierCategory::Sign;
                break;

            case SpecifierCategory::Width:
                ApplyWidthFormat(spec, last_spec_type, stream, spec);
                last_spec_type = SpecifierCategory::Width;
                break;

            case SpecifierCategory::Precision:
                ApplyPrecisionFormat(spec, last_spec_type, stream, spec);
                last_spec_type = SpecifierCategory::Precision;
                break;

            case SpecifierCategory::Type:
                ApplyTypeFormat(spec, last_spec_type, stream, spec);
                last_spec_type = SpecifierCategory::Type;
                break;

            default:
                ENSURE(CHECK, NotReached()).Require();
                break;
        }
    }

    stream << arg;
    formatted = stream.str();
}

template<typename CharT>
typename FormatTraits<CharT>::String StringFormatT(const typename FormatTraits<CharT>::String& fmt,
                                                   PlaceholderList<CharT>& placeholders,
                                                   size_t arg_count)
{
    bool all_args_processed = std::all_of(placeholders.begin(), placeholders.end(),
                                          [arg_count](const auto& placeholder) {
                                              return placeholder.index < arg_count;
                                          });
    ENSURE(THROW, all_args_processed).ThrowIn<FormatError>().Require();

    auto formatted_str(fmt);

    // Sort in the decreasing order of pos, since we are going to expand `formatted_str`
    // from right to left.
    std::sort(placeholders.begin(), placeholders.end(), [](const auto& lhs, const auto& rhs) {
                  return lhs.pos > rhs.pos;
              });

    for (const auto& placeholder : placeholders) {
        formatted_str.replace(placeholder.pos, 1, placeholder.formatted);
    }

    return formatted_str;
}

template<typename CharT, typename Arg, typename... Args>
typename FormatTraits<CharT>::String StringFormatT(const typename FormatTraits<CharT>::String& fmt,
                                                   PlaceholderList<CharT>& placeholders,
                                                   size_t arg_processing_index,
                                                   Arg&& arg,
                                                   Args&&... args)
{
    // One argument may associate with multiple placeholders.
    auto associated_placeholders = std::equal_range(placeholders.begin(),
                                                    placeholders.end(),
                                                    arg_processing_index,
                                                    typename Placeholder<CharT>::RangeCompare());
    typename FormatTraits<CharT>::Stream output;
    auto default_fmt_flags = output.flags();
    for (auto it = associated_placeholders.first; it != associated_placeholders.second; ++it) {
        FormatArgWithSpecifier<CharT, Arg>(std::forward<Arg>(arg),
                                           it->format_specifier,
                                           output,
                                           it->formatted);
        // Reset stream for reuse.
        output.str(typename FormatTraits<CharT>::String());
        output.clear();
        output.flags(default_fmt_flags);
    }

    return StringFormatT(fmt, placeholders, arg_processing_index + 1, std::forward<Args>(args)...);
}

}   // namespace internal

// C#-like string format facility.
// For each format-specifier in `fmt`, it can be in the form of
// {index[:[fill|align]|sign|width|.precision|type]}
// index := the 0-based index number of specifier.
// fill := any single character other than `{` and `}` for filling the padding.
// align := `<` for left-alignemnt with fill character and `>` for right-alignment.
// sign := +, prepend `+` with the positive number.
// width := the width of the field.
// .precision := floating-point precision.
// type := can be one of [b, x, X, o, e, E].
// Specifier marks `fill` and `align` **must** be in presence together.
// Although all of these specifier marks are optional, their relative orders, if any
// present, do matter; otherwise, a StringFormatSpecifierError exception would be raised.
// Also, if a specifier mark has no effect on its corresponding argument, this specifier
// mark is simply ignored, and no exception would be raised.

template<typename... Args>
std::string StringFormat(const char* fmt, Args&&... args)
{
    using namespace kbase::internal;

    PlaceholderList<char> placeholders;
    placeholders.reserve(sizeof...(args));
    auto analyzed_fmt = AnalyzeFormat(fmt, placeholders);

    return StringFormatT(analyzed_fmt, placeholders, 0, std::forward<Args>(args)...);
}

template<typename... Args>
std::wstring StringFormat(const wchar_t* fmt, Args&&... args)
{
    using namespace kbase::internal;

    PlaceholderList<wchar_t> placeholders;
    placeholders.reserve(sizeof...(args));
    auto analyzed_fmt = AnalyzeFormat(fmt, placeholders);

    return StringFormatT(analyzed_fmt, placeholders, 0, std::forward<Args>(args)...);
}

}   // namespace kbase

#endif  // KBASE_STRING_FORMAT_H_
