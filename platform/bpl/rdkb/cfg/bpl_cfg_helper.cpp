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

#include "bpl_cfg_helper.h"
#include "../../common/utils/utils.h"

extern "C" {
	#include <uci_wrapper.h>
}
#include <slibc/stdio.h>

using namespace beerocks::bpl;

//============================================ STATIC START ============================================

static int bpl_cfg_set_security_mode(int index, const std::string& beaconType, const std::string& authMode) {
	char security_mode[MAX_UCI_BUF_LEN] = { 0 };
	char security_mode_param[MAX_UCI_BUF_LEN] = "wav_security_mode";
	std::string security_mode_str = { 0 };

	if (!beaconType.compare("None")) {
		security_mode_str = "None";
	} else if (!beaconType.compare("Basic")) {
		security_mode_str = "WEP-64";
	} else if (!beaconType.compare("WPA")) {
		if (!authMode.compare("None") || !authMode.compare("PSKAuthentication")) {
			security_mode_str = "WPA-Personal";
		} else if (!authMode.compare("EAPAuthentication")) {
			security_mode_str = "WPA-Enterprise";
		} else {
			return RETURN_ERR;
		}
	} else if (!beaconType.compare("11i")) {
		if (!authMode.compare("None") || !authMode.compare("PSKAuthentication")) {
			security_mode_str = "WPA2-Personal";
		} else if (!authMode.compare("EAPAuthentication")) {
			security_mode_str = "WPA2-Enterprise";
		} else {
			return RETURN_ERR;
		}
	} else if (!beaconType.compare("WPAand11i")) {
		if (!authMode.compare("None") || !authMode.compare("PSKAuthentication")) {
			security_mode_str = "WPA-WPA2-Personal";
		} else if (!authMode.compare("EAPAuthentication")) {
			security_mode_str = "WPA-WPA2-Enterprise";
		} else {
			return RETURN_ERR;
		}
	}

	utils::copy_string(security_mode, security_mode_str.c_str(), MAX_UCI_BUF_LEN);

	return uci_converter_set_str(TYPE_VAP, index, security_mode_param, security_mode);
}

static int bpl_cfg_set_encryption(int index)
{
	int status;
	char security_mode[MAX_UCI_BUF_LEN] = { 0 }, encryption_mode[MAX_UCI_BUF_LEN] = { 0 }, authentication_mode[MAX_UCI_BUF_LEN] = { 0 };
	char empty_str[MAX_UCI_BUF_LEN] = { 0 };
	char security_mode_param[MAX_UCI_BUF_LEN] = "wav_security_mode";
	char encryption_param[MAX_UCI_BUF_LEN] = "encryption";
	char encryption_mode_param[MAX_UCI_BUF_LEN] = "wav_encryption_mode";
	char authentication_mode_param[MAX_UCI_BUF_LEN] = "wav_authentication_mode";

	status = uci_converter_get_str(TYPE_VAP, index, security_mode_param, security_mode);
	if (status == RETURN_ERR)
		return RETURN_ERR;

	uci_converter_get_optional_str(TYPE_VAP, index, encryption_mode_param, encryption_mode, empty_str);
	uci_converter_get_optional_str(TYPE_VAP, index, authentication_mode_param, authentication_mode, empty_str);

	std::string security_mode_str(security_mode, MAX_UCI_BUF_LEN);

	if (!security_mode_str.compare("None")) {
		security_mode_str = "none";
	} else if ((!security_mode_str.compare("WEP-64") || !security_mode_str.compare("WEP-128")) && !is_empty_str(authentication_mode))  {
		security_mode_str = authentication_mode;
	} else if (!security_mode_str.compare("WPA-Personal")) {
		security_mode_str = "psk";
	} else if (!security_mode_str.compare("WPA-Enterprise")) {
		security_mode_str = "wpa";
	} else if (!security_mode_str.compare("WPA2-Personal")) {
		security_mode_str = "psk2";
	} else if (!security_mode_str.compare("WPA2-Enterprise")) {
		security_mode_str = "wpa2";
	} else if (!security_mode_str.compare("WPA-WPA2-Personal")) {
		security_mode_str = "psk-mixed";
	} else if (!security_mode_str.compare("WPA-WPA2-Enterprise")) {
		security_mode_str = "wpa-mixed";
	} else {
		return RETURN_ERR;
	}

	char encryption[MAX_UCI_BUF_LEN] = { 0 };
	if (strnlen(encryption_mode, MAX_UCI_BUF_LEN) == 0) {
		status = snprintf_s(encryption, MAX_UCI_BUF_LEN, "%s", security_mode_str.c_str());
		if (status < 1)
			return RETURN_ERR;
	} else {
		status = snprintf_s(encryption, MAX_UCI_BUF_LEN, "%s/%s", security_mode_str.c_str(), encryption_mode);
		if (status < 2)
			return RETURN_ERR;
	}

	return uci_converter_set_str(TYPE_VAP, index, encryption_param, encryption);
}

