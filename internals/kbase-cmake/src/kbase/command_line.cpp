/*
 @ 0xCCCCCCCC
*/

#include "kbase/command_line.h"

#include <algorithm>
#include <utility>

#include "kbase/basic_types.h"
#include "kbase/error_exception_util.h"
#include "kbase/logging.h"
#include "kbase/scope_guard.h"
#include "kbase/string_util.h"

#if defined(OS_WIN)
#include <Windows.h>

#include "kbase/string_encoding_conversions.h"
#endif

namespace {

using kbase::CommandLine;

using ArgList = CommandLine::ArgList;
using CharType = CommandLine::CharType;
using StringType = CommandLine::StringType;
using SwitchPrefix = CommandLine::SwitchPrefix;
using SwitchPair = std::pair<StringType, StringType>;

constexpr const CharType* kSwitchPrefixes[] {
    CMDLINE_LITERAL("--"), CMDLINE_LITERAL("-"), CMDLINE_LITERAL("/")
};

constexpr CharType kSwitchValueDelimiter[] = CMDLINE_LITERAL("=");

constexpr SwitchPrefix kDefaultSwitchPrefix = SwitchPrefix::PrefixDoubleDash;

bool IsArgumentSwitch(const StringType& arg)
{
    return std::any_of(std::begin(kSwitchPrefixes), std::end(kSwitchPrefixes),
                       [&arg](const CharType* prefix) {
                           return kbase::StartsWith(arg, prefix);
                       });
}

SwitchPair UnstickSwitch(StringType& switch_token)
{
    kbase::TrimLeadingString(switch_token, CMDLINE_LITERAL("-/"));

    std::vector<StringType> members;
    kbase::SplitString(switch_token, kSwitchValueDelimiter, members);

    // Note that the switch may not carry a value.
    members.resize(2);

    return {members[0], members[1]};
}

void AddArguments(CommandLine& cmdline, const ArgList& args)
{
    for (auto arg = std::next(args.cbegin()); arg != args.cend(); ++arg) {
        StringType sanitized_arg = *arg;
        kbase::TrimString(sanitized_arg, CMDLINE_LITERAL(" \t"));
        if (IsArgumentSwitch(sanitized_arg)) {
            auto switch_member = UnstickSwitch(sanitized_arg);
            cmdline.AppendSwitch(switch_member.first, switch_member.second);
        } else {
            cmdline.AppendParameter(sanitized_arg);
        }
    }
}

#if defined(OS_WIN)
// Quotes the `arg` if necessary.
// Algorithm comes from http://goo.gl/mxKhoj
StringType QuoteArg(const StringType& arg)
{
    if (arg.find_first_of(L" \t\"") == arg.npos) {
        return arg;
    }

    StringType quoted_arg(1, L'"');
    for (auto it = arg.cbegin(); ; ++it) {
        size_t number_of_backslash = 0U;

        // Count number adjacent backslashes.
        while (it != arg.cend() && *it == L'\\') {
            ++it;
            ++number_of_backslash;
        }

        if (it == arg.cend()) {
            quoted_arg.append(2 * number_of_backslash, L'\\');
            break;
        } else if (*it == L'"') {
            quoted_arg.append(2 * number_of_backslash + 1, L'\\');
            quoted_arg.push_back(*it);
        } else {
            quoted_arg.append(number_of_backslash, L'\\');
            quoted_arg.push_back(*it);
        }
    }

    quoted_arg.push_back(L'"');

    return quoted_arg;
}
#endif

}   // namespace

