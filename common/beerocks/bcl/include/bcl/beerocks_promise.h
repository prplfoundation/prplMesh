/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2019 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _BEEROCKS_PROMISE_H_
#define _BEEROCKS_PROMISE_H_

#include "../../common/include/mapf/common/err.h"
#include <easylogging++.h>
#include <pthread.h>
#include <sys/time.h>

namespace beerocks {

template <class T> class promise {
public:
    promise() : m_value(T()), m_signal(false){};
    ~promise(){};

    // Set value for waiting threads
    void set_value(const T value)
    {
        int err = pthread_mutex_lock(&m_mut);
        mapf_assert(err == 0);
        m_value  = value;
        m_signal = 1;
        err      = pthread_cond_broadcast(&m_cond);
        mapf_assert(err == 0);
        err = pthread_mutex_unlock(&m_mut);
        mapf_assert(err == 0);
    }
    // Wait for a value to be set
    void wait()
    {
        // lock the mutex and wait for the conditional variable to be set
        int err = pthread_mutex_lock(&m_mut);
        mapf_assert(err == 0);
        // If signaled before the wait, no need to wait
        if (m_signal == 1) {
            m_signal = 0;
        } else {
            err = pthread_cond_wait(&m_cond, &m_mut);
            mapf_assert(err == 0);
        }
        err = pthread_mutex_unlock(&m_mut);
        mapf_assert(err == 0);
    }

    // Wait for a value to be set with timeout
    bool wait_for(uint32_t timeout_ms)
    {
        struct timeval now;
        struct timespec timeout;
        // set the absolut timeout
        gettimeofday(&now, NULL);
        timeout.tv_sec  = now.tv_sec + (timeout_ms / 1000);
        timeout.tv_nsec = (now.tv_usec + (timeout_ms % 1000) * 1000) * 1000;

        // lock the mutex and wait for the conditional variable to be set or timeout
        int err      = pthread_mutex_lock(&m_mut);
        int err_wait = 0;
        mapf_assert(err == 0);
        // If signaled before the wait, no need to wait
        if (m_signal == 1) {
            m_signal = 0;
        } else {
            if ((err_wait = pthread_cond_timedwait(&m_cond, &m_mut, &timeout)) != 0) {
                LOG(ERROR) << "pthread_cond_timedwait failed, error code: " << err;
            }
            err = pthread_mutex_unlock(&m_mut);
            mapf_assert(err == 0);
        }
        if (err_wait)
            return false;
        return true;
    }

    T get_value() { return m_value; }

private:
    pthread_cond_t m_cond = PTHREAD_COND_INITIALIZER;
    pthread_mutex_t m_mut = PTHREAD_MUTEX_INITIALIZER;
    T m_value;
    bool m_signal;
};
} //  namespace beerocks

#endif
