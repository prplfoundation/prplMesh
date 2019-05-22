/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include "../include/beerocks/bcl/beerocks_async_work_queue.h"

//////////////////////////////////////////////////////////////////////////////
/////////////////////////////// Implementation ///////////////////////////////
//////////////////////////////////////////////////////////////////////////////

namespace beerocks {

bool async_work_queue::init() { return true; }

bool async_work_queue::work()
{
    auto task = queue.pop();

    // Execute valid tasks
    if (task) {
        task->execute();
    }

    return true;
}

void async_work_queue::before_stop()
{
    // Unblock the worker thread
    queue.unblock();
}
}