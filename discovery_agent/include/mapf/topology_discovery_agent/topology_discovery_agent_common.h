/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _MAP_DISCOVERY_AGENT_COMMON_H_
#define _MAP_DISCOVERY_AGENT_COMMON_H_

namespace mapf
{

#define MCAST_ADDR_1905		"\x01\x80\xc2\x00\x00\x13"
#define MCAST_ADDR_LLDP		"\x01\x80\xc2\x00\x00\x0e"

// TODO : sync with standard defines and those available in framework
enum topology_discovery_agent_common_macros {
	MAX_NAME_LEN		= 64,
	MAX_IPv4_ADDR_LEN	= 4,
	MAX_IPv6_ADDR_LEN	= 16,
	MAX_SSID_LEN		= 6,
	DA_DISC_TIMER_INTVL	= 60,
	MAX_NEIGH_SUPPORTED	= 10,
	MAX_DATA_SIZE		= 1024,
	MAX_BR_COUNT		= 5,
	// TODO put actual MAX AP/CL/BSS
	MAX_AP				= 10,
	MAX_CL				= 10,
	MAX_BSS				= 10,
	PROFILE_1905_VER	= 0x01,
	MAX_URL_LEN			= 53
};

enum cmdu_message_macros {
	TOPOLOGY_DISCOVERY_MESSAGE		= 0x0000,
	TOPOLOGY_NOTIFICATION_MESSAGE	= 0x0001,
	TOPOLOGY_QUERY_MESSAGE			= 0x0002,
	TOPOLOGY_RESPONSE_MESSAGE		= 0x0003,
	HIGHER_LAYER_QUERY_MESSAGE		= 0x000D,
	HIGHER_LAYER_RESPONSE_MESSAGE	= 0x000E,
	CMDU_NO_FRAGMENT_INDICATOR		= 0x80
};

static char da_topics[10][MAX_NAME_LEN] = {"disc.topl_notify", "disc.wlan_ap_event", "disc.cl_assoc"};
#endif // #ifndef _MAP_DISCOVERY_AGENT_COMMON_H_

}
