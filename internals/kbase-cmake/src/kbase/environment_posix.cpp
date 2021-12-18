/*
 @ 0xCCCCCCCC
*/

#include "kbase/environment.h"

#include <cstring>

#include <unistd.h>

#include "kbase/logging.h"

namespace {

using CharType = kbase::Environment::CharType;
using StringType = kbase::Environment::StringType;
using EnvTable = kbase::Environment::EnvTable;

}   // namespace

namespace kbase {

// static
StringType Environment::GetVar(const CharType* name)
{
    const CharType* value = getenv(name);
    if (!value) {
        return StringType();
    }

    return StringType(value);
}

// static
bool Environment::HasVar(const CharType* name)
{
    return getenv(name) != nullptr;
}

// static
void Environment::SetVar(const CharType* name, const StringType& value)
{
    auto rv = setenv(name, value.c_str(), 1);
    DLOG_IF(ERROR, rv != 0) << "Failed to call setenv; errno: " << errno;
}

// static
void Environment::RemoveVar(const CharType* name)
{
    auto rv = unsetenv(name);
    DLOG_IF(ERROR, rv != 0) << "Failed to call unsetenv; errno: " << errno;
}

#if defined(OS_APPLE)
// static
EnvTable Environment::CurrentEnvironmentBlock()
{
    EnvTable current_env_table;
    return current_env_table;
}
#else
// static
EnvTable Environment::CurrentEnvironmentBlock()
{
    EnvTable current_env_table;

    for (int i = 0; environ[i] != nullptr; ++i) {
        const CharType* field_begin = environ[i];
        const CharType* delim_pos = strchr(field_begin, '=');
        current_env_table.insert({StringType(field_begin, delim_pos), StringType(delim_pos + 1)});
    }

    return current_env_table;
}
#endif

}   // namespace kbase
