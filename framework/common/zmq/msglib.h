/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef __MAPF_COMMON_MSGLIB_H__
#define __MAPF_COMMON_MSGLIB_H__

#include <zmq.h>

namespace mapf {

struct msglib_socket {
    void *sd_;
};

} //namespace mapf

#endif /*__MAPF_COMMON_MSGLIB_H__ */