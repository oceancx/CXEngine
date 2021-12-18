/*
 @ 0xCCCCCCCC
*/

#include "kbase/base_path_provider.h"

#include <cstdlib>

#include <limits.h>
#include <unistd.h>

#include "kbase/error_exception_util.h"

namespace {

using kbase::NotReached;
using kbase::Path;

constexpr char kProcSelfExe[] = "/proc/self/exe";

Path GetExecutableDirectory()
{
    constexpr size_t kBufSize = PATH_MAX + 1;
    char buf[kBufSize] {0};
    if (-1 == readlink(kProcSelfExe, buf, kBufSize)) {
        ENSURE(CHECK, NotReached()).Require();
        return Path();
    }

    return Path(buf);
}

Path GetCurrentDirectory()
{
    char buf[PATH_MAX];
    if (!getcwd(buf, sizeof(buf))) {
        ENSURE(CHECK, NotReached()).Require();
        return Path();
    }

    return Path(buf);
}

Path GetTempDirectory()
{
    const char* tmp = getenv("TMPDIR");
    if (tmp) {
        return Path(tmp);
    }

    return Path("/tmp");
}

Path GetHomeDirectory()
{
    const char* home = getenv("HOME");
    if (home) {
        return Path(home);
    }

    ENSURE(CHECK, NotReached()).Require();

    return Path();
}

}   // namespace

namespace kbase {

Path BasePathProvider(PathKey key)
{
    Path path;

    switch (key) {
        case FileExe:
        case FileModule:
            path = GetExecutableDirectory();
            break;

        case DirExe:
        case DirModule:
            path = GetExecutableDirectory().parent_path();
            break;

        case DirCurrent:
            path = GetCurrentDirectory();
            break;

        case DirTemp:
            path = GetTempDirectory();
            break;

        case DirHome:
            path = GetHomeDirectory();
            break;
    }

    return path;
}

}   // namespace kbase
