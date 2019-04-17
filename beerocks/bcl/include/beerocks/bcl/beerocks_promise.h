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

#ifndef _BEEROCKS_PROMISE_H_
#define _BEEROCKS_PROMISE_H_

#include <sys/time.h>
#include <pthread.h>

namespace beerocks {

    template <class T>
    class promise {
        public:
            promise() {};
            ~promise() {};
            
            // Set value for waiting threads
            void set_value(const T value)
            {
                pthread_mutex_lock(&m_mut);
                m_value = value;
                m_signal = 1;
                pthread_cond_broadcast(&m_cond);
                pthread_mutex_unlock(&m_mut);
            }

            // Wait for a value to be set
            void wait()
            {
                // lock the mutex and wait for the conditional variable to be set
                pthread_mutex_lock(&m_mut);
                // If signaled before the wait, no need to wait
                if (m_signal == 1) {
                    m_signal = 0;
                } else {
                    pthread_cond_wait(&m_cond, &m_mut);
                }
                pthread_mutex_unlock(&m_mut);
                
            }
            
            // Wait for a value to be set with timeout
            bool wait_for(uint32_t timeout_ms)
            {
                struct timeval now;
                struct timespec timeout;

                // set the absolut timeout
                gettimeofday(&now, NULL);
                timeout.tv_sec = now.tv_sec + (timeout_ms/1000);
                timeout.tv_nsec = (now.tv_usec + (timeout_ms%1000)*1000) * 1000;

                int retcode = 0;

                // lock the mutex and wait for the conditional variable to be set or timeout
                pthread_mutex_lock(&m_mut);
                // If signaled before the wait, no need to wait
                if (m_signal == 1) {
                    m_signal = 0;
                } else {
                    retcode = pthread_cond_timedwait(&m_cond, &m_mut, &timeout);
                }
                pthread_mutex_unlock(&m_mut);

                if (retcode != 0)
                    return false;
                
                return true;
                
            }

            T get_value()
            {
                return m_value;
            }

        private:
            pthread_cond_t m_cond = PTHREAD_COND_INITIALIZER;
            pthread_mutex_t m_mut = PTHREAD_MUTEX_INITIALIZER;
            T m_value;
            bool m_signal = 0;

    };
}//  namespace beerocks


#endif
