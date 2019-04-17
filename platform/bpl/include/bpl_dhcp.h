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
typedef void (*bpl_dhcp_mon_cb)(const char* op, const char* mac, 
                                const char* ip, const char* hostname);

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
