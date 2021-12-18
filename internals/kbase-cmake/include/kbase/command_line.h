/*
 @ 0xCCCCCCCC
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef KBASE_COMMAND_LINE_H_
#define KBASE_COMMAND_LINE_H_

#include <map>
#include <vector>

#include "kbase/basic_macros.h"
#include "kbase/path.h"

namespace kbase {

#if defined(OS_WIN)
#define CMDLINE_LITERAL(x) L##x
#else
#define CMDLINE_LITERAL(x) x
#endif

// A command line consists of one or more arguments, which are tokens separated by
// one or more spaces or tabs.
// Arguments preceded with '--', '-', and '/' are switches. A switch can optionally
// have a value which is delimited by '='.
// Arguments that are not switches are called parameters. They are just specific
// values.
// Besides, the first argument is called program, since it always refers to the full
// path of the program.
class CommandLine {
public:
#if defined(OS_WIN)
    using StringType = std::wstring;
#else
    using StringType = std::string;
#endif
    using CharType = StringType::value_type;
    using ArgList = std::vector<StringType>;
    using SwitchTable = std::map<StringType, StringType>;

    enum SwitchPrefix : unsigned int {
        PrefixDoubleDash = 0,
        PrefixDash,
        PrefixSlash
    };

    explicit CommandLine(const Path& program);

    explicit CommandLine(const ArgList& args);

    CommandLine(int argc, const CharType* const* argv);

#if defined(OS_WIN)
    // `cmdline` has same requirement as in ParseFromString.
    explicit CommandLine(const StringType& cmdline);
#endif

    DEFAULT_COPY(CommandLine);

    DEFAULT_MOVE(CommandLine);

    ~CommandLine() = default;

    // Initializes the singleton CommandLine instance for the current process. On Windows,
    // be sure to ignore its arguments, we internally use GetCommandLineW() instead, because
    // CRT's parsing algorithm on Windows is not reliable.
    static void Init(int argc, const char* const* argv);

    // Returns the current process's singleton CommandLine instance.
    static const CommandLine& ForCurrentProcess();

    void ParseFromArgs(int argc, const CharType* const* argv);

    void ParseFromArgs(const ArgList& args);

#if defined(OS_WIN)
    // Make sure that file path of the program, i.e. argv[0], is enclosed with
    // quotation marks, if the path may contain spaces.
    void ParseFromString(const StringType& cmdline);
#endif

    SwitchPrefix switch_prefix() const noexcept;

    void set_switch_prefix(SwitchPrefix prefix) noexcept;

    Path GetProgram() const;

    void SetProgram(const Path& program);

    // `name` should not be preceded with prefix.
    CommandLine& AppendSwitch(const StringType& name, const StringType& value = StringType());

    CommandLine& AppendParameter(const StringType& arg);

    // `name` should not be preceded with prefix.
    bool HasSwitch(const StringType& name) const;

    // Returns true if succeeded in querying the value associated with the switch.
    // Returns false if no such switch was found, and `value` remains unchanged.
    bool GetSwitchValue(const StringType& name, StringType& value) const;

    bool GetSwitchValueASCII(const StringType& name, std::string& value) const;

    const SwitchTable& GetSwitches() const noexcept
    {
        return switches_;
    }

    ArgList GetParameters() const;

    const ArgList& GetArgs() const noexcept;

    // The general order of arguments in output string is as follows:
    // { program, [(-|--|/)switch[=value]], [parameter] }
    // that is, switches always precede with arguments.
    StringType GetCommandLineString() const;

    // Similar with GetCommandLineString() but without including program.
    StringType GetArgsStringWithoutProgram() const;

private:
    ArgList args_;
    size_t arg_not_param_count_;
    SwitchTable switches_;
    SwitchPrefix switch_prefix_;
    static CommandLine* current_process_cmdline_;
};

}   // namespace kbase

#endif  // KBASE_COMMAND_LINE_H_
