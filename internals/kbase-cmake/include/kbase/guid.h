/*
 @ 0xCCCCCCCC
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef KBASE_GUID_H_
#define KBASE_GUID_H_

#include "kbase/string_view.h"

namespace kbase {

// Returns a GUID string in the form of version 4 as described in RFC.
// Returns an empty string if failed.
std::string GenerateGUID();

// Returns true, if the argument given is a valid GUID.
// Returns false, otherwise.
// If `strict_mode` is true, then all hexadecimal values "a" through "f" are required
// in lower case, as version 4 RFC says they're case insensitive.
bool IsGUIDValid(StringView guid, bool strict_mode = false);

}   // namespace kbase

#endif  // KBASE_GUID_H_
