/*
 @ 0xCCCCCCCC
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef KBASE_STRING_VIEW_H_
#define KBASE_STRING_VIEW_H_

#include <algorithm>
#include <stdexcept>

#include "kbase/basic_macros.h"

#if defined(OS_POSIX)
#include <cstddef>
#include <string>
#endif

namespace kbase {

namespace internal {

// FNV-1a hash function. Shamelessly stolen from STL.
inline size_t HashByteSequence(const unsigned char* data, size_t length) noexcept
{
#if defined(_WIN64) || defined(OS_POSIX)
    static_assert(sizeof(size_t) == 8, "This code is for 64-bit size_t.");
    constexpr size_t kFNVOffsetBasis = 14695981039346656037ULL;
    constexpr size_t kFNVPrime = 1099511628211ULL;
#else
    static_assert(sizeof(size_t) == 4, "This code is for 32-bit size_t.");
    constexpr size_t kFNVOffsetBasis = 2166136261U;
    constexpr size_t kFNVPrime = 16777619U;
#endif

    size_t val = kFNVOffsetBasis;
    for (size_t next = 0; next < length; ++next) {
        // fold in another byte
        val ^= static_cast<size_t>(data[next]);
        val *= kFNVPrime;
    }

    return val;
}

// Tries to compute the length of a string constant at compile time.
template<typename CharT>
constexpr size_t StringLength(const CharT* str)
{
    return *str ? StringLength(str + 1) + 1 : 0;
}

}   // namespace internal

template<typename CharT, typename Traits = std::char_traits<CharT>>
class BasicStringView {
public:
    using traits_type = Traits;
    using value_type = CharT;
    using pointer = const CharT*;
    using const_pointer = const CharT*;
    using reference = const CharT&;
    using const_reference = const CharT&;
    using const_iterator = const_pointer;
    using iterator = const_iterator;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;
    using reverse_iterator = const_reverse_iterator;
    using size_type = size_t;
    using difference_type = ptrdiff_t;

    static constexpr size_type npos = static_cast<size_type>(-1);

    constexpr BasicStringView() noexcept
        : data_(nullptr), length_(0)
    {}

    // Supports implicit conversion.
    BasicStringView(const std::basic_string<CharT>& str) noexcept
        : data_(str.data()), length_(str.length())
    {}

    constexpr BasicStringView(const CharT* str, size_type count) noexcept
        : data_(str), length_(count)
    {}

    // Supports implicit conversion.
    constexpr BasicStringView(const CharT* str)
        : data_(str), length_(internal::StringLength(str))
    {}

    ~BasicStringView() = default;

    DEFAULT_COPY(BasicStringView);

    constexpr const_iterator begin() const noexcept
    {
        return cbegin();
    }

    constexpr const_iterator cbegin() const noexcept
    {
        return data_;
    }

    constexpr const_iterator end() const noexcept
    {
        return cend();
    }

    constexpr const_iterator cend() const noexcept
    {
        return data_ + length_;
    }

    constexpr const_reverse_iterator rbegin() const noexcept
    {
        return crbegin();
    }

    constexpr const_reverse_iterator crbegin() const noexcept
    {
        return const_reverse_iterator(cend());
    }

    constexpr const_reverse_iterator rend() const noexcept
    {
        return crend();
    }

    constexpr const_reverse_iterator crend() const noexcept
    {
        return const_reverse_iterator(cbegin());
    }

    // The behavior is undefined if `pos` >= size().
    constexpr const_reference operator[](size_type pos) const
    {
        return data_[pos];
    }

    constexpr const_reference at(size_type pos) const
    {
        if (pos >= length_) {
            throw std::out_of_range("BasicStringView::at");
        }

        return data_[pos];
    }

    // The behavior is undefined if empty() is true.
    constexpr const_reference front() const
    {
        return data_[0];
    }

    // The behavior is undefined if empty() is true.
    constexpr const_reference back() const
    {
        return data_[length_-1];
    }

    // The underlying buffer `data_` points to may not be null-terminated.
    constexpr const_pointer data() const noexcept
    {
        return data_;
    }

    constexpr bool empty() const noexcept
    {
        return length_ == 0;
    }

    constexpr size_type length() const noexcept
    {
        return length_;
    }

    constexpr size_type size() const noexcept
    {
        return length_;
    }

    constexpr size_type max_size() const noexcept
    {
        FORCE_AS_MEMBER_FUNCTION();
        return std::numeric_limits<size_type>::max();
    }

    // The behavior is undefined if `n` > size().
    void RemovePrefix(size_type n)
    {
        data_ += n;
        length_ -= n;
    }

    // The behavior is undefined if `n` > size().
    void RemoveSuffix(size_type n)
    {
        length_ -= n;
    }

    void swap(BasicStringView& other) noexcept
    {
        using std::swap;
        swap(data_, other.data_);
        swap(length_, other.length_);
    }

    std::basic_string<CharT, Traits> ToString() const
    {
        return decltype(ToString())(begin(), end());
    }

    // The behavior is undefined if `dest` is within the range of current view.
    size_type copy(CharT* dest, size_type count, size_type pos = 0) const
    {
        if (pos >= length()) {
            throw std::out_of_range("BasicStringView::copy");
        }

        size_type real_count = std::min(count, length() - pos);
        Traits::copy(dest, data() + pos, real_count);

        return real_count;
    }

    constexpr BasicStringView substr(size_type pos, size_type count = npos) const
    {
        if (pos > length()) {
            throw std::out_of_range("BasicStringView::substr");
        }

        return BasicStringView(data() + pos, std::min(count, length() - pos));
    }

    int compare(BasicStringView view) const noexcept
    {
        size_type real_length = std::min(length(), view.length());
        int rv = Traits::compare(data(), view.data(), real_length);
        if (rv == 0) {
            rv = length() == view.length() ? 0 : (length() < view.length() ? -1 : 1);
        }

        return rv;
    }

    int compare(size_type pos, size_type count, BasicStringView view) const
    {
        return substr(pos, count).compare(view);
    }

    int compare(size_type pos_1, size_type count_1, BasicStringView view,
                          size_type pos_2, size_type count_2) const
    {
        return substr(pos_1, count_1).compare(view.substr(pos_2, count_2));
    }

    int compare(const CharT* str) const
    {
        return compare(BasicStringView(str));
    }

    int compare(size_type pos, size_type count, const CharT* str) const
    {
        return substr(pos, count).compare(str);
    }

    int compare(size_type pos_1, size_type count_1, const CharT* str,
                          size_type count_2) const
    {
        return substr(pos_1, count_1).compare(BasicStringView(str, count_2));
    }

    // Make these two operators friend to force appling cast during template deduction.

    friend constexpr bool operator==(BasicStringView lhs, BasicStringView rhs) noexcept
    {
        return lhs.length() == rhs.length() && lhs.compare(rhs) == 0;
    }

    friend constexpr bool operator!=(BasicStringView lhs, BasicStringView rhs) noexcept
    {
        return lhs.length() != rhs.length() || lhs.compare(rhs) != 0;
    }

    size_type find(BasicStringView view, size_type pos = 0) const noexcept
    {
        if (length() < view.length() + pos) {
            return npos;
        }

        auto it = std::search(begin() + pos, end(), view.begin(), view.end(), Traits::eq);
        if (it == end()) {
            return npos;
        }

        return static_cast<size_type>(std::distance(begin(), it));
    }

    size_type find(CharT ch, size_type pos = 0) const noexcept
    {
        return find(BasicStringView(&ch, 1), pos);
    }

    size_type find(const CharT* str, size_type pos, size_type count) const noexcept
    {
        return find(BasicStringView(str, count), pos);
    }

    size_type find(const CharT* str, size_type pos = 0) const
    {
        return find(BasicStringView(str), pos);
    }

    size_type rfind(BasicStringView view, size_type pos = npos) const noexcept
    {
        if (empty() || view.empty()) {
            return npos;
        }

        // Forms the range [0, real_pos+1)
        size_type real_pos = std::min(pos, length() - 1);
        if (real_pos < view.length() - 1) {
            return npos;
        }

        auto last = std::next(begin(), real_pos + 1);
        auto it = std::find_end(begin(), last, view.begin(), view.end(), Traits::eq);
        if (it == last) {
            return npos;
        }

        return static_cast<size_type>(std::distance(begin(), it));
    }

    size_type rfind(CharT ch, size_type pos = npos) const noexcept
    {
        return rfind(BasicStringView(&ch, 1), pos);
    }

    size_type rfind(const CharT* str, size_type pos, size_type count) const noexcept
    {
        return rfind(BasicStringView(str, count), pos);
    }

    size_type rfind(const CharT* str, size_type pos = npos) const
    {
        return rfind(BasicStringView(str), pos);
    }

    size_type find_first_of(BasicStringView view, size_type pos = 0) const noexcept
    {
        if (empty() || view.empty() || pos >= length()) {
            return npos;
        }

        auto it = std::find_first_of(begin() + pos, end(), view.begin(), view.end(), Traits::eq);
        if (it == end()) {
            return npos;
        }

        return static_cast<size_type>(std::distance(begin(), it));
    }

    size_type find_first_of(CharT ch, size_type pos = 0) const noexcept
    {
        return find_first_of(BasicStringView(&ch, 1), pos);
    }

    size_type find_first_of(const CharT* str, size_type pos, size_type count) const noexcept
    {
        return find_first_of(BasicStringView(str, count), pos);
    }

    size_type find_first_of(const CharT* str, size_type pos = 0) const
    {
        return find_first_of(BasicStringView(str), pos);
    }

    size_type find_last_of(BasicStringView view, size_type pos = npos) const noexcept
    {
        if (empty() || view.empty()) {
            return npos;
        }

        size_type real_pos = std::min(pos, length() - 1);
        for (auto i = real_pos;; --i) {
            if (std::any_of(view.begin(), view.end(), [this, i](CharT ch) {
                                return Traits::eq(data_[i], ch);
                            })) {
                return i;
            }

            if (i == 0) {
                break;
            }
        }

        return npos;
    }

    size_type find_last_of(CharT ch, size_type pos = npos) const noexcept
    {
        return find_last_of(BasicStringView(&ch, 1), pos);
    }

    size_type find_last_of(const CharT* str, size_type pos, size_type count) const noexcept
    {
        return find_last_of(BasicStringView(str, count), pos);
    }

    size_type find_last_of(const CharT* str, size_type pos = npos) const
    {
        return find_last_of(BasicStringView(str), pos);
    }

    size_type find_first_not_of(BasicStringView view, size_type pos = 0) const noexcept
    {
        if (empty() || view.empty() || pos >= length()) {
            return npos;
        }

        for (auto i = pos; i < length(); ++i) {
            if (std::none_of(view.begin(), view.end(), [this, i](CharT ch) {
                                 return Traits::eq(data_[i], ch);
                             })) {
                return i;
            }
        }

        return npos;
    }

    size_type find_first_not_of(CharT ch, size_type pos = 0) const noexcept
    {
        return find_fisrt_not_of(BasicStringView(&ch, 1), pos);
    }

    size_type find_first_not_of(const CharT* str, size_type pos, size_type count) const noexcept
    {
        return find_first_not_of(BasicStringView(str, count), pos);
    }

    size_type find_first_not_of(const CharT* str, size_type pos = 0) const
    {
        return find_first_not_of(BasicStringView(str), pos);
    }

    size_type find_last_not_of(BasicStringView view, size_type pos = npos) const noexcept
    {
        if (empty() || view.empty()) {
            return npos;
        }

        size_type real_pos = std::min(pos, length() - 1);
        for (auto i = real_pos;; --i) {
            if (std::none_of(view.begin(), view.end(), [this, i](CharT ch) {
                                 return Traits::eq(data_[i], ch);
                             })) {
                return i;
            }

            if (i == 0) {
                break;
            }
        }

        return npos;
    }

    size_type find_last_not_of(CharT ch, size_type pos = npos) const noexcept
    {
        return find_last_not_of(BasicStringView(&ch, 1), pos);
    }

    size_type find_last_not_of(const CharT* str, size_type pos, size_type count) const noexcept
    {
        return find_last_not_of(BasicStringView(str, count), pos);
    }

    size_type find_last_not_of(const CharT* str, size_type pos = npos) const
    {
        return find_last_not_of(BasicStringView(str), pos);
    }

private:
    const value_type* data_;
    size_type length_;
};

template<typename CharT, typename Traits>
bool operator<(BasicStringView<CharT, Traits> lhs, BasicStringView<CharT, Traits> rhs) noexcept
{
    return lhs.compare(rhs) < 0;
}

template<typename CharT, typename Traits>
bool operator<=(BasicStringView<CharT, Traits> lhs, BasicStringView<CharT, Traits> rhs) noexcept
{
    return lhs.compare(rhs) <= 0;
}

template<typename CharT, typename Traits>
bool operator>(BasicStringView<CharT, Traits> lhs, BasicStringView<CharT, Traits> rhs) noexcept
{
    return lhs.compare(rhs) > 0;
}

template<typename CharT, typename Traits>
bool operator>=(BasicStringView<CharT, Traits> lhs, BasicStringView<CharT, Traits> rhs) noexcept
{
    return lhs.compare(rhs) >= 0;
}

template<typename CharT, typename Traits>
std::basic_ostream<CharT, Traits>& operator<<(std::basic_ostream<CharT, Traits>& os,
                                              BasicStringView<CharT, Traits> view)
{
    os.write(view.data(), view.length());
    return os;
}

template<typename CharT, typename Traits>
constexpr typename BasicStringView<CharT, Traits>::size_type BasicStringView<CharT, Traits>::npos;

using StringView = BasicStringView<char>;
using WStringView = BasicStringView<wchar_t>;

}   // namespace kbase

// Specializations of std::hash.

namespace std {

template<>
struct hash<kbase::StringView> {
    size_t operator()(kbase::StringView view) const
    {
        auto src = reinterpret_cast<const unsigned char*>(view.data());
        auto length = view.length() * sizeof(decltype(view)::value_type);

        return kbase::internal::HashByteSequence(src, length);
    }
};

template<>
struct hash<kbase::WStringView> {
    size_t operator()(kbase::WStringView view) const
    {
        auto src = reinterpret_cast<const unsigned char*>(view.data());
        auto length = view.length() * sizeof(decltype(view)::value_type);

        return kbase::internal::HashByteSequence(src, length);
    }
};

}   // namespace std

#endif  // KBASE_STRING_VIEW_H_
