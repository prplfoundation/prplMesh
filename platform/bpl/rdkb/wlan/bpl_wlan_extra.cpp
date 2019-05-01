/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include "../../include/bpl_wlan.h"

#ifndef BPL_PASSIVE_MODE
int bpl_wlan_ap_start(const char* iface)
{
	return 0;
}
int bpl_wlan_ap_stop(const char* iface)
{
	return 0;
}
int bpl_wlan_sta_start(const char* iface)
{
	return 0;
}
int bpl_wlan_sta_stop(const char* iface)
{
	return 0;
}
int bpl_wlan_ap_enable(const char* iface)
{
	return 0;
}
int bpl_wlan_ap_disable(const char* iface)
{
	return 0;
}
int bpl_wlan_sta_enable(const char* iface)
{
	return 0;
}
int bpl_wlan_sta_disable(const char* iface)
{
	return 0;
}
int bpl_wlan_sta_acs(const char* iface)
{
	return 0;
}
int bpl_wlan_restore(const char* iface)
{
	return 0;
}

#else //PASSIVE_MODE is on

int bpl_wlan_ap_start(const char* iface)
{
	return 0;
}
int bpl_wlan_ap_stop(const char* iface)
{
	return 0;
}
int bpl_wlan_sta_start(const char* iface)
{
	return 0;
}
int bpl_wlan_sta_stop(const char* iface)
{
	return 0;
}
int bpl_wlan_ap_enable(const char* iface)
{
	return 0;
}
int bpl_wlan_ap_disable(const char* iface)
{
	return 0;
}
int bpl_wlan_sta_enable(const char* iface)
{
	return 0;
}
int bpl_wlan_sta_disable(const char* iface)
{
	return 0;
}
int bpl_wlan_sta_acs(const char* iface)
{
	return 0;
}
int bpl_wlan_restore(const char* iface)
{
	return 0;
}
#endif //BPL_PASSIVE_MODE