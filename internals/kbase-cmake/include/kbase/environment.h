/*
 @ 0xCCCCCCCC
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef KBASE_ENVIRONMENT_H_
#define KBASE_ENVIRONMENT_H_

#include <map>
#include <string>

#include "kbase/basic_macros.h"

namespace kbase {

#if defined(OS_WIN)
#define ENV_LITERAL(x) L##x
#else
#define ENV_LITERAL(x) x
#endif

class Environment {
public:
#if defined(OS_WIN)
    using StringType = std::wstring;
#else
    using StringType = std::string;
#endif
    using CharType = StringType::value_type;
    using EnvTable = std::map<StringType, StringType>;

    Environment() = delete;

    ~Environment() = delete;

    DISALLOW_COPY(Environment);

    DISALLOW_MOVE(Environment);

    // Gets the value of an environment variable.
    // Returns empty string, if no such variable exists, or an error occurs, or even
    // the value is indeed empty.
    static StringType GetVar(const CharType* name);

    // Returns true if an environment variable specified by the given `name` exists.
    // Returns false otherwise.
    static bool HasVar(const CharType* name);

    // Creates an environment variable with the given name and value.
    static void SetVar(const CharType* name, const StringType& value);

    // Removes an existing environment variable.
    // This function has no effect if the variable to be removed does not exist.
    static void RemoveVar(const CharType* name);

    // Gets the environment block of the current process.
    static EnvTable CurrentEnvironmentBlock();
};

}   // namespace kbase

#endif  // KBASE_ENVIRONMENT_H_
