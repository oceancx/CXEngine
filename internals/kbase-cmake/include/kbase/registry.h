/*
 @ 0xCCCCCCCC
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef KBASE_REGISTRY_H_
#define KBASE_REGISTRY_H_

#include <memory>
#include <string>
#include <vector>

#include <Windows.h>

#include "kbase/basic_macros.h"

namespace kbase {

// A RegKey instance represents an opened registry key on Windows.
// Create() or Open() may result in an invalid RegKey instance, which will throw an
// exception if further operations involving registry manipulations are performed.
// Validity of an RegKey instance can be indicated by boolean test.

class RegKey {
public:
    enum WOW6432Node {
        DefaultKeyOnWOW64,
        Force32KeyOnWOW64,
        Force64KeyOnWOW64
    };

    // The created instance remains invalid until it succeeds in calling Open() to open
    // a registry key.
    RegKey() noexcept;

    RegKey(RegKey&& other) noexcept;

    RegKey& operator=(RegKey&& other) noexcept;

    ~RegKey();

    DISALLOW_COPY(RegKey);

    // Create or open a registry key, and return a RegKey instance to represent.
    // The `disposition` indicates the exact behavior.

    static RegKey Create(HKEY rootkey, const wchar_t* subkey, REGSAM access);

    static RegKey Create(HKEY rootkey, const wchar_t* subkey, REGSAM access, DWORD& disposition);

    // `subkey` here is relative to the current registry key.
    void Open(NOT_NULL const wchar_t* subkey, REGSAM access);

    void Open(NOT_NULL HKEY rootkey, NOT_NULL const wchar_t* subkey, REGSAM access);

    HKEY Get() const noexcept;

    // Releases the ownership of the opened key.
    HKEY Release() noexcept;

    void Close() noexcept;

    explicit operator bool() const noexcept
    {
        return IsValid();
    }

    const std::wstring& subkey_name() const noexcept
    {
        return subkey_name_;
    }

    // Returns true if the registry key exists; Returns false if it doesn't.
    // For details about WOW6432Node redirection, see @ http://is.gd/6Z23qk
    static bool KeyExists(HKEY rootkey, const wchar_t* subkey,
                          WOW6432Node node_key = WOW6432Node::DefaultKeyOnWOW64);

    // Removes a specific key along with all subkeys it contains. Use with care.
    // The key must have been opened with DELETE, KEY_ENUMERATE_SUB_KEYS, KEY_QUERY_VALUE,
    // and KEY_SET_VALUE access rights.
    // `key_name` may be nullptr, if you want to delete all subkeys of the current
    // registry key.
    void DeleteKey(const wchar_t* key_name) const;

    // Returns true, if this key has the specified value.
    // Returns false if it doesn't.
    bool HasValue(const wchar_t* value_name) const;

    // Gets count of values in key.
    size_t GetValueCount() const;

    // `index` is in the range [0, value_count).
    void GetValueNameAt(size_t index, std::wstring& value_name) const;

    // Removes a specific value under the key.
    void DeleteValue(const wchar_t* value_name) const;

    // Reads a REG_DWORD registry field.
    void ReadValue(const wchar_t* value_name, DWORD& value) const;

    // Reads a REG_QWORD registry field.
    void ReadValue(const wchar_t* value_name, DWORD64& value) const;

    // Reads a REG_SZ or a REG_EXPAND_SZ registry field into the `value`.
    void ReadValue(const wchar_t* value_name, std::wstring& value) const;

    // Reads a REG_MULTI_SZ registry field into a vector of strings.
    void ReadValue(const wchar_t* value_name, std::vector<std::wstring>& values) const;

    // Reads raw data, and `data_type` indicates the what the type of the data is.

    void ReadRawValue(const wchar_t* value_name, DWORD restricted_type, void* data,
                      DWORD& data_size) const;

    void ReadRawValue(const wchar_t* value_name, void* data, DWORD& data_size,
                      DWORD& data_type) const;

    // Sets a REG_DWORD registry field.
    void WriteValue(const wchar_t* value_name, DWORD value) const;

    // Sets a REG_QWORD registry field.
    void WriteValue(const wchar_t* value_name, DWORD64 value) const;

    // Sets a REG_SZ registry field.
    void WriteValue(const wchar_t* value_name, const wchar_t* value, size_t length) const;

    // Sets a raw data into a specific registry field.
    void WriteValue(const wchar_t* value_name, const void* data, size_t data_size,
                    DWORD data_type) const;

private:
    RegKey(HKEY rootkey, const wchar_t* subkey, REGSAM access, DWORD& disposition);

    bool IsValid() const noexcept
    {
        return key_ != nullptr;
    }

private:
    HKEY key_;
    std::wstring subkey_name_;
};

// Iterates over the sub-keys of a given registry key.
class RegKeyIterator {
public:
    using iterator_category = std::input_iterator_tag;
    using difference_type = ptrdiff_t;
    using value_type = RegKey;
    using reference = const RegKey&;
    using pointer = const RegKey*;

    // Constructs an end iterator.
    RegKeyIterator() = default;

    // Constructs a registry key iterator that refers to the first subkey of the given
    // registry key. If the given key cannot be iterated, returns an end iterator.
    RegKeyIterator(HKEY rootkey, const wchar_t* subkey);

    explicit RegKeyIterator(const RegKey& key);

    ~RegKeyIterator() = default;

    DEFAULT_COPY(RegKeyIterator);

    DEFAULT_MOVE(RegKeyIterator);

    RegKeyIterator& operator++();

    reference operator*() const noexcept;

    pointer operator->() const noexcept;

    friend bool operator==(const RegKeyIterator& lhs, const RegKeyIterator& rhs) noexcept
    {
        return lhs.impl_ == rhs.impl_;
    }

    friend bool operator!=(const RegKeyIterator& lhs, const RegKeyIterator& rhs) noexcept
    {
        return !(lhs == rhs);
    }

private:
    class Impl;
    std::shared_ptr<Impl> impl_;
};

inline RegKeyIterator& begin(RegKeyIterator& it) noexcept
{
    return it;
}

inline const RegKeyIterator& begin(const RegKeyIterator& it) noexcept
{
    return it;
}

inline RegKeyIterator end(const RegKeyIterator&) noexcept
{
    return RegKeyIterator();
}

}   // namespace kbase

#endif  // KBASE_REGISTRY_H_
