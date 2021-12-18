/*
 @ 0xCCCCCCCC
*/

#include "kbase/registry.h"

#include <Shlwapi.h>

#include "kbase/error_exception_util.h"
#include "kbase/logging.h"
#include "kbase/scope_guard.h"
#include "kbase/string_encoding_conversions.h"
#include "kbase/string_util.h"

namespace kbase {

RegKey::RegKey() noexcept
    : key_(nullptr)
{}

RegKey::RegKey(HKEY rootkey, const wchar_t* subkey, REGSAM access, DWORD& disposition)
    : key_(nullptr)
{
    auto rv = RegCreateKeyExW(rootkey, subkey, 0, nullptr, REG_OPTION_NON_VOLATILE, access,
                              nullptr, &key_, &disposition);

    LOG_IF(WARNING, rv != ERROR_SUCCESS)
        << "Failed to create instance for key " << kbase::WideToUTF8(subkey)
        << "; Error: " << rv;

    if (rv == ERROR_SUCCESS) {
        subkey_name_ = subkey;
    }
}

RegKey::RegKey(RegKey&& other) noexcept
    : key_(other.Release()), subkey_name_(std::move(other.subkey_name_))
{}

RegKey& RegKey::operator=(RegKey&& other) noexcept
{
    Close();

    key_ = other.Release();
    subkey_name_ = std::move(other.subkey_name_);

    return *this;
}

RegKey::~RegKey()
{
    Close();
}

// static
RegKey RegKey::Create(HKEY rootkey, const wchar_t* subkey, REGSAM access)
{
    DWORD ignored;
    return Create(rootkey, subkey, access, ignored);
}

// static
RegKey RegKey::Create(HKEY rootkey, const wchar_t* subkey, REGSAM access, DWORD& disposition)
{
    ENSURE(CHECK, subkey != nullptr).Require();
    return RegKey(rootkey, subkey, access, disposition);
}

void RegKey::Open(NOT_NULL const wchar_t* subkey, REGSAM access)
{
    Open(key_, subkey, access);
}

void RegKey::Open(NOT_NULL HKEY rootkey, NOT_NULL const wchar_t* subkey, REGSAM access)
{
    ENSURE(CHECK, rootkey != nullptr && subkey != nullptr).Require();

    HKEY new_key = nullptr;
    auto rv = RegOpenKeyExW(rootkey, subkey, 0, access, &new_key);
    LOG_IF(WARNING, rv != ERROR_SUCCESS)
        << "Failed to create instance for key " << kbase::WideToUTF8(subkey)
        << "; Error: " << rv;

    Close();

    key_ = new_key;

    if (rv == ERROR_SUCCESS) {
        subkey_name_ = subkey;
    }
}

HKEY RegKey::Get() const noexcept
{
    return key_;
}

HKEY RegKey::Release() noexcept
{
    HKEY key = key_;
    key_ = nullptr;

    return key;
}

void RegKey::Close() noexcept
{
    if (key_) {
        RegCloseKey(key_);
        key_ = nullptr;
    }

    subkey_name_.clear();
}

// static
bool RegKey::KeyExists(HKEY rootkey, const wchar_t* subkey, WOW6432Node node_mode)
{
    REGSAM access = KEY_READ;
    if (node_mode == Force32KeyOnWOW64) {
        access |= KEY_WOW64_32KEY;
    } else if (node_mode == Force64KeyOnWOW64) {
        access |= KEY_WOW64_64KEY;
    }

    HKEY key = nullptr;
    auto rv = RegOpenKeyExW(rootkey, subkey, 0, access, &key);
    if (rv == ERROR_SUCCESS) {
        RegCloseKey(key);
        return true;
    }

    ENSURE(THROW, rv == ERROR_FILE_NOT_FOUND)(rv).Require();

    return false;
}

bool RegKey::HasValue(const wchar_t* value_name) const
{
    ENSURE(THROW, IsValid()).Require();

    auto rv = RegQueryValueExW(key_, value_name, nullptr, nullptr, nullptr, nullptr);
    if (rv == ERROR_SUCCESS) {
        return true;
    }

    ENSURE(THROW, rv == ERROR_FILE_NOT_FOUND)(rv).Require();

    return false;
}

size_t RegKey::GetValueCount() const
{
    ENSURE(THROW, IsValid()).Require();

    DWORD value_count = 0;
    auto rv = RegQueryInfoKeyW(key_, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
                               &value_count, nullptr, nullptr, nullptr, nullptr);

    ENSURE(THROW, rv == ERROR_SUCCESS)(rv).Require();

    return static_cast<size_t>(value_count);
}

void RegKey::GetValueNameAt(size_t index, std::wstring& value_name) const
{
    ENSURE(THROW, IsValid()).Require();

    wchar_t buf[MAX_PATH + 1];
    DWORD buf_size = _countof(buf);
    auto rv = RegEnumValueW(key_, static_cast<DWORD>(index), buf, &buf_size, nullptr, nullptr,
                            nullptr, nullptr);

    ENSURE(THROW, rv == ERROR_SUCCESS)(rv).Require();

    value_name = buf;
}

void RegKey::DeleteKey(const wchar_t* key_name) const
{
    ENSURE(THROW, IsValid()).Require();

    auto rv = RegDeleteTreeW(key_, key_name);
    ENSURE(THROW, rv == ERROR_SUCCESS)(rv).Require();
}

void RegKey::DeleteValue(const wchar_t* value_name) const
{
    ENSURE(THROW, IsValid()).Require();

    auto rv = RegDeleteValueW(key_, value_name);
    ENSURE(THROW, rv == ERROR_SUCCESS)(rv).Require();
}

void RegKey::ReadRawValue(const wchar_t* value_name, void* data, DWORD& data_size,
                          DWORD& data_type) const
{
    ENSURE(THROW, IsValid()).Require();

    auto rv = RegGetValueW(key_, nullptr, value_name, 0, &data_type, data, &data_size);
    ENSURE(THROW, rv == ERROR_SUCCESS)(rv).Require();
}

void RegKey::ReadRawValue(const wchar_t* value_name, DWORD restricted_type, void* data,
                          DWORD& data_size) const
{
    ENSURE(THROW, IsValid()).Require();

    auto rv = RegGetValueW(key_, nullptr, value_name, restricted_type, nullptr, data, &data_size);
    ENSURE(THROW, rv == ERROR_SUCCESS)(rv).Require();
}

void RegKey::ReadValue(const wchar_t* value_name, std::wstring& value) const
{
    ENSURE(THROW, IsValid()).Require();

    constexpr DWORD kCharSize = sizeof(wchar_t);

    // Length including null.
    DWORD str_length = 1024;

    // It seems that automatic expansion for environment strings in RegGetValue
    // behaves incorrect when using std::basic_string as its buffer.
    // Therefore, does expansions on our own.
    DWORD restricted_type = RRF_RT_REG_SZ | RRF_RT_REG_EXPAND_SZ | RRF_NOEXPAND;
    DWORD data_type = 0;
    DWORD data_size = 0;

    std::wstring raw_data;

    long rv = 0;
    do {
        wchar_t* data_ptr = WriteInto(raw_data, str_length);
        data_size = str_length * kCharSize;
        rv = RegGetValueW(key_, nullptr, value_name, restricted_type, &data_type, data_ptr,
                          &data_size);
        if (rv == ERROR_SUCCESS) {
            if (data_type == REG_SZ) {
                size_t written_length = data_size / kCharSize - 1;
                value.assign(data_ptr, written_length);
                return;
            }

            if (data_type == REG_EXPAND_SZ) {
                std::wstring expanded;
                wchar_t* ptr = WriteInto(expanded, str_length);
                DWORD size = ExpandEnvironmentStringsW(data_ptr, ptr, str_length);
                ENSURE(THROW, size > 0)(LastError()).Require();
                if (size > str_length) {
                    data_size = size * kCharSize;
                    rv = ERROR_MORE_DATA;
                } else {
                    value.assign(ptr, size - 1);
                    return;
                }
            }
        }
    } while (rv == ERROR_MORE_DATA && (str_length = data_size / kCharSize, true));

    ENSURE(THROW, NotReached())(rv).Require();
}

void RegKey::ReadValue(const wchar_t* value_name, std::vector<std::wstring>& values) const
{
    ENSURE(THROW, IsValid()).Require();

    constexpr size_t kCharSize = sizeof(wchar_t);
    DWORD restricted_type = RRF_RT_REG_MULTI_SZ;
    DWORD data_size = 0;

    // Acquires the data size, in bytes.
    ReadRawValue(value_name, restricted_type, nullptr, data_size);

    std::wstring raw_data;
    wchar_t* data_ptr = WriteInto(raw_data, data_size / kCharSize);
    ReadRawValue(value_name, restricted_type, data_ptr, data_size);

    SplitString(raw_data, std::wstring(1, L'\0'), values);
}

void RegKey::ReadValue(const wchar_t* value_name, DWORD& value) const
{
    DWORD restricted_type = RRF_RT_DWORD;
    DWORD tmp = 0;
    DWORD data_size = sizeof(DWORD);

    ReadRawValue(value_name, restricted_type, &tmp, data_size);

    ENSURE(CHECK, data_size == sizeof(DWORD))(data_size).Require();

    value = tmp;
}

void RegKey::ReadValue(const wchar_t* value_name, DWORD64& value) const
{
    DWORD restricted_type = RRF_RT_QWORD;
    DWORD64 tmp = 0;
    DWORD data_size = sizeof(DWORD64);

    ReadRawValue(value_name, restricted_type, &tmp, data_size);

    ENSURE(CHECK, data_size == sizeof(DWORD64))(data_size).Require();

    value = tmp;
}

void RegKey::WriteValue(const wchar_t* value_name, DWORD value) const
{
    WriteValue(value_name, &value, sizeof(DWORD), REG_DWORD);
}

void RegKey::WriteValue(const wchar_t* value_name, DWORD64 value) const
{
    WriteValue(value_name, &value, sizeof(DWORD64), REG_QWORD);
}

void RegKey::WriteValue(const wchar_t* value_name, const wchar_t* value, size_t length) const
{
    // Data size includes terminating-null character.
    WriteValue(value_name, value, sizeof(wchar_t) * (length + 1), REG_SZ);
}

void RegKey::WriteValue(const wchar_t* value_name, const void* data, size_t data_size,
                        DWORD data_type) const
{
    ENSURE(CHECK, data && data_size > 0).Require();

    auto rv = RegSetValueExW(key_, value_name, 0, data_type, static_cast<const BYTE*>(data),
                             static_cast<DWORD>(data_size));

    ENSURE(THROW, rv == ERROR_SUCCESS)(rv).Require();
}

// -*- RegKeyIterator::Impl implementations -*-

class RegKeyIterator::Impl {
public:
    // Will take the ownership of `key`.
    Impl(HKEY key, int subkey_count);

