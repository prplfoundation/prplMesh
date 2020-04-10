/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2019 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include <bcl/beerocks_thread_base.h>

using namespace beerocks;

thread_base::~thread_base()
{
    should_stop = true;
    thread_base::before_stop();
    join();
}

bool thread_base::start(std::string name)
{
    bool init_ok = init();
    if (!init_ok) {
        should_stop = true;
    } else {
        if (!name.empty())
            thread_name = name;
        stop();
        should_stop       = false;
        worker_is_running = true;
        worker            = std::thread(&thread_base::run, this);
    }
    return init_ok;
}

void thread_base::join()
{
    if (worker.joinable()) {
        worker.join();
    }
}

void thread_base::stop(bool block)
{
    should_stop = true;
    thread_base::before_stop();
    if (block) {
        join();
    }
}

void thread_base::run()
{
    while (!should_stop) {
        if (!work()) {
            break;
        }
    }
    on_thread_stop();
    worker_is_running = false;
}
