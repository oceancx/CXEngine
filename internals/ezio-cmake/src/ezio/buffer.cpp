/*
 @ 0xCCCCCCCC
*/

#include "ezio/buffer.h"

#include <cstring>

#include "kbase/error_exception_util.h"

#include "ezio/endian_utils.h"

namespace ezio {

Buffer::Buffer()
    : Buffer(kDefaultInitialSize)
{}

Buffer::Buffer(size_t initial_size)
    : buf_(initial_size + kDefaultPrependSize),
      reader_index_(kDefaultPrependSize),
      writer_index_(kDefaultPrependSize)
{}

void Buffer::Write(const void* data, size_t size)
{
    ReserveWritable(size);
    ENSURE(CHECK, writable_size() >= size)(writable_size())(size).Require();

    memcpy(BeginWrite(), data, size);
    EndWrite(size);
}

void Buffer::Write(int8_t n)
{
    Write(&n, sizeof(n));
}

void Buffer::Write(int16_t n)
{
    auto be = HostToNetwork(n);
    Write(&be, sizeof(be));
}

void Buffer::Write(int32_t n)
{
    auto be = HostToNetwork(n);
    Write(&be, sizeof(be));
}

void Buffer::Write(int64_t n)
{
    auto be = HostToNetwork(n);
    Write(&be, sizeof(be));
}

void Buffer::Write(uint8_t n)
{
	auto be = HostToNetwork(n);
	Write(&be, sizeof(be));
}

void Buffer::Write(uint16_t n)
{
	auto be = HostToNetwork(n);
	Write(&be, sizeof(be));
}

void Buffer::Write(uint32_t n)
{
	auto be = HostToNetwork(n);
	Write(&be, sizeof(be));
}

void Buffer::Write(uint64_t n)
{
	auto be = HostToNetwork(n);
	Write(&be, sizeof(be));
}

void Buffer::Write(float n)
{
	auto be = HostToNetwork(n);
	Write(&be, sizeof(be));
}

int8_t Buffer::PeekAsInt8() const
{
    ENSURE(CHECK, readable_size() >= sizeof(int8_t))(readable_size()).Require();
    int8_t n = *Peek();
    return n;
}

int16_t Buffer::PeekAsInt16() const
{
    ENSURE(CHECK, readable_size() >= sizeof(int16_t))(readable_size()).Require();
    int16_t be;
    memcpy(&be, Peek(), sizeof(be));
    return NetworkToHost(be);
}

int32_t Buffer::PeekAsInt32() const
{
    ENSURE(CHECK, readable_size() >= sizeof(int32_t))(readable_size()).Require();
    int32_t be;
    memcpy(&be, Peek(), sizeof(be));
    return NetworkToHost(be);
}

int64_t Buffer::PeekAsInt64() const
{
    ENSURE(CHECK, readable_size() >= sizeof(int64_t))(readable_size()).Require();
    int64_t be;
    memcpy(&be, Peek(), sizeof(be));
    return NetworkToHost(be);
}

uint8_t Buffer::PeekAsUInt8() const
{
	ENSURE(CHECK, readable_size() >= sizeof(uint8_t))(readable_size()).Require();
	uint8_t be;
	memcpy(&be, Peek(), sizeof(be));
	return NetworkToHost(be);
}

uint16_t Buffer::PeekAsUInt16() const
{
	ENSURE(CHECK, readable_size() >= sizeof(int16_t))(readable_size()).Require();
	int16_t be;
	memcpy(&be, Peek(), sizeof(be));
	return NetworkToHost(be);
}

uint32_t Buffer::PeekAsUInt32() const
{
	ENSURE(CHECK, readable_size() >= sizeof(int32_t))(readable_size()).Require();
	int32_t be;
	memcpy(&be, Peek(), sizeof(be));
	return NetworkToHost(be);
}

uint64_t Buffer::PeekAsUInt64() const
{
	ENSURE(CHECK, readable_size() >= sizeof(int64_t))(readable_size()).Require();
	int64_t be;
	memcpy(&be, Peek(), sizeof(be));
	return NetworkToHost(be);
}

float Buffer::PeekAsFloat() const
{
	ENSURE(CHECK, readable_size() >= sizeof(float))(readable_size()).Require();
	float be;
	memcpy(&be, Peek(), sizeof(be));
	return NetworkToHost(be);
}

void Buffer::Consume(size_t data_size)
{
    ENSURE(CHECK, data_size <= readable_size())(data_size)(readable_size()).Require();
    if (data_size < readable_size()) {
        reader_index_ += data_size;
    } else {
        ConsumeAll();
    }
}

int8_t Buffer::ReadAsInt8()
{
    auto n = PeekAsInt8();
    Consume(sizeof(n));
    return n;
}

int16_t Buffer::ReadAsInt16()
{
    auto n = PeekAsInt16();
    Consume(sizeof(n));
    return n;
}

int32_t Buffer::ReadAsInt32()
{
    auto n = PeekAsInt32();
    Consume(sizeof(n));
    return n;
}

int64_t Buffer::ReadAsInt64()
{
    auto n = PeekAsInt64();
    Consume(sizeof(n));
    return n;
}

uint8_t Buffer::ReadAsUInt8()
{
	auto n = PeekAsUInt8();
	Consume(sizeof(n));
	return n;
}

uint16_t Buffer::ReadAsUInt16()
{
	auto n = PeekAsUInt16();
	Consume(sizeof(n));
	return n;
}

uint32_t Buffer::ReadAsUInt32()
{
	auto n = PeekAsUInt32();
	Consume(sizeof(n));
	return n;
}

uint64_t Buffer::ReadAsUInt64()
{
	auto n = PeekAsUInt64();
	Consume(sizeof(n));
	return n;
}

float Buffer::ReadAsFloat()
{
	auto n = PeekAsFloat();
	Consume(sizeof(n));
	return n;
}

std::string Buffer::ReadAsString(size_t length)
{
    ENSURE(CHECK, readable_size() >= length)(readable_size())(length).Require();
    auto b = begin();
    std::string s(b, b + length);
    Consume(length);
    return s;
}

std::string Buffer::ReadAllAsString()
{
    std::string s(begin(), end());
    ConsumeAll();
    return s;
}

void Buffer::Prepend(const void* data, size_t size)
{
    ENSURE(CHECK, prependable_size() >= size)(prependable_size())(size).Require();
    auto start = reader_index_ - size;
    memcpy(buf_.data() + start, data, size);
    reader_index_ -= size;
}

void Buffer::Prepend(int32_t n)
{
    auto be = HostToNetwork(n);
    Prepend(&be, sizeof(be));
}

void Buffer::Prepend(int64_t n)
{
    auto be = HostToNetwork(n);
    Prepend(&be, sizeof(be));
}

void Buffer::Prepend(uint16_t n)
{
	auto be = HostToNetwork(n);
	Prepend(&be, sizeof(be));
}

void Buffer::ReserveWritable(size_t new_size)
{
    if (writable_size() >= new_size) {
        return;
    }

    if (prependable_size() + writable_size() < kDefaultPrependSize + new_size) {
        buf_.resize(writer_index_ + new_size);
    } else {
        // Ranges may overlap.
        auto data_size = readable_size();
        memmove(buf_.data() + kDefaultPrependSize, buf_.data() + reader_index_, data_size);
        reader_index_ = kDefaultPrependSize;
        writer_index_ = reader_index_ + data_size;
    }
}

char* Buffer::BeginWrite()
{
    return buf_.data() + writer_index_;
}

void Buffer::EndWrite(size_t written_size)
{
    ENSURE(CHECK, writable_size() >= written_size).Require();
    writer_index_ += written_size;
}

}   // namespace ezio