static int bpl_cfg_set_wep_key(int index, int keyIndex, const char key[MAX_UCI_BUF_LEN])
{
	/*TODO: implement using d/s-pal apis*/

	//strcpy(key, "\0");
	return RETURN_OK;
}

static int bpl_cfg_set_basic_authentication_modes(int index, const std::string& authMode)
{
	int status;
	char security[MAX_UCI_BUF_LEN] = { 0 }, beaconType[MAX_UCI_BUF_LEN] = { 0 }, wep_empty_str[MAX_UCI_BUF_LEN] = { 0 };
	char security_mode_param[MAX_UCI_BUF_LEN] = "wav_security_mode";
	char beacon_type_param[MAX_UCI_BUF_LEN] = "wav_beacon_type";
	char authentication_mode_param[MAX_UCI_BUF_LEN] = "wav_authentication_mode";
	char wep_str[MAX_UCI_BUF_LEN] = "wep";
	char wep_mixed_str[MAX_UCI_BUF_LEN] = "wep-mixed";
	char wep_shared_str[MAX_UCI_BUF_LEN] = "wep-shared";

	status = uci_converter_get_str(TYPE_VAP, index, beacon_type_param, beaconType);
	if (status == RETURN_ERR)
		return RETURN_ERR;

	status = bpl_cfg_set_security_mode(index, beaconType, authMode);
	if (status == RETURN_ERR)
		return RETURN_ERR;

	if (!authMode.compare("None")) {
		status = uci_converter_get_str(TYPE_VAP, index, security_mode_param, security);
		if (status == RETURN_ERR)
			return RETURN_ERR;
		
		std::string security_str(security);
		if (!security_str.compare("WEP-64") || !security_str.compare("WEP-128")) {
			status = uci_converter_set_str(TYPE_VAP, index, authentication_mode_param, wep_str);
		} else {
			status = uci_converter_set_str(TYPE_VAP, index, authentication_mode_param, wep_empty_str);
		}
	} else if (!authMode.compare("EAPAuthentication")) {
		status = uci_converter_set_str(TYPE_VAP, index, authentication_mode_param, wep_mixed_str);
	} else if (authMode.compare("SharedAuthentication")) {
		status = uci_converter_set_str(TYPE_VAP, index, authentication_mode_param, wep_shared_str);
	} else {
		status = RETURN_ERR;
	}

	if (status == RETURN_ERR)
		return RETURN_ERR;

	return bpl_cfg_set_encryption(index);
}

