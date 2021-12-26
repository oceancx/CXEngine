#pragma once

// this queue is multithread safe. but it's not a efficient implement.

#include <queue>

namespace asio_kcp {

template<typename T>
class threadsafe_queue_mutex
{
private:
    std::queue<T> data_queue;
public:
    threadsafe_queue_mutex(){}

    size_t size()
    {
        std::lock_guard<std::mutex> guard(mutex_);
        return data_queue.size();
    }
    void push(T new_value)
    {
        std::lock_guard<std::mutex> guard(mutex_);
        data_queue.push(new_value);
    }
    bool try_pop(T& value)
    {
        std::lock_guard<std::mutex> guard(mutex_);
        if(data_queue.empty())
            return false;
        value=data_queue.front();
        data_queue.pop();
        return true;
    }
    std::queue<T> grab_all(void)
    {
        std::queue<T> ret;

        {
            std::lock_guard<std::mutex> guard(mutex_);
            std::swap(ret, data_queue);
        }

        return ret;
    }
    bool empty() const
    {
        std::lock_guard<std::mutex> guard(mutex_);
        return data_queue.empty();
    }

private:
    std::mutex mutex_;
};

} // end of namespace asio_kcp
