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

#include "../../include/bpl_cfg.h"
#include "bpl_cfg_helper.h"
#include "../../common/utils/utils.h"

extern "C" {
#include <uci_wrapper.h>
}

#include "mapf/common/logger.h"

using namespace mapf;

using namespace beerocks::bpl;

#ifndef BPL_PASSIVE_MODE

//============================================ STATIC START ============================================

static int bpl_get_number_of_radios(int *radios)
{
	char radio_dev[MAX_LEN_PARAM_VALUE] = "";
	char phy_param[MAX_LEN_PARAM_VALUE] = "phy";
	int i;

	*radios = 0;
	for (i = 0; i < MAX_NUM_OF_RADIOS; i++) {
		if (uci_converter_get_str(TYPE_RADIO, i, phy_param, radio_dev) == RETURN_OK)
			(*radios)++;
	}

	return RETURN_OK;
}

static int bpl_cfg_iface_config_up(int index)
{
	char radio_dev[MAX_LEN_PARAM_VALUE] = { '\0' };
	char device_param[MAX_LEN_PARAM_VALUE] = "device";
	char command[MAX_LEN_PARAM_VALUE] = { '\0' };
	int radioCount = 0;

	bpl_get_number_of_radios(&radioCount);

	if (index < radioCount) {/* radio */
		sprintf(command, "%s up radio%d", "/sbin/wifi", UCI_INDEX(TYPE_RADIO, index));
	} else { /* VAP */
		if (uci_converter_get_str(TYPE_VAP, index, device_param, radio_dev) == RETURN_ERR) {
			MAPF_ERR("%s failed retrieving radio device for vap %d\n", __FUNCTION__, index);
			return RETURN_ERR;
		}
		sprintf(command, "%s up %s", "/sbin/wifi", radio_dev);
	}

	if (uci_converter_commit_wireless() != RETURN_OK)
		MAPF_ERR("%s uci_converter_commit_wireless failed\n", __FUNCTION__);

	if (uci_converter_system(command) == RETURN_OK) {
		MAPF_INFO("%s completed successfully\n", __FUNCTION__);
	} else {
		MAPF_ERR("%s failed\n", __FUNCTION__);
		return RETURN_ERR;
	}

	return RETURN_OK;
}

static int bpl_cfg_iface_config_down(int index)
{
	/*TODO: implement using d/s-pal apis*/

	return RETURN_OK;
}

//============================================ STATIC END ============================================

int bpl_cfg_set_wifi_advertise_ssid(const char iface[BPL_IFNAME_LEN], int advertise_ssid)
{
	int retVal = 0;
	int index = 0;
	char hidden_param[MAX_UCI_BUF_LEN] = "hidden";

	if (!iface) {
		return RETURN_ERR;
	}

	retVal = bpl_cfg_get_index_from_interface(iface, &index);
	if (retVal) {
		return retVal;
	}

	retVal |= uci_converter_set_bool(TYPE_VAP, index, hidden_param, !advertise_ssid);

	return retVal;
}

int bpl_cfg_set_wifi_credentials(const char iface[BPL_IFNAME_LEN], const char ssid[BPL_SSID_LEN], const char pass[BPL_PASS_LEN], const char sec[BPL_SEC_LEN])
{
	int index = 0;
	int retVal = 0;

	if (!iface || !ssid || !pass || !sec) {
		return RETURN_ERR;
	}

	retVal = bpl_cfg_get_index_from_interface(iface, &index);
	if (retVal) {
		return retVal;
	}

	if (!strcmp(sec,"WEP-64") || !strcmp(sec,"WEP-128")) {
		/* Set pass as WepKey */
		return bpl_cfg_set_vap_credentials(index, ssid, sec, pass, NULL);
	} else {
		/* Set pass as KeyPassphrase */
		return bpl_cfg_set_vap_credentials(index, ssid, sec, NULL, pass);
	}
}

