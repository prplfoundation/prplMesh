/*
 * INTEL CONFIDENTIAL
 * Copyright 2016 Intel Corporation All Rights Reserved.
 *
 * The source code contained or described herein and all documents related to
 * the source code ("Material") are owned by Intel Corporation or its
 * suppliers or licensors.  Title to the Material remains with Intel
 * Corporation or its suppliers and licensors.  The Material contains trade
 * secrets and proprietary and confidential information of Intel or its
 * suppliers and licensors.  The Material is protected by worldwide copyright
 * and trade secret laws and treaty provisions. No part of the Material may
 * be used, copied, reproduced, modified, published, uploaded, posted,
 * transmitted, distributed, or disclosed in any way without Intel's prior
 * express written permission.
 *
 * No license under any patent, copyright, trade secret or other intellectual
 * property right is granted to or conferred upon you by disclosure or
 * delivery of the Materials,  either expressly, by implication, inducement,
 * estoppel or otherwise.  Any license under such intellectual property
 * rights must be express and approved by Intel in writing.
 */

#ifndef _BEEROCKS_THREAD_SAFE_QUEUE_H_
#define _BEEROCKS_THREAD_SAFE_QUEUE_H_

#include <queue>
#include <mutex>
#include <chrono>
#include <condition_variable>

namespace beerocks {
 
template <typename T>
class thread_safe_queue
{
public:
 
    T pop(bool block = true, int timeout = 0)
    {
        std::unique_lock<std::mutex> mlock(mutex_, std::defer_lock);

        if (block) {
            mlock.lock();
            while (queue_.empty()) {
                if (!timeout) {
                    cond_.wait(mlock);
                    if (queue_.empty()) { return T(); }

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
 
    bool push(const T& item, bool block = true)
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
 
    bool push(T&& item, bool block = true)
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