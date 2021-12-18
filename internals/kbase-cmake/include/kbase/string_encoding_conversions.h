/*
 @ 0xCCCCCCCC
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef KBASE_STRING_ENCODING_CONVERSIONS_H_
#define KBASE_STRING_ENCODING_CONVERSIONS_H_

#include "kbase/string_view.h"

namespace kbase {

std::string WideToUTF8(WStringView wide_str);
std::wstring UTF8ToWide(StringView utf_str);

// Don't use these functions to convert strings that might contain non-ASCII characters.
std::wstring ASCIIToWide(StringView ascii_str);
std::string WideToASCII(WStringView wide_str);

}   // namespace kbase

#endif  // KBASE_STRING_ENCODING_CONVERSIONS_H_
