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

#ifndef _BEEROCKS_ASYNC_WORK_QUEUE_H_
#define _BEEROCKS_ASYNC_WORK_QUEUE_H_

#include "beerocks_thread_base.h"
#include "beerocks_thread_safe_queue.h"
#include "beerocks_backport.h"

#include <future>

namespace beerocks {

class async_work_queue : public thread_base
{
    public:
        async_work_queue() = default;
        ~async_work_queue() = default;

        template <typename R, typename ... Args, typename F>
        std::future<R> enqueue(F func, Args... args) {
            auto task = std::make_unique<Task<R>>(std::bind(func, args...));
            std::future<R> f = task->task.get_future();
            queue.push(std::move(task));           
            return f;
        }

    protected:

        virtual bool init() override;
        virtual bool work() override;
        virtual void before_stop() override;
    
    private:

        struct BasicTask
        {
            virtual ~BasicTask() {};
            virtual void execute() = 0;
        };

        template <typename R>
        struct Task : public BasicTask 
        {
            std::packaged_task<R()> task;
            
            Task(std::function<R()> func) : task(func) { }
            
            void execute() { task(); }
        };
        
        beerocks::thread_safe_queue<std::unique_ptr<BasicTask>> queue;
};

} // namespace beerocks

#endif // _BEEROCKS_THREAD_SAFE_QUEUE_H_