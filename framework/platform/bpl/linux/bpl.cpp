/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include <bpl/bpl.h>

#include <mapf/common/logger.h>

// Use easylogging++ instance of the main application
SHARE_EASYLOGGINGPP(el::Helpers::storage())

//////////////////////////////////////////////////////////////////////////////
/////////////////////////////// Implementation ///////////////////////////////
//////////////////////////////////////////////////////////////////////////////

namespace bpl {

int init()
{
    // Do nothing
    return 0;
}

void close()
{
    // Do nothing
}

} // namespace bpl
