/*
 @ 0xCCCCCCCC
*/

#include "kbase/environment.h"

#include <vector>

#include <Windows.h>

#include "kbase/error_exception_util.h"
#include "kbase/logging.h"
#include "kbase/scope_guard.h"
#include "kbase/string_view.h"

namespace {

using CharType = kbase::Environment::CharType;
using StringType = kbase::Environment::StringType;
using EnvTable = kbase::Environment::EnvTable;

// Not including null-terminator
constexpr DWORD kEnvVarMaxSize = 32766;

void ParseEnvironmentBlock(const CharType* block_string, EnvTable& env_table)
{
    if (!block_string || *block_string == L'\0') {
        DLOG(ERROR) << "block_string is NULL or it points to an empty block!";
        return;
    }

    // Now the environment block has at least one line.
    const CharType* cur = block_string;
    const CharType* line_begin = block_string;
    const CharType* field_delim_pos = nullptr;
    while (true) {
        if (*cur == L'=') {
            field_delim_pos = cur;
        } else if (*cur == L'\0') {
            auto line_end = cur;
            env_table.insert({StringType(line_begin, field_delim_pos),
                              StringType(field_delim_pos + 1, line_end)});

            if (*(cur + 1) == L'\0') {
                break;
            }

            // We can skip the first character of the next line.
            line_begin = ++cur;
        }

        ++cur;
    }
}

}   // namespace

namespace kbase {

// static
StringType Environment::GetVar(const CharType* name)
{
    DWORD required_size = GetEnvironmentVariableW(name, nullptr, 0);
    if (required_size == 0) {
        auto err_code = LastError().error_code();
        DLOG_IF(ERROR, err_code != ERROR_ENVVAR_NOT_FOUND) <<
            "Failed to call GetEnvrionmentVariableW; error: " << err_code;
        return StringType();
    }

    std::vector<CharType> buf;
    buf.resize(required_size);
    if (0 == GetEnvironmentVariableW(name, buf.data(), required_size)) {
        // If the value is indeed empty, the function fails anyway, and with last error
        // being set ERROR_ENVVAR_NOT_FOUND.
        auto err_code = LastError().error_code();
        DLOG_IF(ERROR, err_code != ERROR_ENVVAR_NOT_FOUND) <<
            "Failed to call GetEnvrionmentVariableW; error: " << err_code;
        return StringType();
    }

    return StringType(buf.data());
}

// static
bool Environment::HasVar(const CharType* name)
{
    if (0 == GetEnvironmentVariableW(name, nullptr, 0)) {
        auto err_code = LastError().error_code();
        DLOG_IF(ERROR, err_code != ERROR_ENVVAR_NOT_FOUND) <<
            "Failed to call GetEnvrionmentVariableW; error: " << err_code;
        return false;
    }

    return true;
}

// static
void Environment::SetVar(const CharType* name, const StringType& value)
{
    ENSURE(CHECK, value.size() <= kEnvVarMaxSize)(value.size()).Require();
    if (!SetEnvironmentVariableW(name, value.c_str())) {
        auto err_code = LastError().error_code();
        DLOG(ERROR) << "Failed to call SetEnvironmentVariableW; error: " << err_code;
    }
}

// static
void Environment::RemoveVar(const CharType* name)
{
    SetEnvironmentVariableW(name, nullptr);
}

// static
EnvTable Environment::CurrentEnvironmentBlock()
{
    LPWCH env_block = GetEnvironmentStringsW();
    ON_SCOPE_EXIT { FreeEnvironmentStringsW(env_block); };

    EnvTable current_env_table;
    ParseEnvironmentBlock(env_block, current_env_table);

    return current_env_table;
}

}   // namespace kbase
