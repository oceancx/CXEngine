/*
 @ 0xCCCCCCCC
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef KBASE_ERROR_EXCEPTION_UTIL_H_
#define KBASE_ERROR_EXCEPTION_UTIL_H_

#include <memory>
#include <sstream>
#include <string>
#include <stdexcept>

#include "kbase/basic_macros.h"
#include "kbase/basic_types.h"
#include "kbase/string_encoding_conversions.h"
#include "kbase/string_view.h"

#if defined(OS_WIN)
#include <Windows.h>
#endif

namespace kbase {

enum class EnsureAction {
    Check,
    Throw,
};

constexpr bool NotReached() noexcept
{
    return false;
}

namespace internal {

struct general_category_tag {};
struct wide_string_category_tag {};
struct enum_category_tag {};

template<typename T, typename = void>
struct map_type_to_category
    : general_category_tag
{};

template<typename T>
struct map_type_to_category<T, std::enable_if_t<std::is_enum<T>::value>>
    : enum_category_tag
{};

template<typename T>
struct map_type_to_category<T, std::enable_if_t<std::is_same<T, std::wstring>::value ||
                                                std::is_same<T, kbase::WStringView>::value>>
    : wide_string_category_tag
{};

class ExceptionPump {
public:
    virtual ~ExceptionPump() = default;
    virtual void Throw(const std::string& what) const = 0;
};

template<typename E>
class ExceptionPumpImpl : public ExceptionPump {
public:
    void Throw(const std::string& what) const override
    {
        throw E(what);
    }
};

}   // namespace internal

class EnsureFailure : public std::runtime_error {
public:
    explicit EnsureFailure(const std::string& what)
        : runtime_error(what)
    {}

    explicit EnsureFailure(const char* what)
        : runtime_error(what)
    {}
};

#define ENSURE_ACTION_CHECK kbase::EnsureAction::Check
#define ENSURE_ACTION_THROW kbase::EnsureAction::Throw

// The action `CHECK` is performed only in debug mode.
// Besides, we also need to make the CHECK-call cause no runtime penalty,
// when in non-debug mode.
#if defined(NDEBUG)
#define ACTION_IS_ON(action) (ENSURE_ACTION_##action != ENSURE_ACTION_CHECK)
#else
#define ACTION_IS_ON(action) true
#endif

#define GUARANTOR_A(x) GUARANTOR_OP(x, B)
#define GUARANTOR_B(x) GUARANTOR_OP(x, A)
#define GUARANTOR_OP(x, next) \
    GUARANTOR_A.CaptureVar(#x, (x)).GUARANTOR_##next

#define MAKE_GUARANTOR(cond, action) \
    kbase::Guarantor(cond, __FILE__, __LINE__, ENSURE_ACTION_##action)

#define ENSURE(action, cond) \
    static_assert(std::is_same<std::remove_const_t<decltype(cond)>, bool>::value, \
                  "cond must be a bool expression"); \
    (!ACTION_IS_ON(action) || (cond)) ? (void)0 : MAKE_GUARANTOR(#cond, action).GUARANTOR_A

class Guarantor {
public:
    Guarantor(const char* condition, const char* file_name, int line, EnsureAction action)
        : action_(action)
    {
        // Keep execution in construction short, and try not to call WinAPI here,
        // which might overwrite last-error code we need, even when they succeed.
        exception_desc_ << "Failed: " << condition
                        << "\nFile: " << file_name << " Line: " << line
                        << "\nCaptured diagnostic variables:\n";
    }

    ~Guarantor() = default;

    DISALLOW_COPY(Guarantor);

    DISALLOW_MOVE(Guarantor);

    // Capture diagnostic variables.

    template<typename T>
    Guarantor& CaptureVar(const char* name, const T& value)
    {
        HandleCapturedVar(name, value, internal::map_type_to_category<T>{});
        return *this;
    }

    template<typename E>
    Guarantor& ThrowIn()
    {
        static_assert(std::is_base_of<std::exception, E>::value, "E is not a std::exception");
        exception_pump_ = std::make_unique<internal::ExceptionPumpImpl<E>>();
        return *this;
    }

    void Require(StringView msg = {});

    // Access stubs for infinite variable capture.
    Guarantor& GUARANTOR_A = *this;
    Guarantor& GUARANTOR_B = *this;

private:
    void DoCheck() const;

    void DoThrow();

    template<typename T>
    void HandleCapturedVar(const char* name, const T& value, internal::general_category_tag)
    {
        RecordCapturedVar(name, value);
    }

    template<typename T>
    void HandleCapturedVar(const char* name, const T& value, internal::wide_string_category_tag)
    {
        RecordCapturedVar(name, WideToUTF8(value));
    }

    template<typename T>
    void HandleCapturedVar(const char* name, T value, internal::enum_category_tag)
    {
        RecordCapturedVar(name, enum_cast(value));
    }

    template<typename T>
    void RecordCapturedVar(const char* name, const T& value)
    {
        exception_desc_ << "    " << name << " = " << value << "\n";
    }

private:
    EnsureAction action_;
    std::ostringstream exception_desc_;
    std::unique_ptr<internal::ExceptionPump> exception_pump_;
};

void AlwaysCheckFirstInDebug(bool always_check) noexcept;

#if defined(OS_WIN)

// This class automatically retrieves the last error code of the calling thread when
// constructing an instance, and stores the value internally.
class LastError {
public:
    LastError() noexcept;

    ~LastError() = default;

    DWORD error_code() const noexcept;

    // Since the description of the error is intended for programmers only, the
    // function insists on using English as its dispalying language.
    std::wstring GetDescriptiveMessage() const;

private:
    DWORD error_code_;
};

std::ostream& operator<<(std::ostream& os, const LastError& last_error);

#endif

}   // namespace kbase

#endif  // KBASE_ERROR_EXCEPTION_UTIL_H_
