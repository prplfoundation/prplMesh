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

#include <string.h>


#include "../../include/bpl_cfg.h"
#include "bpl_cfg_helper.h"
#include "../../common/utils/utils.h"
#include "../../common/utils/utils_net.h"

extern "C" {
#include <uci_wrapper.h>
}

#include "mapf/common/logger.h"

using namespace mapf;

using namespace beerocks::bpl;

int bpl_cfg_is_enabled()
{
	int retVal = -1;
	if (bpl_cfg_get_beerocks_param_int("enable", &retVal) == RETURN_ERR) {
		MAPF_ERR("bpl_cfg_is_enabled: Failed to read Enable parameter\n");
		return RETURN_ERR;
	}
	return retVal;
}

int bpl_cfg_is_master()
{
	int retVal = 0;
	char man_mode[BPL_GW_DB_MANAGE_MODE_LEN] = { 0 };
	if (bpl_cfg_get_beerocks_param("management_mode", man_mode, BPL_GW_DB_MANAGE_MODE_LEN) < 0) {
		MAPF_ERR("bpl_cfg_is_master: Failed to read ManagementMode\n");
		retVal = -1;
	} else {
		std::string mode_str(man_mode);
		if (mode_str == "Multi-AP-Controller-and-Agent") {
			retVal = 1;
		} else if (mode_str == "Multi-AP-Agent") {
			retVal = 0;
		} else {
			MAPF_ERR("bpl_cfg_is_master: Unexpected ManagementMode\n");
			retVal = -1;
		}
	}
	return retVal;
}

int bpl_cfg_get_operating_mode()
{
	int retVal = 0;
	char op_mode[BPL_GW_DB_OPER_MODE_LEN] = { 0 };
	if (bpl_cfg_get_beerocks_param("operating_mode", op_mode, BPL_GW_DB_OPER_MODE_LEN) < 0) {
		MAPF_ERR("bpl_cfg_get_operating_mode: Failed to read OperatingMode\n");
		retVal = -1;
	} else {
		std::string mode_str(op_mode);
		if (mode_str == "Gateway") {
			retVal = BPL_OPER_MODE_GATEWAY;
		} else if (mode_str == "Gateway-WISP") {
			retVal = BPL_OPER_MODE_GATEWAY_WISP;
		}else if (mode_str == "WDS-Extender") {
			retVal = BPL_OPER_MODE_WDS_EXTENDER;
		}else if (mode_str == "WDS-Repeater") {
			retVal = BPL_OPER_MODE_WDS_REPEATER;
		}else if (mode_str == "L2NAT-Client") {
			retVal = BPL_OPER_MODE_L2NAT_CLIENT;
		} else {
			MAPF_ERR("bpl_cfg_get_operating_mode: Unexpected OperatingMode\n");
			retVal = -1;
		}
	}
	return retVal;
}

int bpl_cfg_is_onboarding()
{
	int retVal = -1;
	if (bpl_cfg_get_beerocks_param_int("onboarding", &retVal) == RETURN_ERR) {
		MAPF_ERR("bpl_cfg_is_onboarding: Failed to read Onboarding parameter\n");
		return RETURN_ERR;
	}
	return retVal;
}

int bpl_cfg_is_wired_backhaul()
{
	int retVal = -1;
	if (bpl_cfg_get_beerocks_param_int("wired_backhaul", &retVal) == RETURN_ERR) {
		MAPF_ERR("bpl_cfg_is_wired_backhaul: Failed to read WiredBackhaul parameter\n");
		return RETURN_ERR;
	}
	return retVal;
}

int bpl_cfg_get_rdkb_extensions()
{
	int retVal = -1;
	if (bpl_cfg_get_beerocks_param_int("rdkb_extensions", &retVal) == RETURN_ERR) {
		printf("bpl_cfg_get_rdkb_extensions: Failed to read RDKB Extensions parameter\n");
		return RETURN_ERR;
	}
	return retVal;
}

int bpl_cfg_get_band_steering()
{
	int retVal = -1;
	if (bpl_cfg_get_beerocks_param_int("band_steering", &retVal) == RETURN_ERR) {
		MAPF_ERR("bpl_cfg_get_band_steering: Failed to read BandSteering parameter\n");
		return RETURN_ERR;
	}
	return retVal;
}

int bpl_cfg_get_dfs_reentry()
{
	int retVal = -1;
	if (bpl_cfg_get_beerocks_param_int("dfs_reentry", &retVal) == RETURN_ERR) {
		printf("bpl_cfg_get_dfs_reentry: Failed to read DfsReentry parameter\n");
		return RETURN_ERR;
	}
	return retVal;
}

