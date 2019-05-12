/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _BEEROCKS_ASYNC_WORK_QUEUE_H_
#define _BEEROCKS_ASYNC_WORK_QUEUE_H_

#include "beerocks_backport.h"
#include "beerocks_thread_base.h"
#include "beerocks_thread_safe_queue.h"

#include <future>

namespace beerocks {

class async_work_queue : public thread_base {
public:
    async_work_queue()  = default;
    ~async_work_queue() = default;

    template <typename R, typename... Args, typename F> std::future<R> enqueue(F func, Args... args)
    {
        auto task        = std::make_unique<Task<R>>(std::bind(func, args...));
        std::future<R> f = task->task.get_future();
        queue.push(std::move(task));
        return f;
    }

protected:
    virtual bool init() override;
    virtual bool work() override;
    virtual void before_stop() override;

private:
    struct BasicTask {
        virtual ~BasicTask(){};
        virtual void execute() = 0;
    };

    template <typename R> struct Task : public BasicTask {
        std::packaged_task<R()> task;

        Task(std::function<R()> func) : task(func) {}

        void execute() { task(); }
    };

    beerocks::thread_safe_queue<std::unique_ptr<BasicTask>> queue;
};

} // namespace beerocks

#endif // _BEEROCKS_THREAD_SAFE_QUEUE_H_