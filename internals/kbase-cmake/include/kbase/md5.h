/*
 @ 0xCCCCCCCC
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef KBASE_MD5_H_
#define KBASE_MD5_H_

#include <array>

#include "kbase/string_view.h"

namespace kbase {

// The underlying implementation of the functions that perform MD5 operations, is
// the openssl-compatible version, and is in the public domain.

// Any 32-bit or wider unsigned integer data type will do
using MD5uint = uint32_t;
using MD5byte = uint8_t;

using MD5Digest = std::array<MD5byte, 16>;

struct MD5Context {
    MD5uint lo, hi;
    MD5uint a, b, c, d;
    MD5byte buffer[64];
    MD5uint block[16];
};

// The following three functions are all together used to incrementally calculate
// the MD5 checksum of a bunch of data.

void MD5Init(MD5Context& context);

void MD5Update(MD5Context& context, const void* data, size_t size);

void MD5Final(MD5Context& context, MD5Digest& digest);

// Calculates the MD5 checksum of a given data.
void MD5Sum(const void* data, size_t size, MD5Digest& digest);

// Converts a digest into a hexadecimal representation.
std::string MD5DigestToString(const MD5Digest& digest);

// Returns the MD5 checksum, in hexadecimal, of the `str`.
std::string MD5String(StringView str);

}   // namespace kbase

#endif  // KBASE_MD5_H_