int bpl_cfg_get_passive_mode()
{
	int retVal = -1;
	if (bpl_cfg_get_beerocks_param_int("passive_mode", &retVal) == RETURN_ERR) {
		printf("bpl_cfg_get_passive_mode: Failed to read PassiveMode parameter\n");
		return RETURN_ERR;
	}
	return retVal;
}

int bpl_cfg_get_client_roaming()
{
	int retVal = -1;
	if (bpl_cfg_get_beerocks_param_int("client_roaming", &retVal) == RETURN_ERR) {
		MAPF_ERR("bpl_cfg_get_client_roaming: Failed to read ClientRoaming parameter\n");
		return RETURN_ERR;
	}
	return retVal;
}

int bpl_cfg_get_device_info(BPL_DEVICE_INFO* device_info)
{
	return 0;
}

int bpl_cfg_get_wifi_params(const char iface[BPL_IFNAME_LEN], struct BPL_WLAN_PARAMS* wlan_params)
{
	int retVal = 0;
	bool disabled = false;
	int index = 0;
	
	if (!iface || !wlan_params) {
		return RETURN_ERR;
	}
	
	retVal = bpl_cfg_get_index_from_interface(iface, &index);
	if (retVal) {
		return retVal;
	}

	retVal |= uci_converter_get_bool(TYPE_RADIO, index, "disabled", &disabled);
	if (!retVal) {
		wlan_params->enabled = !disabled;
	}
	retVal |= bpl_cfg_get_auto_channel_enable(index, &wlan_params->acs);
	retVal |= bpl_cfg_get_ssid_advertisement_enabled(index, &wlan_params->advertise_ssid);

	char ssid[MAX_UCI_BUF_LEN] = {0}, security[MAX_UCI_BUF_LEN] = {0}, passphrase[MAX_UCI_BUF_LEN] = {0};
	retVal |= uci_converter_get_str(TYPE_VAP, index, "ssid", ssid);
	retVal |= uci_converter_get_str(TYPE_VAP, index, "wav_security_mode", security);
	std::string mode = std::string(security);
	if (mode == BPL_WLAN_SEC_WEP64_STR || mode == BPL_WLAN_SEC_WEP128_STR) {
		retVal |= bpl_cfg_get_wep_key(index, -1, passphrase);
	} else if (mode != BPL_WLAN_SEC_NONE_STR) {
		retVal |= uci_converter_get_str(TYPE_VAP, index, "key", passphrase);
	}

	utils::copy_string(wlan_params->ssid, ssid, BPL_SSID_LEN);
	utils::copy_string(wlan_params->security, security, BPL_SEC_LEN);
	utils::copy_string(wlan_params->passphrase, passphrase, BPL_PASS_LEN);

	return retVal;
}

int bpl_cfg_get_backhaul_params(int* max_vaps, int* network_enabled, int* prefered_radio_band)
{
    return 0;
}

int bpl_cfg_get_backhaul_vaps(char* backhaul_vaps_buf, const int buf_len)
{
    return 0;
}

int bpl_cfg_get_beerocks_credentials(const int radio_dir, char ssid[BPL_SSID_LEN], char pass[BPL_PASS_LEN], char sec[BPL_SEC_LEN])
{
	int retVal = 0;

	retVal |= bpl_cfg_get_beerocks_param("ssid", ssid, BPL_SSID_LEN);
	retVal |= bpl_cfg_get_beerocks_param("mode_enabled", sec, BPL_SEC_LEN);
	if (!strcmp(sec,"WEP-64") || !strcmp(sec,"WEP-128")) {
		retVal |= bpl_cfg_get_beerocks_param("wep_key", pass, BPL_PASS_LEN);
	} else {
		retVal |= bpl_cfg_get_beerocks_param("key_passphrase", pass, BPL_PASS_LEN);
	}
	
	return retVal;
}

int bpl_cfg_notify_onboarding_completed(const char ssid[BPL_SSID_LEN], const char pass[BPL_PASS_LEN], const char sec[BPL_SEC_LEN],
                                        const char iface_name[BPL_IFNAME_LEN], const int success)
{
    //return (sl_beerocks_notify_onboarding_completed(ssid, pass, sec, iface_name, success) ? 0 : -1);
	return 0;
}

int bpl_cfg_notify_fw_version_mismatch()
{
	return 0;
}

int bpl_cfg_notify_error(int code, const char data[BPL_ERROR_STRING_LEN])
{
	return 0;
}

int bpl_cfg_notify_iface_status(const BPL_INTERFACE_STATUS_NOTIFICATION *status_notif)
{
	return 0;
}

int bpl_cfg_get_administrator_credentials(char pass[BPL_PASS_LEN])
{
	return 0;
}