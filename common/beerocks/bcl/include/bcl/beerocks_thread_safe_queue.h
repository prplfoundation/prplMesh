/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _BEEROCKS_THREAD_SAFE_QUEUE_H_
#define _BEEROCKS_THREAD_SAFE_QUEUE_H_

#include <chrono>
#include <condition_variable>
#include <mutex>
#include <queue>

namespace beerocks {

template <typename T> class thread_safe_queue {
public:
    T pop(bool block = true, int timeout = 0)
    {
        std::unique_lock<std::mutex> mlock(mutex_, std::defer_lock);

        if (block) {
            mlock.lock();
            while (queue_.empty()) {
                if (!timeout) {
                    cond_.wait(mlock);
                    if (queue_.empty()) {
                        return T();
                    }

                } else {
                    if (cond_.wait_for(mlock, std::chrono::milliseconds(timeout)) ==
                        std::cv_status::timeout) {
                        return T();
                    }
                }
            }
        } else {
            if (!mlock.try_lock() || queue_.empty()) {
                return T();
            }
        }

        auto item = std::move(queue_.front());
        queue_.pop();
        return item;
    }

    bool push(const T &item, bool block = true)
    {
        std::unique_lock<std::mutex> mlock(mutex_, std::defer_lock);

        if (block) {
            mlock.lock();
        } else if (!mlock.try_lock()) {
            return false;
        }

        queue_.push(item);
        mlock.unlock();
        cond_.notify_one();

        return true;
    }

    bool push(T &&item, bool block = true)
    {
        std::unique_lock<std::mutex> mlock(mutex_, std::defer_lock);

        if (block) {
            mlock.lock();
        } else if (!mlock.try_lock()) {
            return false;
        }

        queue_.push(std::move(item));
        mlock.unlock();
        cond_.notify_one();

        return true;
    }

    bool empty()
    {
        std::unique_lock<std::mutex> mlock(mutex_);
        return queue_.empty();
    }

    void clear()
    {
        std::unique_lock<std::mutex> mlock(mutex_);
        while (!queue_.empty()) {
            queue_.pop();
        }
    }

    void unblock()
    {
        // Unblock any client that is blocked on the pop() method
        cond_.notify_one();
    }

private:
    std::queue<T> queue_;
    std::condition_variable cond_;
    mutable std::mutex mutex_;
};

} // namespace beerocks

#endif // _BEEROCKS_THREAD_SAFE_QUEUE_H_