/*
 @ 0xCCCCCCCC
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef KBASE_PATH_SERVICE_H_
#define KBASE_PATH_SERVICE_H_

#include <functional>

#include "kbase/basic_macros.h"
#include "kbase/basic_types.h"
#include "kbase/path.h"

namespace kbase {

// The path service is thread-safe. It is also OK to call PathService::Get() inside
// a provider function.

class PathService {
public:
    using ProviderFunc = std::function<Path(PathKey)>;

    PathService() = delete;

    ~PathService() = delete;

    DISALLOW_COPY(PathService);

    DISALLOW_MOVE(PathService);

    // Returns the absolute path corresponding to the key if the function succeeds.
    // Returns an empty file path if no matching was found.
    static Path Get(PathKey key);

    // Users can register their own path provider along with a bunch of new path keys.
    // By default, only in debug mode does this function internally do key overlapping
    // checking.
    static void RegisterPathProvider(ProviderFunc provider, PathKey start, PathKey end);

    // Disables internal cache.
    static void DisableCache();

    // Enables internal cache.
    static void EnableCache();
};

}   // namespace kbase

#endif  // KBASE_PATH_SERVICE_H_
