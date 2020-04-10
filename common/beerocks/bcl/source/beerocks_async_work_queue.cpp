/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2019 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include <bcl/beerocks_async_work_queue.h>

//////////////////////////////////////////////////////////////////////////////
/////////////////////////////// Implementation ///////////////////////////////
//////////////////////////////////////////////////////////////////////////////

namespace beerocks {

async_work_queue::~async_work_queue() { async_work_queue::before_stop(); }

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
} // namespace beerocks
