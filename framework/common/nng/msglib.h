/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2019 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef __MAPF_COMMON_MSGLIB_H__
#define __MAPF_COMMON_MSGLIB_H__

#include <nng/nng.h>

namespace mapf {

struct msglib_socket {
    nng_socket sd_;
};

} //namespace mapf

#endif /*__MAPF_COMMON_MSGLIB_H__ */