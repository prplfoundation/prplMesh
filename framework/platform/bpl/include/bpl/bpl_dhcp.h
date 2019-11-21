/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _BPL_DHCP_H_
#define _BPL_DHCP_H_

#include "bpl.h"

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************/
/******************************* Definitions ********************************/
/****************************************************************************/

/* 
Example for sending an event using the "ubus" shell command ubus call:
  dhcp_event notify '{ "id": 1234, "op": "add", "mac": "11:22:33:44:55:66", 
                       "ip": "1.1.1.1", "hostname": "test-hostname" }'
*/

/**
 * DHCP Monitor Event Callback
 *
 * @param [in] op Operation string (add, del etc.)
 * @param [in] mac Client's MAC address
 * @param [in] ip Client's IP address
 * @param [in] hostname Client's host name
 */
typedef void (*bpl_dhcp_mon_cb)(const char *op, const char *mac, const char *ip,
                                const char *hostname);

/****************************************************************************/
/******************************** Functions *********************************/
/****************************************************************************/

/**
 * Start the DHCP monitor.
 *
 * @param [in] cb Callback function for DHCP events.
 *
 * @return File descriptor to the socket used for monitoring the UBUS.
 * @return -1 Error.
 */
int bpl_dhcp_mon_start(bpl_dhcp_mon_cb cb);

/** 
 * Handle UBUS event.
 * This function should be called when there's data to be read from
 * from the UBUS socket.
 *
 * @return 0 On success of -1 on failure.
 */
int bpl_dhcp_mon_handle_event();

/**
 * Stop the DHCP monitor.
 *
 * @return 0 On success of -1 on failure.
 */
int bpl_dhcp_mon_stop();

#ifdef __cplusplus
}
#endif

#endif // _BPL_DHCP_H_
