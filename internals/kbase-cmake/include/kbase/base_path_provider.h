/*
 @ 0xCCCCCCCC
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef KBASE_BASE_PATH_PROVIDER_H_
#define KBASE_BASE_PATH_PROVIDER_H_

#include "kbase/basic_macros.h"
#include "kbase/basic_types.h"
#include "kbase/path.h"

namespace kbase {

enum BasePathKey : PathKey {
    BasePathStart = 0,
    FileExe,               // Path with filename of the current executable.
    FileModule,            // Path with filename of the module running the code.
    DirExe,                // Directory containing FileExe.
    DirModule,             // Directory containing FileModule.
    DirCurrent,            // Current directory.
    DirTemp,               // Temporary directory.
    DirHome,               // User's home directory.
#if defined(OS_WIN)
    DirUserDesktop,        // Current user's desktop.
    DirPublicDesktop,      // Public user's desktop.
    DirWindows,            // Windows directory, i.e. C:\Windows.
    DirSystem,             // System directory, i.e. C:\Windows\System32.
    DirProgramFiles,       // Directory C:\Program Files.
    DirProgramFilesX86,    // Directory C:\Program Files or C:\Program Files (x86).
    DirRoamingAppData,     // Directory %AppData%.
    DirLocalAppData,       // Directory %LocalAppData%.
    DirCommonAppData,      // Usually C:\ProgramData.
#endif
    BasePathEnd
};

Path BasePathProvider(PathKey key);

}   // namespace kbase

#endif // KBASE_BASE_PATH_PROVIDER_H_