static int bpl_cfg_set_wpa_encryption_modes(int index, const std::string& encModes)
{
	int status;
	char encryption_mode_empty_val[MAX_UCI_BUF_LEN] = { 0 };
	char encryption_mode_param[MAX_UCI_BUF_LEN] = "wav_encryption_mode";
	char encryption_mode_tkip_val[MAX_UCI_BUF_LEN] = "tkip";
	char encryption_mode_aes_val[MAX_UCI_BUF_LEN] = "aes";
	char encryption_mode_tkip_aes_val[MAX_UCI_BUF_LEN] = "tkip+aes";
	char beacon_type_param[MAX_UCI_BUF_LEN] = "wav_beacon_type";
	char beacon_type_none_val[MAX_UCI_BUF_LEN] = "None";
	char beacon_type_basic_val[MAX_UCI_BUF_LEN] = "Basic";
	char beacon_type_wpa_val[MAX_UCI_BUF_LEN] = "WPA";
	char beacon_type_11i_val[MAX_UCI_BUF_LEN] = "11i";
	char beacon_type_wpa_11i_val[MAX_UCI_BUF_LEN] = "WPAand11i";

	if (!encModes.compare("None")) {
		status = uci_converter_set_str(TYPE_VAP, index, encryption_mode_param, encryption_mode_empty_val);
		status += uci_converter_set_str(TYPE_VAP, index, beacon_type_param, beacon_type_none_val);
		bpl_cfg_set_basic_authentication_modes(index, "None");
	} else if (!encModes.compare("WEPEncryption")) {
		status = uci_converter_set_str(TYPE_VAP, index, encryption_mode_param, encryption_mode_empty_val);
		status += uci_converter_set_str(TYPE_VAP, index, beacon_type_param, beacon_type_basic_val);
		bpl_cfg_set_basic_authentication_modes(index, "None");
	} else if (!encModes.compare("TKIPEncryption")) {
		status = uci_converter_set_str(TYPE_VAP, index, encryption_mode_param, encryption_mode_tkip_val);
		status += uci_converter_set_str(TYPE_VAP, index, beacon_type_param, beacon_type_wpa_val);
	} else if (!encModes.compare("AESEncryption")) {
		status = uci_converter_set_str(TYPE_VAP, index, encryption_mode_param, encryption_mode_aes_val);
		status += uci_converter_set_str(TYPE_VAP, index, beacon_type_param, beacon_type_11i_val);
	} else if (!encModes.compare("TKIPandAESEncryption")) {
		status = uci_converter_set_str(TYPE_VAP, index, encryption_mode_param, encryption_mode_tkip_aes_val);
		status += uci_converter_set_str(TYPE_VAP, index, beacon_type_param, beacon_type_wpa_11i_val);
	} else {
		status = RETURN_ERR;
	}

	if (status == RETURN_ERR)
		return RETURN_ERR;

	return bpl_cfg_set_encryption(index);
}

static int bpl_cfg_set_ap_security_mode_enabled(int index, const std::string& modeEnabled)
{
	int status;
	char mode_enabled[MAX_UCI_BUF_LEN] = { 0 };
	char security_mode_param[MAX_UCI_BUF_LEN] = "wav_security_mode";
	
	utils::copy_string(mode_enabled, modeEnabled.c_str(), MAX_UCI_BUF_LEN);

	status = uci_converter_set_str(TYPE_VAP, index, security_mode_param, mode_enabled);
	if (status == RETURN_ERR)
		return RETURN_ERR;

	if (!modeEnabled.compare("None")) {
		bpl_cfg_set_wpa_encryption_modes(index, "None");
		bpl_cfg_set_basic_authentication_modes(index, "None");
	} else if (!modeEnabled.compare("WEP-64") || !modeEnabled.compare("WEP-128")) {
		bpl_cfg_set_wpa_encryption_modes(index, "WEPEncryption");
		bpl_cfg_set_basic_authentication_modes(index, "None");
	} else if (!modeEnabled.compare("WPA-Personal")) {
		bpl_cfg_set_wpa_encryption_modes(index, "TKIPEncryption");
		bpl_cfg_set_basic_authentication_modes(index, "None");
	} else if (!modeEnabled.compare("WPA-Enterprise")) {
		bpl_cfg_set_wpa_encryption_modes(index, "TKIPEncryption");
		bpl_cfg_set_basic_authentication_modes(index, "EAPAuthentication");
	} else if (!modeEnabled.compare("WPA2-Personal")) {
		bpl_cfg_set_wpa_encryption_modes(index, "AESEncryption");
		bpl_cfg_set_basic_authentication_modes(index, "None");
	} else if (!modeEnabled.compare("WPA2-Enterprise")) {
		bpl_cfg_set_wpa_encryption_modes(index, "AESEncryption");
		bpl_cfg_set_basic_authentication_modes(index, "EAPAuthentication");
	} else if (!modeEnabled.compare("WPA-WPA2-Personal")) {
		bpl_cfg_set_wpa_encryption_modes(index, "TKIPandAESEncryption");
		bpl_cfg_set_basic_authentication_modes(index, "None");
	} else if (!modeEnabled.compare("WPA-WPA2-Enterprise")) {
		bpl_cfg_set_wpa_encryption_modes(index, "TKIPandAESEncryption");
		bpl_cfg_set_basic_authentication_modes(index, "EAPAuthentication");
	} else {
		return RETURN_ERR;
	}

	return bpl_cfg_set_encryption(index);
}

