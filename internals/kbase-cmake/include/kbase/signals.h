/*
 @ 0xCCCCCCCC
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef KBASE_SIGNALS_H_
#define KBASE_SIGNALS_H_

#include <algorithm>
#include <functional>
#include <memory>
#include <mutex>
#include <vector>

#include "kbase/basic_macros.h"

namespace kbase {

namespace internal {

// This class/interface makes slots able to disconnect from their associated signal.
class IDisposable {
public:
    virtual ~IDisposable() = default;
    virtual void Dispose() = 0;
};

template<typename Func, typename... Args>
class SignalImpl;

template<typename Func, typename... Args>
struct SlotImpl : std::enable_shared_from_this<SlotImpl<Func, Args...>>, IDisposable {
    using SignalType = SignalImpl<Func, Args...>;

    SlotImpl(const std::shared_ptr<SignalType>& signal_source, Func&& func)
        : fn(std::move(func)), source(signal_source), weakly_bound(false)
    {}

    SlotImpl(const std::shared_ptr<SignalType>& signal_source, Func&& func,
             const std::shared_ptr<void>& object)
        : fn(std::move(func)), source(signal_source),
          weakly_bound_object(object), weakly_bound(true)
    {}

    ~SlotImpl() = default;

    DISALLOW_COPY(SlotImpl);

    DISALLOW_MOVE(SlotImpl);

    void Dispose() override
    {
        auto signal_source = source.lock();
        if (!signal_source) {
            return;
        }

        signal_source->RemoveSlot(this->shared_from_this());
    }

    Func fn;
    std::weak_ptr<SignalType> source;
    std::weak_ptr<void> weakly_bound_object;
    bool weakly_bound;
};

template<typename Func, typename... Args>
class SignalImpl {
private:
    using SlotType = SlotImpl<Func, Args...>;
    using SlotList = std::vector<std::shared_ptr<SlotType>>;

public:
    SignalImpl()
        : slots_(std::make_shared<SlotList>())
    {}

    ~SignalImpl() = default;

    DISALLOW_COPY(SignalImpl);

    DISALLOW_MOVE(SignalImpl);

    void AddSlot(const std::shared_ptr<SlotType>& slot)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        CopyIfNeeded();
        slots_->push_back(slot);
    }

    void Invoke(Args... args)
    {
        std::shared_ptr<SlotList> slots;
        {
            std::lock_guard<std::mutex> lock(mutex_);
            slots = slots_;
        }

        for (const auto& slot : *slots) {
            if (slot->weakly_bound) {
                auto bound_object = slot->weakly_bound_object.lock();
                if (!bound_object) {
                    // The bound object that member function relies on has gone.
                    continue;
                }
            }

            slot->fn(std::forward<Args>(args)...);
        }
    }

    void RemoveSlot(const std::shared_ptr<SlotType>& slot)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        CopyIfNeeded();
        SlotList& slots(*slots_);
        auto it = std::find_if(slots.begin(), slots.end(), [&slot](const auto& other_slot) {
            return !slot.owner_before(other_slot) && !other_slot.owner_before(slot);
        });
        if (it != slots.end()) {
            slots.erase(it);
        }
    }

    void RemoveAllSlots()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        CopyIfNeeded();
        slots_->clear();
    }

private:
    // Make a copy of `slots_`, if there are readers in other threads.
    // This call requires a mutex for protection.
    void CopyIfNeeded()
    {
        if (!slots_.unique()) {
            slots_.reset(new SlotList(*slots_));
        }
    }

private:
    std::shared_ptr<SlotList> slots_;
    std::mutex mutex_;
};

}   // namespace internal

// The signal/slot provides yet another thread-safe style of notification/callback mechanism for
// classes.
// It aims to displace the observer pattern which is old-fashioned yet extensively employed.
// A signal object represents a message plugin board. It offers support for a given function to
// register in, i.e. establishing a connection between the signal and the function. When a message
// is emitted on a signal, all signed up functions are called, with the message passed in as
// the parameter.
// A slot object represents the connection between the function and it's associated signal. Someone
// with a slot object can disconnect from its signal, to stop receiving any message.
template<typename... Args>
class Signal;

class Slot {
private:
    explicit Slot(const std::shared_ptr<internal::IDisposable>& slot_impl) noexcept
        : impl_(slot_impl)
    {}

public:
    ~Slot() = default;

    DISALLOW_COPY(Slot);

    Slot(Slot&& other) noexcept
        : impl_(std::move(other.impl_))
    {}

    Slot& operator=(Slot&& rhs) noexcept
    {
        if (this != &rhs) {
            impl_ = std::move(rhs.impl_);
        }

        return *this;
    }

    void Disconnect() const
    {
        auto slot_impl = impl_.lock();
        if (!slot_impl) {
            return;
        }

        slot_impl->Dispose();
    }

private:
    template<typename... Args>
    friend class Signal;
    std::weak_ptr<internal::IDisposable> impl_;
};

template<typename... Args>
class Signal {
private:
    using Func = std::function<void(Args...)>;
    using SignalImpl = internal::SignalImpl<Func, Args...>;
    using SlotImpl = internal::SlotImpl<Func, Args...>;

public:
    Signal()
        : impl_(std::make_shared<SignalImpl>())
    {}

    ~Signal() = default;

    DISALLOW_COPY(Signal);

    DISALLOW_MOVE(Signal);

    Slot Connect(const Func& fn)
    {
        return Connect(Func(fn));
    }

    Slot Connect(Func&& fn)
    {
        auto slot_impl = std::make_shared<SlotImpl>(impl_, std::move(fn));
        impl_->AddSlot(slot_impl);

        return Slot(slot_impl);
    }

    Slot Connect(const Func& fn, const std::shared_ptr<void>& weakly_bound_object)
    {
        return Connect(Func(fn), weakly_bound_object);
    }

    Slot Connect(Func&& fn, const std::shared_ptr<void>& weakly_bound_object)
    {
        auto slot_impl = std::make_shared<SlotImpl>(impl_, std::move(fn), weakly_bound_object);
        impl_->AddSlot(slot_impl);

        return Slot(slot_impl);
    }

    void Emit(Args... args) const
    {
        impl_->Invoke(std::forward<Args>(args)...);
    }

    void DisconnectAll()
    {
        impl_->RemoveAllSlots();
    }

private:
    std::shared_ptr<SignalImpl> impl_;
};

}   // namespace kbase

#endif  // KBASE_SIGNALS_H_
