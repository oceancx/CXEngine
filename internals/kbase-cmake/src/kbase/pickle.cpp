/*
 @ 0xCCCCCCCC
*/

#include "kbase/pickle.h"

#include <algorithm>

#include "kbase/secure_c_runtime.h"
#include "kbase/string_util.h"

namespace {

// Rounds up `num` to the nearest multiple of `factor`.
constexpr size_t RoundToMultiple(size_t num, size_t factor) noexcept
{
    return factor == 0 ? 0 : (num - 1 - (num - 1) % factor + factor);
}

// Zeros padding memory; otherwise some memory detectors may complain about
// uninitialized memory.
void SanitizePadding(kbase::byte* padding_begin, size_t padding_size)
{
    if (padding_size != 0) {
        memset(padding_begin, 0, padding_size);
    }
}

}   // namespace

namespace kbase {

PickleReader::PickleReader(const void* pickled_data, size_t size_in_bytes) noexcept
    : read_ptr_(static_cast<const byte*>(pickled_data) + sizeof(Pickle::Header)),
      data_end_(read_ptr_ + size_in_bytes - sizeof(Pickle::Header))
{}

PickleReader::PickleReader(const Pickle& pickle) noexcept
    : read_ptr_(pickle.payload()),
      data_end_(pickle.end_of_payload())
{}

PickleReader& PickleReader::operator>>(std::string& value)
{
    PickleReader& reader = *this;
    size_t length;
    reader >> length;
    if (length != 0) {
        auto* dest = WriteInto(value, length + 1);
        ReadRawData(dest, sizeof(std::string::value_type) * length);
    }

    return reader;
}

PickleReader& PickleReader::operator>>(std::wstring& value)
{
    PickleReader& reader = *this;
    size_t length;
    reader >> length;
    if (length != 0) {
        auto* dest = WriteInto(value, length + 1);
        ReadRawData(dest, sizeof(std::wstring::value_type) * length);
    }

    return reader;
}

void PickleReader::ReadRawData(void* dest, size_t size_in_bytes)
{
    ENSURE(CHECK, !!*this && size_in_bytes != 0).Require();
    SecureMemcpy(dest, size_in_bytes, read_ptr_, size_in_bytes);
    SeekReadPosition(size_in_bytes);
}

template<typename T>
void PickleReader::ReadBuiltIn(T& value)
{
    static_assert(std::is_fundamental<T>::value, "T is not built-in type");
    value = *reinterpret_cast<const T*>(read_ptr_);
    SeekReadPosition(sizeof(T));
}

void PickleReader::SeekReadPosition(size_t data_size) noexcept
{
    size_t rounded_size = RoundToMultiple(data_size, sizeof(uint32_t));
    if (read_ptr_ + rounded_size > data_end_) {
        read_ptr_ += data_size;
    } else {
        read_ptr_ += rounded_size;
    }
}

void PickleReader::SkipData(size_t data_size) noexcept
{
    SeekReadPosition(data_size);
}

// -*- Pickle -*-

Pickle::Pickle()
    : header_(nullptr), capacity_(0)
{
    ResizeCapacity(kCapacityUnit);
    header_->payload_size = 0;
}

Pickle::Pickle(const void* data, size_t size_in_bytes)
    : header_(nullptr), capacity_(0)
{
    ENSURE(CHECK, data != nullptr && size_in_bytes > 0).Require();
    ResizeCapacity(size_in_bytes);
    SecureMemcpy(header_, capacity_, data, size_in_bytes);
}

Pickle::Pickle(const Pickle& other)
    : header_(nullptr), capacity_(0)
{
    ResizeCapacity(other.size());
    SecureMemcpy(header_, capacity_, other.header_, other.size());
}

Pickle::Pickle(Pickle&& other) noexcept
    : header_(other.header_), capacity_(other.capacity_)
{
    other.header_ = nullptr;
    other.capacity_ = 0;
}

Pickle::~Pickle()
{
    // Technically, only pickles having been moved have null header.
    if (header_ != nullptr) {
        free(header_);
    }
}

Pickle& Pickle::operator=(const Pickle& rhs)
{
    if (this != &rhs) {
        if (capacity_ < rhs.size()) {
            ResizeCapacity(rhs.size());
        }

        SecureMemcpy(header_, capacity_, rhs.header_, rhs.size());
    }

    return *this;
}

Pickle& Pickle::operator=(Pickle&& rhs) noexcept
{
    if (this != &rhs) {
        Header* old_header = header_;

        header_ = rhs.header_;
        capacity_ = rhs.capacity_;
        rhs.header_ = nullptr;
        rhs.capacity_ = 0;

        free(old_header);
    }

    return *this;
}

void Pickle::ResizeCapacity(size_t new_capacity)
{
    ENSURE(CHECK, new_capacity > capacity_).Require();

    new_capacity = RoundToMultiple(new_capacity, kCapacityUnit);
    void* ptr = realloc(header_, new_capacity);

    ENSURE(THROW, ptr != nullptr).Require("Failed to realloc a new memory block!");
    header_ = static_cast<Header*>(ptr);
    capacity_ = new_capacity;
}

Pickle& Pickle::operator<<(const std::string& value)
{
    Pickle& pickle = *this;
    auto length = value.length();
    pickle << length;
    if (length != 0) {
        Write(value.data(), length * sizeof(char));
    }

    return pickle;
}

Pickle& Pickle::operator<<(const std::wstring& value)
{
    Pickle& pickle = *this;
    auto length = value.length();
    pickle << length;
    if (length != 0) {
        Write(value.data(), length * sizeof(wchar_t));
    }

    return pickle;
}

void Pickle::Write(const void* data, size_t size_in_bytes)
{
    ENSURE(CHECK, size_in_bytes != 0).Require();
    size_t last_payload_size = payload_size();
    byte* dest = SeekWritePosition(size_in_bytes);
    size_t free_buf_size = capacity_ - (dest - reinterpret_cast<byte*>(header_));
    SecureMemcpy(dest, free_buf_size, data, size_in_bytes);
    size_t padding_size = payload_size() - last_payload_size - size_in_bytes;
    SanitizePadding(dest - padding_size, padding_size);
}

template<typename T>
void Pickle::WriteBuiltIn(T value)
{
    static_assert(std::is_fundamental<T>::value, "T is not built-in type");
    size_t last_payload_size = payload_size();
    constexpr size_t size_in_bytes = sizeof(T);
    byte* dest = SeekWritePosition(size_in_bytes);
    *reinterpret_cast<T*>(dest) = value;
    size_t padding_size = payload_size() - last_payload_size - size_in_bytes;
    SanitizePadding(dest - padding_size, padding_size);
}

byte* Pickle::SeekWritePosition(size_t length)
{
    // Writing starts at a uint32-aligned offset.
    size_t offset = RoundToMultiple(header_->payload_size, sizeof(uint32_t));
    size_t required_size = offset + length;
    size_t required_total_size = required_size + sizeof(Header);

    if (required_total_size > capacity_) {
        ResizeCapacity(std::max(capacity_ << 1, required_total_size));
    }

    ENSURE(CHECK, required_size <= std::numeric_limits<uint32_t>::max())(required_size).Require();
    header_->payload_size = static_cast<uint32_t>(required_size);

    return mutable_payload() + offset;
}

// Explicit instantiation.

template void PickleReader::ReadBuiltIn(bool&);
template void PickleReader::ReadBuiltIn(int8_t&);
template void PickleReader::ReadBuiltIn(uint8_t&);
template void PickleReader::ReadBuiltIn(short&);
template void PickleReader::ReadBuiltIn(unsigned short&);
template void PickleReader::ReadBuiltIn(int&);
template void PickleReader::ReadBuiltIn(unsigned int&);
template void PickleReader::ReadBuiltIn(int64_t&);
template void PickleReader::ReadBuiltIn(uint64_t&);
template void PickleReader::ReadBuiltIn(float&);
template void PickleReader::ReadBuiltIn(double&);

template void Pickle::WriteBuiltIn(bool);
template void Pickle::WriteBuiltIn(int8_t);
template void Pickle::WriteBuiltIn(uint8_t);
template void Pickle::WriteBuiltIn(short);
template void Pickle::WriteBuiltIn(unsigned short);
template void Pickle::WriteBuiltIn(int);
template void Pickle::WriteBuiltIn(unsigned int);
template void Pickle::WriteBuiltIn(int64_t);
template void Pickle::WriteBuiltIn(uint64_t);
template void Pickle::WriteBuiltIn(float);
template void Pickle::WriteBuiltIn(double);

}   // namespace kbase
