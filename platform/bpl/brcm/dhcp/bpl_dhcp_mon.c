/*
 * INTEL CONFIDENTIAL
 * Copyright 2016 Intel Corporation All Rights Reserved.
 *
 * The source code contained or described herein and all documents related to
 * the source code ("Material") are owned by Intel Corporation or its
 * suppliers or licensors.  Title to the Material remains with Intel
 * Corporation or its suppliers and licensors.  The Material contains trade
 * secrets and proprietary and confidential information of Intel or its
 * suppliers and licensors.  The Material is protected by worldwide copyright
 * and trade secret laws and treaty provisions. No part of the Material may
 * be used, copied, reproduced, modified, published, uploaded, posted,
 * transmitted, distributed, or disclosed in any way without Intel's prior
 * express written permission.
 *
 * No license under any patent, copyright, trade secret or other intellectual
 * property right is granted to or conferred upon you by disclosure or
 * delivery of the Materials,  either expressly, by implication, inducement,
 * estoppel or otherwise.  Any license under such intellectual property
 * rights must be express and approved by Intel in writing.
 */

#include <bpl_dhcp.h>

#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>

//////////////////////////////////////////////////////////////////////////////
////////////////////////// Local Module Definitions //////////////////////////
//////////////////////////////////////////////////////////////////////////////

static bpl_dhcp_mon_cb  s_pCallback = NULL;
static int              s_fdDHCPEvents = -1;
static char             s_arrReadBuff[1024];

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
    char* arrTok[4] = {0};
    char* tmpTok;
    int   numTok = 0;

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
        tmpTok = strtok(NULL, ",");
    }

    // TODO: Handle multiple events in one buffer?

    // Execute the callback
    if (s_pCallback)
        s_pCallback((arrTok[0]) ? arrTok[0] : "", 
                    (arrTok[1]) ? arrTok[1] : "",
                    (arrTok[2]) ? arrTok[2] : "",
                    (arrTok[3]) ? arrTok[3] : "");

    return 0;
}

int bpl_dhcp_mon_stop()
{
    if (s_fdDHCPEvents != -1) {
        close (s_fdDHCPEvents);
        s_fdDHCPEvents = -1;
    }

    return 0;
}
