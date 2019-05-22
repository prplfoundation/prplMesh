/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _BWL_UTILS_H
#define _BWL_UTILS_H

#ifdef __cplusplus
extern "C" {
#endif

int bwl_wl_ioctl(char *name, int cmd, void *buf, int len);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif // _BWL_UTILS_H
