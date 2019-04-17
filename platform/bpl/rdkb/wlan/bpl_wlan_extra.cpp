/*
 * INTEL CONFIDENTIAL
 * Copyright 2019 Intel Corporation All Rights Reserved.
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