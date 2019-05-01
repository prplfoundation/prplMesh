/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _BEEROCKS_THREAD_BASE_H_
#define _BEEROCKS_THREAD_BASE_H_

#define THREAD_LOG(a) (LOG(a) << get_name() << ": ")

#include <thread>
#include <string>
#include <atomic>

namespace beerocks
{
    class thread_base
    {
        public:
            thread_base() : should_stop(false), worker_is_running(false) {}
            virtual ~thread_base();
            bool start(std::string name = "");
            void join();
            void stop(bool block = true);
            bool is_running() {
                return worker_is_running;
            }
            std::string get_name() {
                return thread_name;
            }

            int get_thread_last_error_code() { return thread_last_error_code;}
            
        protected:
            std::string thread_name;
            uint32_t thread_last_error_code = 0;
            bool should_stop;

            virtual bool init() = 0;
            virtual bool work() = 0;
            virtual void before_stop() {}
            virtual void on_thread_stop() {}

        private:
            void run();
            std::thread worker;
            bool worker_is_running;
    };
}

#endif // _BEEROCKS_THREAD_BASE_H_
