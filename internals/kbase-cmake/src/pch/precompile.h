/*
 @ Kingsley Chen
*/

// Add frequently used header files or remove header files that are less frequently
// used during project evolvement.

#if defined(BUILD_PCH_PRECOMPILE_H_)
#error Include the precompiled header file more than once is prohibited.
#endif

#define BUILD_PCH_PRECOMPILE_H_

// The Windows header needs to come before almost all the other
// Windows-specific headers.
#ifdef _WIN32
#include <Windows.h>
#include <dwmapi.h>
#include <shellapi.h>
#include <wincrypt.h>
#include <wtypes.h>
#endif
// C-Runtime headers.
#include <fcntl.h>
#include <memory.h>

// CPP-Runtime headers.
#include <algorithm>
#include <bitset>
#include <cerrno>
#include <climits>
#include <cmath>
#include <csignal>
#include <cstdarg>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <deque>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iosfwd>
#include <iterator>
#include <limits>
#include <list>
#include <map>
#include <numeric>
#include <ostream>
#include <queue>
#include <set>
#include <sstream>
#include <stack>
#include <string>
#include <utility>
#include <vector>