//============================================ STATIC END ============================================

int bpl_cfg_get_index_from_interface(const std::string& inputIfName, int *nIndex)
{
	char ifname[BPL_IFNAME_LEN] = { 0 };
	int rpcIndex = -1;

	if (!nIndex) {
		return RETURN_ERR;
	}
	utils::copy_string(ifname, inputIfName.c_str(), BPL_IFNAME_LEN);

	const int ifType = (inputIfName.find('.') != std::string::npos) ? TYPE_VAP : TYPE_RADIO;

	if (uci_getIndexFromInterface(ifname, &rpcIndex) == RETURN_OK) {
		*nIndex = UCI_RETURN_INDEX(ifType, rpcIndex);
	} else {
		return RETURN_ERR;
	}

	return RETURN_OK;
}

int bpl_cfg_get_beerocks_param(const std::string& param, char *buf, size_t buf_len)
{
	char path[MAX_UCI_BUF_LEN] = { 0 };
	
	if (buf_len > MAX_UCI_BUF_LEN) {
		buf_len = MAX_UCI_BUF_LEN;
	}

	if (snprintf_s(path, MAX_UCI_BUF_LEN, "wireless.beerocks.%s", param.c_str()) <= 0)
		return RETURN_ERR;
	
	return uci_converter_get(path, buf, buf_len);
}

int bpl_cfg_get_beerocks_param_int(const std::string& param, int *buf)
{
	int status;
	char val[MAX_UCI_BUF_LEN] = "";

	status = bpl_cfg_get_beerocks_param(param, val, MAX_UCI_BUF_LEN);
	if (status == RETURN_ERR)
		return RETURN_ERR;

	status = sscanf_s(val, "%d", buf);
	if (status != 1)
		return RETURN_ERR;

	return RETURN_OK;
}

int bpl_cfg_get_auto_channel_enable(int index, int *enable)
{
	char channel[MAX_UCI_BUF_LEN] = { 0 };
	char channel_param[MAX_UCI_BUF_LEN] = "channel";

	if(!enable) {
		return RETURN_ERR;
	}

	int status = uci_converter_get_str(TYPE_RADIO, index, channel_param, channel);
	if (status == RETURN_ERR)
		return RETURN_ERR;

	std::string channel_str(channel);
	if (!channel_str.compare("auto")) {
		*enable = 1;
	} else {
		*enable = 0;
	}

	return RETURN_OK;
}

int bpl_cfg_get_ssid_advertisement_enabled(int index, int *enabled)
{
	bool hidden;
	char hidden_param[MAX_UCI_BUF_LEN] = "hidden";

	int status = uci_converter_get_bool(TYPE_VAP, index, hidden_param, &hidden);
	if(status == RETURN_OK) {
		*enabled = (!hidden) ? 1 : 0;
	}

	return status;
}

int bpl_cfg_get_wep_key(int index, int keyIndex, char *key)
{
	/*TODO: implement using d/s-pal apis*/

	return RETURN_OK;
}

int bpl_cfg_set_vap_credentials(int index, const char ssid[BPL_SSID_LEN], const char sec[BPL_SEC_LEN], const char key[BPL_PASS_LEN], const char psk[BPL_PASS_LEN])
{
	int ret = RETURN_OK;
	char ssid_param[MAX_UCI_BUF_LEN] = "ssid";
	char key_param[MAX_UCI_BUF_LEN] = "key";

	if (ssid) {
		char ssid_str[MAX_UCI_BUF_LEN] = { 0 };
		utils::copy_string(ssid_str, ssid, MAX_UCI_BUF_LEN);
		ret |= uci_converter_set_str(TYPE_VAP, index, ssid_param, ssid_str);
	}
	if (sec) {
		ret |= bpl_cfg_set_ap_security_mode_enabled(index, sec);
	}
	if (key) {
		ret |= bpl_cfg_set_wep_key(index, -1, key);
	}
	if (psk) {
		char psk_str[MAX_UCI_BUF_LEN] = { 0 };
		utils::copy_string(psk_str, psk, MAX_UCI_BUF_LEN);
		ret |= uci_converter_set_str(TYPE_VAP, index, key_param, psk_str);
	}

	return ret;
}