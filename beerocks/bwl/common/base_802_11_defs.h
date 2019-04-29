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

#ifndef _BWL_802_11_DEFS_H_
#define _BWL_802_11_DEFS_H_

#include <stdint.h>

#define BIT(x) (1U << (x))

/* HT Capabilities Info field within HT Capabilities element */
#define HT_CAP_INFO_LDPC_CODING_CAP		((uint16_t) BIT(0))
#define HT_CAP_INFO_SUPP_CHANNEL_WIDTH_SET	((uint16_t) BIT(1))
#define HT_CAP_INFO_SMPS_MASK			((uint16_t) (BIT(2) | BIT(3)))
#define HT_CAP_INFO_SMPS_STATIC			((uint16_t) 0)
#define HT_CAP_INFO_SMPS_DYNAMIC		((uint16_t) BIT(2))
#define HT_CAP_INFO_SMPS_DISABLED		((uint16_t) (BIT(2) | BIT(3)))
#define HT_CAP_INFO_GREEN_FIELD			((uint16_t) BIT(4))
#define HT_CAP_INFO_SHORT_GI20MHZ		((uint16_t) BIT(5))
#define HT_CAP_INFO_SHORT_GI40MHZ		((uint16_t) BIT(6))
#define HT_CAP_INFO_TX_STBC			((uint16_t) BIT(7))
#define HT_CAP_INFO_RX_STBC_MASK		((uint16_t) (BIT(8) | BIT(9)))
#define HT_CAP_INFO_RX_STBC_1			((uint16_t) BIT(8))
#define HT_CAP_INFO_RX_STBC_12			((uint16_t) BIT(9))
#define HT_CAP_INFO_RX_STBC_123			((uint16_t) (BIT(8) | BIT(9)))
#define HT_CAP_INFO_DELAYED_BA			((uint16_t) BIT(10))
#define HT_CAP_INFO_MAX_AMSDU_SIZE		((uint16_t) BIT(11))
#define HT_CAP_INFO_DSSS_CCK40MHZ		((uint16_t) BIT(12))
/* B13 - Reserved (was PSMP support during P802.11n development) */
#define HT_CAP_INFO_40MHZ_INTOLERANT		((uint16_t) BIT(14))
#define HT_CAP_INFO_LSIG_TXOP_PROTECT_SUPPORT	((uint16_t) BIT(15)



/* Radio Measurement capabilities (from RM Enabled Capabilities element)
 * IEEE Std 802.11-2016, 9.4.2.45, Table 9-157 */
/* byte 1 (out of 5) */
#define WLAN_RRM_CAPS_LINK_MEASUREMENT BIT(0)
#define WLAN_RRM_CAPS_NEIGHBOR_REPORT BIT(1)
#define WLAN_RRM_CAPS_BEACON_REPORT_PASSIVE BIT(4)
#define WLAN_RRM_CAPS_BEACON_REPORT_ACTIVE BIT(5)
#define WLAN_RRM_CAPS_BEACON_REPORT_TABLE BIT(6)
/* byte 2 (out of 5) */
#define WLAN_RRM_CAPS_CHANNEL_LOAD BIT(1)
#define WLAN_RRM_CAPS_NOISE_HISTOGRAM BIT(2)
#define WLAN_RRM_CAPS_STATISTICS_MEASUREMENT BIT(3)
#define WLAN_RRM_CAPS_LCI_MEASUREMENT BIT(4)
/* byte 5 (out of 5) */
#define WLAN_RRM_CAPS_FTM_RANGE_REPORT BIT(2)





#endif // _BWL_802_11_DEFS_H_