    ~Impl();

    DISALLOW_COPY(Impl);

    DISALLOW_MOVE(Impl);

    // Returns true, if successfully advanced to the next key.
    // Returns false, if there is no more key to iterate.
    bool Next();

    const value_type& subkey() const noexcept
    {
        return subkey_;
    }

private:
    HKEY key_;
    int next_index_;
    int subkey_count_;
    value_type subkey_;
};

RegKeyIterator::Impl::Impl(HKEY key, int subkey_count)
    : key_(key), next_index_(0), subkey_count_(subkey_count)
{
    ENSURE(CHECK, key != nullptr && subkey_count > 0)(key)(subkey_count).Require();

    Next();
}

RegKeyIterator::Impl::~Impl()
{
    if (key_) {
        RegCloseKey(key_);
    }
}

bool RegKeyIterator::Impl::Next()
{
    if (next_index_ >= subkey_count_) {
        return false;
    }

    wchar_t name_buf[MAX_PATH + 1];
    DWORD name_buf_size = _countof(name_buf);
    auto rv = RegEnumKeyExW(key_, next_index_, name_buf, &name_buf_size, nullptr, nullptr, nullptr,
                            nullptr);

    ENSURE(THROW, rv == ERROR_SUCCESS)(rv).Require();

    // Be careful, opening some registry keys may fail due to lack of administrator privilege.
    subkey_.Open(key_, name_buf, KEY_READ | KEY_SET_VALUE);

    ++next_index_;

    return true;
}

// -*- RegKeyIterator implementations -*-

RegKeyIterator::RegKeyIterator(HKEY rootkey, const wchar_t* subkey)
{
    HKEY key = nullptr;
    auto rv = RegOpenKeyExW(rootkey, subkey, 0, KEY_READ, &key);
    if (rv != ERROR_SUCCESS) {
        LOG(WARNING) << "Failed to construct RegKeyIterator on " << subkey << "; Error: " << rv;
        return;
    }

    auto guard = MAKE_SCOPE_GUARD { RegCloseKey(key); };

    DWORD subkey_count = 0;
    rv = RegQueryInfoKeyW(key, nullptr, nullptr, nullptr, &subkey_count, nullptr, nullptr, nullptr,
                          nullptr, nullptr, nullptr, nullptr);

    if (rv != ERROR_SUCCESS) {
        LOG(WARNING) << "Failed to query count of subkey for " << subkey << "; Error: " << rv;
        return;
    }

    if (subkey_count > 0) {
        impl_ = std::make_shared<Impl>(key, static_cast<int>(subkey_count));
        guard.Dismiss();
    }
}

RegKeyIterator::RegKeyIterator(const RegKey& regkey)
{
    if (!regkey) {
        return;
    }

    HKEY key = SHRegDuplicateHKey(regkey.Get());
    auto guard = MAKE_SCOPE_GUARD { RegCloseKey(key); };

    DWORD subkey_count = 0;
    auto result = RegQueryInfoKeyW(key, nullptr, nullptr, nullptr, &subkey_count, nullptr, nullptr,
                                   nullptr, nullptr, nullptr, nullptr, nullptr);

    if (result != ERROR_SUCCESS) {
        LOG(WARNING) << "Failed to query count of subkey; Error: " << result;
        return;
    }

    if (subkey_count > 0) {
        impl_ = std::make_shared<Impl>(key, static_cast<int>(subkey_count));
        guard.Dismiss();
    }
}

RegKeyIterator& RegKeyIterator::operator++()
{
    if (!impl_->Next()) {
        impl_ = nullptr;
    }

    return *this;
}

RegKeyIterator::reference RegKeyIterator::operator*() const noexcept
{
    return impl_->subkey();
}

RegKeyIterator::pointer RegKeyIterator::operator->() const noexcept
{
    return &impl_->subkey();
}

}   // namespace kbase
