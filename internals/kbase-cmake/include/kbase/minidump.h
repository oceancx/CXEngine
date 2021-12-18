/*
 @ 0xCCCCCCCC
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef KBASE_MINIDUMP_H_
#define KBASE_MINIDUMP_H_

namespace kbase {

class Path;

// Creates a minidump at a given path.
// Returns true if succeeded, returns false otherwise.
// Note that, if creating the minidump failed, we log the error only in debug mode;
// and do nothing in other builds.
bool CreateMiniDump(const Path& dump_path);

}   // namespace kbase

#endif  // KBASE_MINIDUMP_H_
