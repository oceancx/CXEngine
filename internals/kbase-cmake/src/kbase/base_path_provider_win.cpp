/*
 @ 0xCCCCCCCC
*/

#include "kbase/base_path_provider.h"

#include <Windows.h>
#include <ShlObj.h>

#include "kbase/error_exception_util.h"
#include "kbase/path_service.h"
#include "kbase/scope_guard.h"

// See @ http://blogs.msdn.com/b/oldnewthing/archive/2004/10/25/247180.aspx
extern "C" IMAGE_DOS_HEADER __ImageBase;

// Use macros to enforce inlining code.
#define CURRENT_EXE() nullptr
#define CURRENT_MODULE() reinterpret_cast<HMODULE>(&__ImageBase)

namespace {

using kbase::Path;

Path ShellGetFolderPath(const KNOWNFOLDERID& folder_id)
{
    wchar_t* folder_path = nullptr;
    HRESULT ret = SHGetKnownFolderPath(folder_id, 0, nullptr, &folder_path);
    ON_SCOPE_EXIT { CoTaskMemFree(folder_path); };
    ENSURE(CHECK, ret == S_OK)(ret).Require();

    return Path(folder_path);
}

Path GetModulePath(HMODULE module, wchar_t* buf, DWORD buf_size)
{
    GetModuleFileNameW(module, buf, buf_size);
    return Path(buf);
}

}   // namespace

namespace kbase {

Path BasePathProvider(PathKey key)
{
    // For simplicity, just ignore long path here.
    constexpr DWORD kMaxPath = MAX_PATH + 1;
    wchar_t buffer[kMaxPath] {0};
    Path path;

    switch (key) {
        case FileExe:
            path = GetModulePath(CURRENT_EXE(), buffer, kMaxPath);
            break;

        case FileModule: {
            path = GetModulePath(CURRENT_MODULE(), buffer, kMaxPath);
            break;
        }

        case DirExe:
            path = GetModulePath(CURRENT_EXE(), buffer, kMaxPath).parent_path();
            break;

        case DirModule:
            path = GetModulePath(CURRENT_MODULE(), buffer, kMaxPath).parent_path();
            break;

        case DirCurrent:
            GetCurrentDirectoryW(kMaxPath, buffer);
            path = Path(buffer);
            break;

        case DirTemp:
            GetTempPathW(kMaxPath, buffer);
            path = Path(buffer);
            break;

        case DirHome:
            path = ShellGetFolderPath(FOLDERID_Profile);
            break;

        case DirUserDesktop:
            path = ShellGetFolderPath(FOLDERID_Desktop);
            break;

        case DirPublicDesktop:
            path = ShellGetFolderPath(FOLDERID_PublicDesktop);
            break;

        case DirWindows:
            path = ShellGetFolderPath(FOLDERID_Windows);
            break;

        case DirSystem:
            GetSystemDirectoryW(buffer, kMaxPath);
            path = Path(buffer);
            break;

        case DirProgramFiles:
            path = ShellGetFolderPath(FOLDERID_ProgramFiles);
            break;

        case DirProgramFilesX86:
            path = ShellGetFolderPath(FOLDERID_ProgramFilesX86);
            break;

        case DirRoamingAppData:
            path = ShellGetFolderPath(FOLDERID_RoamingAppData);
            break;

        case DirLocalAppData:
            path = ShellGetFolderPath(FOLDERID_LocalAppData);
            break;

        case DirCommonAppData:
            path = ShellGetFolderPath(FOLDERID_ProgramData);
            break;

        default:
            break;
    }

    return path;
}

}   // namespace kbase