int bpl_cfg_set_beerocks_credentials(const int radio_dir, const char ssid[BPL_SSID_LEN], const char pass[BPL_PASS_LEN], const char sec[BPL_SEC_LEN])
{
    int retVal = 0;
	char ssid_str[BPL_SSID_LEN] = { 0 }, sec_str[BPL_SEC_LEN] = { 0 }, pass_str[BPL_PASS_LEN] = { 0 };
	char wireless_beerocks_path[MAX_UCI_BUF_LEN] = "wireless.beerocks";
	char ssid_param[MAX_UCI_BUF_LEN] = "ssid";
	char mode_enabled_param[MAX_UCI_BUF_LEN] = "mode_enabled";
	char wep_key_param[MAX_UCI_BUF_LEN] = "wep_key";
	char key_passphrase_param[MAX_UCI_BUF_LEN] = "key_passphrase";

	utils::copy_string(ssid_str, ssid, BPL_SSID_LEN);
	utils::copy_string(sec_str,  sec,  BPL_SEC_LEN);
	utils::copy_string(pass_str, pass, BPL_PASS_LEN);

	retVal |= uci_converter_set(wireless_beerocks_path, ssid_param, ssid_str);
	retVal |= uci_converter_set(wireless_beerocks_path, mode_enabled_param, sec_str);
	if (!strcmp(sec,"WEP-64") || !strcmp(sec,"WEP-128")) {
		retVal |= uci_converter_set(wireless_beerocks_path, wep_key_param, pass_str);
	} else {
		retVal |= uci_converter_set(wireless_beerocks_path, key_passphrase_param, pass_str);
	}
	
	return retVal;
}

int bpl_cfg_set_onboarding(int enable)
{
	return 0;
}

int bpl_cfg_set_wifi_iface_state(const char iface[BPL_IFNAME_LEN], int op)
{
	int retVal = 0;
	int index = 0;

	if (!iface) {
		return RETURN_ERR;
	}

	MAPF_INFO("bpl_cfg_set_wifi_iface_state(%s,%d)\n", iface, op);

	retVal = bpl_cfg_get_index_from_interface(iface, &index);
	if (retVal) {
		MAPF_ERR("bpl_cfg_set_wifi_iface_state: Can't find index from insterface %s\n", iface);
		return retVal;
	}

	if (op) {
		retVal = bpl_cfg_iface_config_up(index);
	} else {
		retVal = bpl_cfg_iface_config_down(index);
	}

	return retVal;
}

int bpl_cfg_set_wifi_radio_tx_state(const char iface[BPL_IFNAME_LEN], int enable)
{
	int retVal = 0;
	int index = 0;
	char disabled_param[MAX_UCI_BUF_LEN] = "disabled";

	if (!iface) {
		return RETURN_ERR;
	}

	retVal = bpl_cfg_get_index_from_interface(iface, &index);
	if (retVal) {
		return retVal;
	}

	retVal = uci_converter_set_bool(TYPE_RADIO, index, disabled_param, !enable);

	return retVal;
}

#else //PASSIVE_MODE is on

int bpl_cfg_set_wifi_advertise_ssid(const char iface[BPL_IFNAME_LEN], int advertise_ssid)
{
	return 0;
}

int bpl_cfg_set_wifi_credentials(const char iface[BPL_IFNAME_LEN], const char ssid[BPL_SSID_LEN], const char pass[BPL_PASS_LEN], const char sec[BPL_SEC_LEN])
{
	return 0;
}

int bpl_cfg_set_beerocks_credentials(const int radio_dir, const char ssid[BPL_SSID_LEN], const char pass[BPL_PASS_LEN], const char sec[BPL_SEC_LEN])
{
    return 0;
}

int bpl_cfg_set_onboarding(int enable)
{
	return 0;
}

int bpl_cfg_set_wifi_iface_state(const char iface[BPL_IFNAME_LEN], int op)
{
	return 0;
}

int bpl_cfg_set_wifi_radio_tx_state(const char iface[BPL_IFNAME_LEN], int enable)
{
	return 0;
}

#endif