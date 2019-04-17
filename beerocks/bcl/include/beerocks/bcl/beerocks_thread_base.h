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
