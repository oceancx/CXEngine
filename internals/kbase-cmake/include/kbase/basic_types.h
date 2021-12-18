/*
 @ 0xCCCCCCCC
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef KBASE_BASIC_TYPES_H_
#define KBASE_BASIC_TYPES_H_

#include <cstdint>
#include <string>

#include "kbase/basic_macros.h"

// Defines types that would be shared by among several files.
namespace kbase {

// `PathKey` is used by `PathService` and `BasePathProvider`.
using PathKey = int;

#if defined(OS_WIN)
using PathChar = wchar_t;
#else
using PathChar = char;
#endif

using PathString = std::basic_string<PathChar>;

using byte = uint8_t;

// Casts an enum value into an equivalent integer.
template<typename E>
constexpr auto enum_cast(E e) noexcept
{
    return static_cast<std::underlying_type_t<E>>(e);
}

namespace enum_ops {

template<typename E>
constexpr std::enable_if_t<std::is_enum<E>::value, E> operator|(E lhs, E rhs) noexcept
{
    return E(enum_cast(lhs) | enum_cast(rhs));
}

template<typename E>
constexpr std::enable_if_t<std::is_enum<E>::value, E&> operator|=(E& lhs, E rhs) noexcept
{
    lhs = E(enum_cast(lhs) | enum_cast(rhs));
    return lhs;
}

template<typename E>
constexpr std::enable_if_t<std::is_enum<E>::value, E> operator&(E lhs, E rhs) noexcept
{
    return E(enum_cast(lhs) & enum_cast(rhs));
}

template<typename E>
constexpr std::enable_if_t<std::is_enum<E>::value, E&> operator&=(E& lhs, E rhs) noexcept
{
    lhs = E(enum_cast(lhs) & enum_cast(rhs));
    return lhs;
}

template<typename E>
constexpr std::enable_if_t<std::is_enum<E>::value, E> operator^(E lhs, E rhs) noexcept
{
    return E(enum_cast(lhs) ^ enum_cast(rhs));
}

template<typename E>
constexpr std::enable_if_t<std::is_enum<E>::value, E&> operator^=(E& lhs, E rhs) noexcept
{
    lhs = E(enum_cast(lhs) ^ enum_cast(rhs));
    return lhs;
}

template<typename E>
constexpr std::enable_if_t<std::is_enum<E>::value, E> operator~(E op) noexcept
{
    return E(~enum_cast(op));
}

}   // namespace enum_ops

}   // namespace kbase

#endif  // KBASE_BASIC_TYPES_H_