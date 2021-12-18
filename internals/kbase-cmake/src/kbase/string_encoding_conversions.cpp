/*
 @ 0xCCCCCCCC
*/

#include "kbase/string_encoding_conversions.h"

#include <codecvt>
#include <locale>

#include "kbase/error_exception_util.h"
#include "kbase/string_util.h"

namespace kbase {

std::string WideToUTF8(WStringView wide_str)
{
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t, 0x10ffff, std::little_endian>> converter;
    return converter.to_bytes(wide_str.begin(), wide_str.end());
}

std::wstring UTF8ToWide(StringView utf_str)
{
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t, 0x10ffff, std::little_endian>> converter;
    return converter.from_bytes(utf_str.begin(), utf_str.end());
}

std::wstring ASCIIToWide(StringView ascii_str)
{
    ENSURE(CHECK, IsStringASCIIOnly(ascii_str)).Require();
    return std::wstring(ascii_str.begin(), ascii_str.end());
}

std::string WideToASCII(WStringView wide_str)
{
    ENSURE(CHECK, IsStringASCIIOnly(wide_str)).Require();
    return std::string(wide_str.begin(), wide_str.end());
}

}   // namespace kbase
