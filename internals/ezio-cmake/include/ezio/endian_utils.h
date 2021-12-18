/*
 @ 0xCCCCCCCC
*/

#ifndef EZIO_ENDIAN_UTILS_H_
#define EZIO_ENDIAN_UTILS_H_

#include <cstdint>
#include <cstdlib>

#include "kbase/basic_macros.h"

#if defined(OS_APPLE)
#include "endian_apple.h"
#elif defined(OS_POSIX)
#include <endian.h>
#endif

#ifdef _MSC_VER

#include <stdlib.h>
#define bswap_32(x) _byteswap_ulong(x)
#define bswap_64(x) _byteswap_uint64(x)

#elif defined(__APPLE__)

// Mac OS X / Darwin features
#include <libkern/OSByteOrder.h>
#define bswap_32(x) OSSwapInt32(x)
#define bswap_64(x) OSSwapInt64(x)

#elif defined(__sun) || defined(sun)

#include <sys/byteorder.h>
#define bswap_32(x) BSWAP_32(x)
#define bswap_64(x) BSWAP_64(x)

#elif defined(__FreeBSD__)

#include <sys/endian.h>
#define bswap_32(x) bswap32(x)
#define bswap_64(x) bswap64(x)

#elif defined(__OpenBSD__)

#include <sys/types.h>
#define bswap_32(x) swap32(x)
#define bswap_64(x) swap64(x)

#elif defined(__NetBSD__)

#include <sys/types.h>
#include <machine/bswap.h>
#if defined(__BSWAP_RENAME) && !defined(__bswap_32)
#define bswap_32(x) bswap32(x)
#define bswap_64(x) bswap64(x)
#endif

#else

#include <byteswap.h>

#endif

namespace ezio {

// -*- little endian to big endian -*-

#if defined(OS_POSIX)

inline int16_t HostToNetwork(int16_t n) noexcept
{
    return htobe16(n);
}

inline uint16_t HostToNetwork(uint16_t n) noexcept
{
    return htobe16(n);
}

inline int32_t HostToNetwork(int32_t n) noexcept
{
    return htobe32(n);
}

inline uint32_t HostToNetwork(uint32_t n) noexcept
{
    return htobe32(n);
}

inline int64_t HostToNetwork(int64_t n) noexcept
{
    return htobe64(n);
}

inline uint64_t HostToNetwork(uint64_t n) noexcept
{
    return htobe64(n);
}

inline float HostToNetwork(float f) noexcept
{
	uint32_t n = *reinterpret_cast<uint32_t*>(&f);
	n = bswap_32(n);
	return *reinterpret_cast<float*>(&n);
}

#elif defined(OS_WIN)

inline int16_t HostToNetwork(int16_t n) noexcept
{
    return _byteswap_ushort(n);
}

inline uint16_t HostToNetwork(uint16_t n) noexcept
{
    return _byteswap_ushort(n);
}

inline int32_t HostToNetwork(int32_t n) noexcept
{
    return _byteswap_ulong(n);
}

inline uint32_t HostToNetwork(uint32_t n) noexcept
{
    return _byteswap_ulong(n);
}

inline int64_t HostToNetwork(int64_t n) noexcept
{
    return _byteswap_uint64(n);
}

inline uint64_t HostToNetwork(uint64_t n) noexcept
{
    return _byteswap_uint64(n);
}

inline float HostToNetwork(float f) noexcept
{
	uint32_t n = *reinterpret_cast<uint32_t*>(&f);
	n = _byteswap_ulong(n);
	return *reinterpret_cast<float*>(&n);
}


#endif  // OS_POSIX

// -*- big endian to little endian -*-

#if defined(OS_POSIX)

inline int16_t NetworkToHost(int16_t n) noexcept
{
    return be16toh(n);
}

inline uint16_t NetworkToHost(uint16_t n) noexcept
{
    return be16toh(n);
}

inline int32_t NetworkToHost(int32_t n) noexcept
{
    return be32toh(n);
}

inline uint32_t NetworkToHost(uint32_t n) noexcept
{
    return be32toh(n);
}

inline int64_t NetworkToHost(int64_t n) noexcept
{
    return be64toh(n);
}

inline uint64_t NetworkToHost(uint64_t n) noexcept
{
    return be64toh(n);
}

inline float NetworkToHost(float f) noexcept
{
    uint32_t n = *reinterpret_cast<uint32_t*>(&f);
    n = bswap_32(n);
    return *reinterpret_cast<float*>(&n);
}
#elif defined(OS_WIN)

inline int16_t NetworkToHost(int16_t n) noexcept
{
    return _byteswap_ushort(n);
}

inline uint16_t NetworkToHost(uint16_t n) noexcept
{
    return _byteswap_ushort(n);
}

inline int32_t NetworkToHost(int32_t n) noexcept
{
    return _byteswap_ulong(n);
}

inline uint32_t NetworkToHost(uint32_t n) noexcept
{
    return _byteswap_ulong(n);
}

inline int64_t NetworkToHost(int64_t n) noexcept
{
    return _byteswap_uint64(n);
}

inline uint64_t NetworkToHost(uint64_t n) noexcept
{
    return _byteswap_uint64(n);
}

inline float NetworkToHost(float f) noexcept
{
	uint32_t n = *reinterpret_cast<uint32_t*>(&f);
	n = _byteswap_ulong(n);
	return *reinterpret_cast<float*>(&n);
}

#endif  // OS_POSIX

}   // namespace ezio

#endif  // EZIO_ENDIAN_UTILS_H_
