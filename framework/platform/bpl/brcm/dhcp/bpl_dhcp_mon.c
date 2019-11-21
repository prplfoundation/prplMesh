/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include <bpl/bpl_dhcp.h>

#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#include <sys/stat.h>
#include <sys/types.h>

//////////////////////////////////////////////////////////////////////////////
////////////////////////// Local Module Definitions //////////////////////////
//////////////////////////////////////////////////////////////////////////////

static bpl_dhcp_mon_cb s_pCallback = NULL;
static int s_fdDHCPEvents          = -1;
static char s_arrReadBuff[1024];

//////////////////////////////////////////////////////////////////////////////
/////////////////////////////// Implementation ///////////////////////////////
//////////////////////////////////////////////////////////////////////////////

int bpl_dhcp_mon_start(bpl_dhcp_mon_cb cb)
{
// TODO: Move to configuration
#define DHCP_FIFO_PATH "/tmp/beerocks/beerocks_dhcp_mon.fifo"

    int ret;

    // Create the FIFO file
    if ((ret = mkfifo(DHCP_FIFO_PATH, 0666)) < 0) {
        if (errno != EEXIST) {
            return -errno;
        }
    }

    if (s_fdDHCPEvents == -1) {
        // Open the FIFO for Read/Write prevent the OS from generating
        // EOF for reads with no writer
        // TODO: Get the FIFO path externally
        s_fdDHCPEvents = open("/tmp/beerocks/beerocks_dhcp_mon.fifo", O_RDWR);
    }

    // Store the callback
    s_pCallback = cb;

    return (s_fdDHCPEvents);
}

int bpl_dhcp_mon_handle_event()
{
    // op, mac, ip, hostname
    char *arrTok[4] = {0};
    char *tmpTok;
    int numTok = 0;

    // Read incoming data
    int ret = read(s_fdDHCPEvents, s_arrReadBuff, sizeof(s_arrReadBuff));

    if (ret <= 0) {
        return ret;
    }

    // Terminate the string (and remove the newline character from the end)
    s_arrReadBuff[ret - 1] = 0;

    // Parse the string
    tmpTok = strtok(s_arrReadBuff, ",");
    while (tmpTok != NULL && numTok < 4) {
        arrTok[numTok++] = tmpTok;
        tmpTok           = strtok(NULL, ",");
    }

    // TODO: Handle multiple events in one buffer?

    // Execute the callback
    if (s_pCallback)
        s_pCallback((arrTok[0]) ? arrTok[0] : "", (arrTok[1]) ? arrTok[1] : "",
                    (arrTok[2]) ? arrTok[2] : "", (arrTok[3]) ? arrTok[3] : "");

    return 0;
}

int bpl_dhcp_mon_stop()
{
    if (s_fdDHCPEvents != -1) {
        close(s_fdDHCPEvents);
        s_fdDHCPEvents = -1;
    }

    return 0;
}