namespace kbase {

CommandLine* CommandLine::current_process_cmdline_ = nullptr;

CommandLine::CommandLine(const Path& program)
    : args_(1), arg_not_param_count_(1), switch_prefix_(kDefaultSwitchPrefix)
{
    SetProgram(program);
}

CommandLine::CommandLine(const ArgList& args)
    : args_(1), arg_not_param_count_(1), switch_prefix_(kDefaultSwitchPrefix)
{
    ParseFromArgs(args);
}

CommandLine::CommandLine(int argc, const CharType* const* argv)
    : args_(1), arg_not_param_count_(1), switch_prefix_(kDefaultSwitchPrefix)
{
    ParseFromArgs(argc, argv);
}

#if defined(OS_WIN)
CommandLine::CommandLine(const StringType& cmdline)
    : args_(1), arg_not_param_count_(1), switch_prefix_(kDefaultSwitchPrefix)
{
    ParseFromString(cmdline);
}
#endif

// static
void CommandLine::Init(int argc, const char* const* argv)
{
    if (current_process_cmdline_) {
        ENSURE(CHECK, NotReached()).Require();
        return;
    }

#if defined(OS_WIN)
    UNUSED_VAR(argc);
    UNUSED_VAR(argv);
    current_process_cmdline_ = new CommandLine(GetCommandLineW());
#else
    current_process_cmdline_ = new CommandLine(argc, argv);
#endif
}

// static
const CommandLine& CommandLine::ForCurrentProcess()
{
    ENSURE(CHECK, current_process_cmdline_ != nullptr).Require();
    return *current_process_cmdline_;
}

SwitchPrefix CommandLine::switch_prefix() const noexcept
{
    return switch_prefix_;
}

void CommandLine::set_switch_prefix(SwitchPrefix prefix) noexcept
{
    switch_prefix_ = prefix;
}

Path CommandLine::GetProgram() const
{
    return Path(args_[0]);
}

void CommandLine::SetProgram(const Path& program)
{
    args_[0] = program.value();
    TrimString(args_[0], CMDLINE_LITERAL(" \t"));
}

void CommandLine::ParseFromArgs(int argc, const CharType* const* argv)
{
    ArgList bundled_args;
    for (int i = 0; i < argc; ++i) {
        bundled_args.emplace_back(argv[i]);
    }

    ParseFromArgs(bundled_args);
}

void CommandLine::ParseFromArgs(const ArgList& args)
{
    // Anyway, we start from scratch.
    args_ = ArgList(1);
    arg_not_param_count_ = 1;
    switches_.clear();

    SetProgram(Path(args[0]));
    AddArguments(*this, args);
}

#if defined(OS_WIN)
void CommandLine::ParseFromString(const StringType& cmdline)
{
    StringType sanitized_cmdline_str = cmdline;
    TrimString(sanitized_cmdline_str, CMDLINE_LITERAL(" \t"));

    int argc = 0;
    wchar_t** argv = nullptr;
    argv = CommandLineToArgvW(sanitized_cmdline_str.c_str(), &argc);
    ON_SCOPE_EXIT { if (argv) LocalFree(argv); };

    if (!argv) {
        auto last_err = LastError();
        LOG(ERROR) << "CommandLineToArgvW failed on " << WideToUTF8(cmdline) << "; " << last_err;
    }

    ParseFromArgs(argc, argv);
}
#endif

CommandLine& CommandLine::AppendSwitch(const StringType& name, const StringType& value)
{
    switches_[name] = value;

    // Since we have `last_arg_not_param_` to demarcate switches and parameters, we here
    // leave switch prefix unprepended.
    StringType switch_arg(name);
    if (!value.empty()) {
        switch_arg.append(1, '=').append(value);
    }

    args_.insert(std::next(args_.begin(), arg_not_param_count_), switch_arg);
    ++arg_not_param_count_;

    return *this;
}

CommandLine& CommandLine::AppendParameter(const StringType& parameter)
{
    args_.push_back(parameter);
    return *this;
}

bool CommandLine::HasSwitch(const StringType& name) const
{
    return switches_.find(name) != switches_.end();
}

bool CommandLine::GetSwitchValue(const StringType& name, StringType& value) const
{
    auto it = switches_.find(name);
    if (it == switches_.end()) {
        return false;
    }

    value = it->second;
    return true;
}

bool CommandLine::GetSwitchValueASCII(const StringType& name, std::string& value) const
{
    StringType switch_value;
    if (!GetSwitchValue(name, switch_value)) {
        return false;
    }

#if defined(OS_WIN)
    value = WideToASCII(switch_value);
#else
    value = switch_value;
#endif

    return true;
}

ArgList CommandLine::GetParameters() const
{
    auto params_begin = std::next(args_.cbegin(), arg_not_param_count_);
    return ArgList(params_begin, args_.cend());
}

const ArgList& CommandLine::GetArgs() const noexcept
{
    return args_;
}

StringType CommandLine::GetCommandLineString() const
{
#if defined(OS_WIN)
    StringType cmdline_str(QuoteArg(args_[0]));
#else
    StringType cmdline_str(args_[0]);
#endif
    cmdline_str.append(1, ' ').append(GetArgsStringWithoutProgram());

    return cmdline_str;
}

StringType CommandLine::GetArgsStringWithoutProgram() const
{
    ArgList stringified_args;
    stringified_args.reserve(args_.size());

    auto prefix = kSwitchPrefixes[enum_cast(switch_prefix())];
    for (size_t i = 1; i < args_.size(); ++i) {
        StringType arg_str = i + 1 <= arg_not_param_count_ ? StringType(prefix) : StringType();
        arg_str.append(args_[i]);
#if defined(OS_WIN)
        stringified_args.push_back(QuoteArg(arg_str));
#else
        stringified_args.push_back(arg_str);
#endif
    }

    return JoinString(stringified_args, StringType(1, ' '));
}

}   // namespace kbase
