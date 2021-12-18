/*
 @ 0xCCCCCCCC
*/

#include "kbase/path_service.h"

#include <mutex>
#include <unordered_map>
#include <vector>

#include "kbase/base_path_provider.h"
#include "kbase/error_exception_util.h"
#include "kbase/file_util.h"
#include "kbase/lazy.h"

namespace {

using kbase::Lazy;
using kbase::Path;
using kbase::PathKey;
using kbase::PathService;

using ProviderFunc = PathService::ProviderFunc ;
using PathMap = std::unordered_map<PathKey, Path>;

// Both `start` and `end` are used to prevent path keys claimed by different
// providers being overlapped.
struct PathProvider {
    ProviderFunc fn;
    PathKey start;
    PathKey end;
};

struct PathContext {
    std::recursive_mutex mutex;
    std::vector<PathProvider> providers;
    PathMap path_cache;
    bool cache_disabled;

    PathContext()
        : providers({ PathProvider { kbase::BasePathProvider,
                                     kbase::BasePathStart,
                                     kbase::BasePathEnd }}),
          cache_disabled(false)
    {}

    DISALLOW_COPY(PathContext);
};

PathContext& GetPathContext()
{
    static Lazy<PathContext> path_context;
    return path_context.value();
}

// Returns the path corresponding to the `key`, or an empty path if no cache was found.
Path GetPathFromCache(PathKey key)
{
    auto& path_context = GetPathContext();
    std::lock_guard<std::recursive_mutex> lock(path_context.mutex);

    if (path_context.cache_disabled || key == kbase::DirCurrent) {
        return Path();
    }

    auto it = path_context.path_cache.find(key);
    return it != path_context.path_cache.end() ? it->second : Path();
}

void CachePathWithKey(PathKey key, const Path& path)
{
    auto& path_context = GetPathContext();
    std::lock_guard<std::recursive_mutex> lock(path_context.mutex);

    // We don't cache current directory.
    if (!path_context.cache_disabled && key != kbase::DirCurrent) {
        path_context.path_cache[key] = path;
    }
}

#if !defined(NDEBUG)
void EnsureNoPathKeyOverlapped(PathKey start, PathKey end, const PathContext& path_data)
{
    for (const PathProvider& provider : path_data.providers) {
        ENSURE(CHECK, start > provider.end || end < provider.start)
            (start)(end)(provider.start)(provider.end).Require();
    }
}
#endif

}   // namespace

namespace kbase {

// static
Path PathService::Get(PathKey key)
{
    ENSURE(CHECK, key >= BasePathStart)(key).Require();
    ENSURE(CHECK, GetPathContext().providers.empty() == false).Require();

    PathContext& path_context = GetPathContext();

    Path path = GetPathFromCache(key);
    if (!path.empty()) {
        return path;
    }

    {
        std::lock_guard<std::recursive_mutex> lock(path_context.mutex);
        for (const auto& provider : path_context.providers) {
            path = provider.fn(key);
            if (!path.empty()) {
                break;
            }
        }
    }

    // No path that associates with the key was found.
    if (path.empty()) {
        return path;
    }

    // Ensure the returned path never contains '..'.
    if (!path.IsAbsolute()) {
        Path full_path = MakeAbsolutePath(path);
        ENSURE(CHECK, !full_path.empty())(path.value()).Require();
        path = std::move(full_path);
    }

    CachePathWithKey(key, path);

    return path;
}

// static
void PathService::RegisterPathProvider(ProviderFunc provider, PathKey start, PathKey end)
{
    ENSURE(CHECK, start < end)(start)(end).Require();

    PathContext& path_context = GetPathContext();

    std::lock_guard<std::recursive_mutex> lock(path_context.mutex);

#if !defined(NDEBUG)
    EnsureNoPathKeyOverlapped(start, end, path_context);
#endif

    path_context.providers.push_back(PathProvider { provider, start, end });
}

// static
void PathService::DisableCache()
{
    PathContext& path_context = GetPathContext();

    std::lock_guard<std::recursive_mutex> scoped_lock(path_context.mutex);

    if (!path_context.cache_disabled) {
        path_context.path_cache.clear();
        path_context.cache_disabled = true;
    }
}

// static
void PathService::EnableCache()
{
    PathContext& path_context = GetPathContext();

    std::lock_guard<std::recursive_mutex> lock(path_context.mutex);

    if (path_context.cache_disabled) {
        path_context.cache_disabled = false;
    }
}

}   // namespace kbase
