/*
 @ 0xCCCCCCCC
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef KBASE_LRU_CACHE_H_
#define KBASE_LRU_CACHE_H_

#include <list>
#include <map>
#include <unordered_map>

#include "kbase/basic_macros.h"
#include "kbase/error_exception_util.h"

namespace kbase {

// Standardize type argument signatures of std::map and std::unordered_map.

template<typename Key, typename Value>
struct TreeMap {
    using MapType = std::map<Key, Value>;
};

template<typename Key, typename Value>
struct HashMap {
    using MapType = std::unordered_map<Key, Value>;
};

// A cache container that allows O(logn)-time, i.e. TreeMap-based implementation,
// or O(1)-time, i.e. HashMap-based implementation, access to entries using a key.
// If auto eviction is enabled, LRU-replacement algorithm would be employed when
// the cache runs out its free storage.
template<typename Key, typename Entry, template<typename, typename> class Map = TreeMap>
class LRUCache {
public:
    using key_type = Key;
    using value_type = std::pair<const Key, Entry>;

private:
    using CachedEntryList = std::list<value_type>;
    using KeyTable = typename Map<Key, typename CachedEntryList::iterator>::MapType;

public:
    using size_type = size_t;
    using iterator = typename CachedEntryList::iterator;
    using const_iterator = typename CachedEntryList::const_iterator;
    using reverse_iterator = typename CachedEntryList::reverse_iterator;
    using const_reverse_iterator = typename CachedEntryList::const_reverse_iterator;

    enum : size_type {
        NoAutoEvict = 0
    };

    explicit LRUCache(size_type max_size) noexcept(
        std::is_nothrow_default_constructible<CachedEntryList>::value &&
        std::is_nothrow_default_constructible<KeyTable>::value)
        : max_size_(max_size)
    {}

    LRUCache(LRUCache&& other) noexcept(
        std::is_nothrow_move_constructible<CachedEntryList>::value &&
        std::is_nothrow_move_constructible<KeyTable>::value)
        : max_size_(other.max_size_),
          entry_ordering_list_(std::move(other.entry_ordering_list_)),
          key_table_(std::move(other.key_table_))
    {}

    LRUCache& operator=(LRUCache&& rhs) noexcept(
        std::is_nothrow_move_assignable<CachedEntryList>::value &&
        std::is_nothrow_move_assignable<KeyTable>::value)
    {
        if (this != &rhs) {
            entry_ordering_list_ = std::move(rhs.entry_ordering_list_);
            key_table_ = std::move(rhs.key_table_);
            // Work-around for assigning to a const variable.
            size_type* new_max_size = const_cast<size_type*>(&max_size_);
            *new_max_size = rhs.max_size_;
        }

        return *this;
    }

    ~LRUCache() = default;

    DISALLOW_COPY(LRUCache);

    // Add a pair of <key, entry> into the cache. If the key already exists, update
    // the entry.
    // If auto-eviction is enabled for the cache, and also cache runs out its free
    // storage, then LRU replacement algorithm is employed when caching into new
    // entry.

    iterator Put(const Key& key, const Entry& entry)
    {
        return PutInternal(key, entry);
    }

    iterator Put(const Key& key, Entry&& entry)
    {
        return PutInternal(key, std::move(entry));
    }

    // Returns the iterator to the value associated with `key`.
    // Returns end() if no matched value was found.
    // Access to a cached entry marks this entry as recently used by moving it to
    // the tail of cached entry list.
    iterator Get(const Key& key)
    {
        auto key_it = key_table_.find(key);
        if (key_it == key_table_.end()) {
            return end();
        }

        auto entry_it = key_it->second;
        entry_ordering_list_.splice(entry_ordering_list_.end(), entry_ordering_list_, entry_it);

        return entry_it;
    }

    // Returns the iterator to the value associated with the `key`.
    // Returns end() if no such value was found.
    // These two functions does not touch the entry, i.e. will not mark the entry recently used.

    const_iterator find(const Key& key) const
    {
        auto key_it = key_table_.find(key);
        if (key_it == key_table_.end()) {
            return end();
        }

        return key_it->second;
    }

    iterator find(const Key& key)
    {
        auto key_it = key_table_.find(key);
        if (key_it == key_table_.end()) {
            return end();
        }

        return key_it->second;
    }

    // Erases the value with specific iterator, and returns the iterator to
    // the next value.
    iterator erase(const_iterator pos)
    {
        key_table_.erase(pos->first);
        return entry_ordering_list_.erase(pos);
    }

    // Evict a single entry, or |count_to_evict| entries from cache.

    void Evict()
    {
        erase(entry_ordering_list_.begin());
    }

    void Evict(size_type count_to_evict)
    {
        ENSURE(CHECK, count_to_evict <= size())(count_to_evict)(size()).Require();
        for (size_type i = 0; i < count_to_evict; ++i) {
            Evict();
        }
    }

    size_type size() const
    {
        return entry_ordering_list_.size();
    }

    bool empty() const
    {
        return entry_ordering_list_.empty();
    }

    size_type max_size() const
    {
        return max_size_;
    }

    bool auto_evict() const
    {
        return max_size_ != 0;
    }

    iterator begin() { return entry_ordering_list_.begin(); }

    const_iterator begin() const { return entry_ordering_list_.begin(); }

    const_iterator cbegin() const { return entry_ordering_list_.cbegin(); }

    iterator end() { return entry_ordering_list_.end(); }

    const_iterator end() const { return entry_ordering_list_.end(); }

    const_iterator cend() const { return entry_ordering_list_.cend(); }

private:
    template<typename KeyType, typename EntryType>
    iterator PutInternal(const KeyType& key, EntryType&& entry)
    {
        auto key_it = key_table_.find(key);
        if (key_it != key_table_.end()) {
            auto entry_it = key_it->second;
            entry_it->second = std::forward<EntryType>(entry);
            entry_ordering_list_.splice(entry_ordering_list_.end(), entry_ordering_list_, entry_it);
            return entry_it;
        }

        if (auto_evict() && max_size() == size()) {
            Evict();
        }

        entry_ordering_list_.push_back({key, std::forward<EntryType>(entry)});
        auto rv = key_table_.insert({key, std::prev(entry_ordering_list_.end())});

        return rv.first->second;
    }

private:
    const size_type max_size_;
    CachedEntryList entry_ordering_list_;
    KeyTable key_table_;
};

}   // namespace kbase

#endif  // KBASE_LRU_CACHE_H_
