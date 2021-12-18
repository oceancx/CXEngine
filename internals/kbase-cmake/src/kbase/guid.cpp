/*
 @ 0xCCCCCCCC
*/

#include "kbase/guid.h"

#include <array>
#include <random>

#include "kbase/string_format.h"

namespace {

void GenerateRawGUIDBytes(std::array<uint64_t, 2>& raw_bytes)
{
    std::random_device rd;
    std::mt19937 engine(rd());
    std::uniform_int_distribution<std::uint64_t> dist;
    raw_bytes[0] = dist(engine);
    raw_bytes[1] = dist(engine);
}

bool IsLowerHexDigit(char c)
{
    return ('a' <= c && c <= 'f') ||
           ('0' <= c && c <= '9');
}

bool IsHexDigit(char c)
{
    return ('A' <= c && c <= 'F') ||
           ('a' <= c && c <= 'f') ||
           ('0' <= c && c <= '9');
}

}   // namespace

namespace kbase {

std::string GenerateGUID()
{
    std::array<uint64_t, 2> raw_guid_bytes;

    GenerateRawGUIDBytes(raw_guid_bytes);

    // Set the version bits to 4.
    raw_guid_bytes[0] &= 0xFFFFFFFFFFFF0FFFULL;
    raw_guid_bytes[0] |= 0x0000000000004000ULL;

    // Se the bits 6 and 7 of the clock_seq_hi_and_reserved to zero and one, respectively.
    raw_guid_bytes[1] &= 0x3FFFFFFFFFFFFFFFULL;
    raw_guid_bytes[1] |= 0x8000000000000000ULL;

    return StringPrintf("%08x-%04x-%04x-%04x-%012llx",
                        static_cast<unsigned int>(raw_guid_bytes[0] >> 32),
                        static_cast<unsigned int>((raw_guid_bytes[0] >> 16) & 0x0000FFFF),
                        static_cast<unsigned int>(raw_guid_bytes[0] & 0x0000FFFF),
                        static_cast<unsigned int>(raw_guid_bytes[1] >> 48),
                        raw_guid_bytes[1] & 0x0000FFFFFFFFFFFFULL);
}

bool IsGUIDValid(StringView guid, bool strict_mode)
{
    constexpr size_t kGUIDLength = 36;
    if (guid.length() != kGUIDLength) {
        return false;
    }

    for (size_t i = 0; i < guid.length(); ++i) {
        auto cur_ch = guid[i];
        if (cur_ch == '-') {
            if (i == 8 || i == 13 || i == 18 || i == 23) {
                continue;
            }

            return false;
        }

        if ((strict_mode && !IsLowerHexDigit(cur_ch)) ||
            (!strict_mode && !IsHexDigit(cur_ch))) {
            return false;
        }
    }

    return true;
}

}   // namespace kbase
