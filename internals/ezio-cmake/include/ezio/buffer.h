/*
 @ 0xCCCCCCCC
*/

#ifndef EZIO_BUFFER_H_
#define EZIO_BUFFER_H_

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>
#include <sys/types.h>

#include "kbase/basic_macros.h"

namespace ezio {

class Buffer {
private:
    static constexpr size_t kDefaultPrependSize = 8;
    static constexpr size_t kDefaultInitialSize = 1024;

public:
    using value_type = char;

    class Iterator {
    public:
        using iterator_category = std::random_access_iterator_tag;
        using value_type = Buffer::value_type;
        using difference_type = ptrdiff_t;
        using pointer = const value_type*;
        using reference = const value_type&;

        explicit Iterator(pointer ptr) noexcept
            : ptr_(ptr)
        {}

        ~Iterator() = default;

        reference operator*() const noexcept
        {
            return *ptr_;
        }

        pointer operator->() const noexcept
        {
            return ptr_;
        }

        reference operator[](difference_type n) const noexcept
        {
            return *(ptr_ + n);
        }

        Iterator& operator++() noexcept
        {
            ++ptr_;
            return *this;
        }

        Iterator& operator--() noexcept
        {
            --ptr_;
            return *this;
        }

        Iterator& operator+=(difference_type n) noexcept
        {
            ptr_ += n;
            return *this;
        }

        Iterator& operator-=(difference_type n) noexcept
        {
            ptr_ -= n;
            return *this;
        }

        friend difference_type operator-(Iterator lhs, Iterator rhs) noexcept
        {
            return lhs.ptr_ - rhs.ptr_;
        }

        friend bool operator==(Iterator lhs, Iterator rhs) noexcept
        {
            return lhs.ptr_ == rhs.ptr_;
        }

        friend bool operator!=(Iterator lhs, Iterator rhs) noexcept
        {
            return !(lhs == rhs);
        }

    private:
        pointer ptr_;
    };

    using const_iterator = Iterator;
    using iterator = const_iterator;

    Buffer();

    explicit Buffer(size_t initial_size);

    ~Buffer() = default;

    DEFAULT_COPY(Buffer);

    DEFAULT_MOVE(Buffer);

    size_t prependable_size() const noexcept
    {
        return reader_index_;
    }

    size_t readable_size() const noexcept
    {
        return writer_index_ - reader_index_;
    }

    size_t writable_size() const noexcept
    {
        return buf_.size() - writer_index_;
    }

    iterator begin() const noexcept
    {
        return cbegin();
    }

    const_iterator cbegin() const noexcept
    {
        return iterator(buf_.data() + reader_index_);
    }

    iterator end() const noexcept
    {
        return cend();
    }

    const_iterator cend() const noexcept
    {
        return iterator(buf_.data() + writer_index_);
    }

    void Write(const void* data, size_t size);

    void Write(int8_t n);

    void Write(int16_t n);

    void Write(int32_t n);

    void Write(int64_t n);

	void Write(uint8_t n);

	void Write(uint16_t n);

	void Write(uint32_t n);

	void Write(uint64_t n);

	void Write(float n);

    const value_type* Peek() const noexcept
    {
        return &*begin();
    }

    int8_t PeekAsInt8() const;

    int16_t PeekAsInt16() const;

    int32_t PeekAsInt32() const;

    int64_t PeekAsInt64() const;

	uint8_t PeekAsUInt8() const;

	uint16_t PeekAsUInt16() const;

	uint32_t PeekAsUInt32() const;

	uint64_t PeekAsUInt64() const;

	float PeekAsFloat() const;

    void Consume(size_t data_size);

    void ConsumeAll() noexcept
    {
        reader_index_ = kDefaultPrependSize;
        writer_index_ = reader_index_;
    }

    int8_t ReadAsInt8();

    int16_t ReadAsInt16();

    int32_t ReadAsInt32();

    int64_t ReadAsInt64();

	uint8_t ReadAsUInt8();

	uint16_t ReadAsUInt16();

	uint32_t ReadAsUInt32();

	uint64_t ReadAsUInt64();

	float ReadAsFloat();

    std::string ReadAsString(size_t length);

    std::string ReadAllAsString();

    void Prepend(const void* data, size_t size);

	void Prepend(uint16_t n);

    void Prepend(int32_t n);

    void Prepend(int64_t n);

    char* BeginWrite();

    void EndWrite(size_t written_size);

    void ReserveWritable(size_t new_size);

private:
    std::vector<value_type> buf_;
    size_t reader_index_;
    size_t writer_index_;
};

using Iterator = Buffer::Iterator;

inline Iterator operator+(Iterator it, Iterator::difference_type n)
{
    auto temp(it);
    temp += n;
    return temp;
}

inline Iterator operator+(Iterator::difference_type n, Iterator it)
{
    return it + n;
}

inline Iterator operator-(Iterator it, Iterator::difference_type n)
{
    auto temp(it);
    temp -= n;
    return temp;
}

inline bool operator<(Iterator lhs, Iterator rhs)
{
    return (rhs - lhs) > 0;
}

inline bool operator>(Iterator lhs, Iterator rhs)
{
    return rhs < lhs;
}

inline bool operator<=(Iterator lhs, Iterator rhs)
{
    return !(lhs > rhs);
}

inline bool operator>=(Iterator lhs, Iterator rhs)
{
    return !(lhs < rhs);
}

#if defined(OS_POSIX)

ssize_t ReadFDInVec(int fd, Buffer& buf);

#endif

}   // namespace ezio

#endif  // EZIO_BUFFER_H_
