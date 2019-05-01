/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
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
