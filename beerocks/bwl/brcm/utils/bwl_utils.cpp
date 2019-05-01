/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include "bwl_utils.h"

#include <wlioctl.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <linux/types.h>
 
int bwl_wl_ioctl(char *name, int cmd, void *buf, int len)
{
    struct ifreq ifr;
    wl_ioctl_t ioc;
    int ret = 0;
    int s;

    /*
    * open socket to kernel
    */
    if ((s = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket");
        return errno;
    }

    /*
    * do it
    */
    ioc.cmd = cmd;
    ioc.buf = buf;
    ioc.len = len;
    strncpy(ifr.ifr_name, name, IFNAMSIZ);
    ifr.ifr_data = (caddr_t) & ioc;
    if ((ret = ioctl(s, SIOCDEVPRIVATE, &ifr)) < 0) {
        if (cmd != WLC_GET_MAGIC) {
            perror(ifr.ifr_name);
        }
    }

    /*
    * cleanup
    */
    close(s);
    return ret;
} 