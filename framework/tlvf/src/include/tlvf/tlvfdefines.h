/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2019 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _TLVF_DEFINES_H_
#define _TLVF_DEFINES_H_

#if __GNUC__ >= 7 || __cplussplus >= 201703L
#define FALLTHROUGH __attribute__((fallthrough))
#else
// clang-format off
#define FALLTHROUGH do { } while (0)
// clang-format on
#endif

#endif
