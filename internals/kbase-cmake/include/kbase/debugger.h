/*
 @ 0xCCCCCCCC
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef KBASE_DEBUGGER_H_
#define KBASE_DEBUGGER_H_

namespace kbase {

bool IsDebuggerPresent();

void BreakDebugger();

}   // namespace kbase

#endif  // KBASE_DEBUGGER_H_
