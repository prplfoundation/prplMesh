/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include "bpl_cfg_helper.h"
#include "../../common/utils/utils.h"

extern "C" {
#include "../uci/bpl_cfg_uci.h"
}

#include <slibc/stdio.h>

using namespace beerocks::bpl;

#ifndef BPL_PASSIVE_MODE

static bool bpl_cfg_is_empty_str(const char *str)
{
    if (str == NULL)
        return true;

    return str[0] == '\0';
}

static int bpl_cfg_set_security_mode(int index, const std::string &beaconType,
                                     const std::string &authMode)
{
    char security_mode[MAX_UCI_BUF_LEN] = {0};
    std::string security_mode_str       = {0};

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

    return uci_converter_set_str(TYPE_VAP, index, "wav_security_mode", security_mode);
}

static int bpl_cfg_set_encryption(int index)
{
    int status;
    char security_mode[MAX_UCI_BUF_LEN] = {0}, encryption_mode[MAX_UCI_BUF_LEN] = {0},
         authentication_mode[MAX_UCI_BUF_LEN] = {0};
    char empty_str[MAX_UCI_BUF_LEN]           = {0};

    status = bpl_cfg_uci_get_wireless(TYPE_VAP, index, "wav_security_mode", security_mode);
    if (status == RETURN_ERR)
        return RETURN_ERR;

    uci_converter_get_optional_str(TYPE_VAP, index, "wav_encryption_mode", encryption_mode,
                                   empty_str);
    uci_converter_get_optional_str(TYPE_VAP, index, "wav_authentication_mode", authentication_mode,
                                   empty_str);

    std::string security_mode_str(security_mode, MAX_UCI_BUF_LEN);

    if (!security_mode_str.compare("None")) {
        security_mode_str = "none";
    } else if ((!security_mode_str.compare("WEP-64") || !security_mode_str.compare("WEP-128")) &&
               !bpl_cfg_is_empty_str(authentication_mode)) {
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

    char encryption[MAX_UCI_BUF_LEN] = {0};
    if (strnlen(encryption_mode, MAX_UCI_BUF_LEN) == 0) {
        status = snprintf_s(encryption, MAX_UCI_BUF_LEN, "%s", security_mode_str.c_str());
        if (status < 1)
            return RETURN_ERR;
    } else {
        status = snprintf_s(encryption, MAX_UCI_BUF_LEN, "%s/%s", security_mode_str.c_str(),
                            encryption_mode);
        if (status < 2)
            return RETURN_ERR;
    }

    return uci_converter_set_str(TYPE_VAP, index, "encryption", encryption);
}

static int bpl_cfg_set_wep_key(int index, int keyIndex, const char key[MAX_UCI_BUF_LEN])
{
    /*TODO: implement using d/s-pal apis*/

    //strcpy(key, "\0");
    return RETURN_OK;
}

static int bpl_cfg_set_basic_authentication_modes(int index, const std::string &authMode)
{
    int status;
    char security[MAX_UCI_BUF_LEN] = {0}, beaconType[MAX_UCI_BUF_LEN] = {0},
         wep_empty_str[MAX_UCI_BUF_LEN] = {0};

    status = bpl_cfg_uci_get_wireless(TYPE_VAP, index, "wav_beacon_type", beaconType);
    if (status == RETURN_ERR)
        return RETURN_ERR;

    status = bpl_cfg_set_security_mode(index, beaconType, authMode);
    if (status == RETURN_ERR)
        return RETURN_ERR;

    if (!authMode.compare("None")) {
        status = bpl_cfg_uci_get_wireless(TYPE_VAP, index, "wav_security_mode", security);
        if (status == RETURN_ERR)
            return RETURN_ERR;

        std::string security_str(security);
        if (!security_str.compare("WEP-64") || !security_str.compare("WEP-128")) {
            status = uci_converter_set_str(TYPE_VAP, index, "wav_authentication_mode", "wep");
        } else {
            status =
                uci_converter_set_str(TYPE_VAP, index, "wav_authentication_mode", wep_empty_str);
        }
    } else if (!authMode.compare("EAPAuthentication")) {
        status = uci_converter_set_str(TYPE_VAP, index, "wav_authentication_mode", "wep-mixed");
    } else if (authMode.compare("SharedAuthentication")) {
        status = uci_converter_set_str(TYPE_VAP, index, "wav_authentication_mode", "wep-shared");
    } else {
        status = RETURN_ERR;
    }

    if (status == RETURN_ERR)
        return RETURN_ERR;

    return bpl_cfg_set_encryption(index);
}

static int bpl_cfg_set_wpa_encryption_modes(int index, const std::string &encModes)
{
    int status;
    char encryption_mode_empty_val[MAX_UCI_BUF_LEN] = {0};

    if (!encModes.compare("None")) {
        status = uci_converter_set_str(TYPE_VAP, index, "wav_encryption_mode",
                                       encryption_mode_empty_val);
        status += uci_converter_set_str(TYPE_VAP, index, "wav_beacon_type", "None");
        bpl_cfg_set_basic_authentication_modes(index, "None");
    } else if (!encModes.compare("WEPEncryption")) {
        status = uci_converter_set_str(TYPE_VAP, index, "wav_encryption_mode",
                                       encryption_mode_empty_val);
        status += uci_converter_set_str(TYPE_VAP, index, "wav_beacon_type", "Basic");
        bpl_cfg_set_basic_authentication_modes(index, "None");
    } else if (!encModes.compare("TKIPEncryption")) {
        status = uci_converter_set_str(TYPE_VAP, index, "wav_encryption_mode", "tkip");
        status += uci_converter_set_str(TYPE_VAP, index, "wav_beacon_type", "WPA");
    } else if (!encModes.compare("AESEncryption")) {
        status = uci_converter_set_str(TYPE_VAP, index, "wav_encryption_mode", "aes");
        status += uci_converter_set_str(TYPE_VAP, index, "wav_beacon_type", "11i");
    } else if (!encModes.compare("TKIPandAESEncryption")) {
        status = uci_converter_set_str(TYPE_VAP, index, "wav_encryption_mode", "tkip+aes");
        status += uci_converter_set_str(TYPE_VAP, index, "wav_beacon_type", "WPAand11i");
    } else {
        status = RETURN_ERR;
    }

    if (status == RETURN_ERR)
        return RETURN_ERR;

    return bpl_cfg_set_encryption(index);
}

static int bpl_cfg_set_ap_security_mode_enabled(int index, const std::string &modeEnabled)
{
    int status;
    char mode_enabled[MAX_UCI_BUF_LEN] = {0};

    utils::copy_string(mode_enabled, modeEnabled.c_str(), MAX_UCI_BUF_LEN);

    status = uci_converter_set_str(TYPE_VAP, index, "wav_security_mode", mode_enabled);
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

int bpl_cfg_set_vap_credentials(int index, const char ssid[BPL_SSID_LEN],
                                const char sec[BPL_SEC_LEN], const char key[BPL_PASS_LEN],
                                const char psk[BPL_PASS_LEN])
{
    int ret = RETURN_OK;

    if (ssid) {
        char ssid_str[MAX_UCI_BUF_LEN] = {0};
        utils::copy_string(ssid_str, ssid, MAX_UCI_BUF_LEN);
        ret |= uci_converter_set_str(TYPE_VAP, index, "ssid", ssid_str);
    }
    if (sec) {
        ret |= bpl_cfg_set_ap_security_mode_enabled(index, sec);
    }
    if (key) {
        ret |= bpl_cfg_set_wep_key(index, -1, key);
    }
    if (psk) {
        char psk_str[MAX_UCI_BUF_LEN] = {0};
        utils::copy_string(psk_str, psk, MAX_UCI_BUF_LEN);
        ret |= uci_converter_set_str(TYPE_VAP, index, "key", psk_str);
    }

    return ret;
}

#endif // BPL_PASSIVE_MODE

int bpl_cfg_get_index_from_interface(const std::string &inputIfName, int *nIndex)
{
    char ifname[BPL_IFNAME_LEN] = {0};
    int rpcIndex                = -1;

    if (!nIndex) {
        return RETURN_ERR;
    }
    utils::copy_string(ifname, inputIfName.c_str(), BPL_IFNAME_LEN);

    const int ifType = (inputIfName.find('.') != std::string::npos) ? TYPE_VAP : TYPE_RADIO;

    if (bpl_cfg_uci_get_wireless_idx(ifname, &rpcIndex) == RETURN_OK) {
        *nIndex = UCI_RETURN_INDEX(ifType, rpcIndex);
    } else {
        return RETURN_ERR;
    }

    return RETURN_OK;
}

int bpl_cfg_get_beerocks_param(const std::string &param, char *buf, size_t buf_len)
{
    char path[MAX_UCI_BUF_LEN] = {0};

    if (buf_len > MAX_UCI_BUF_LEN) {
        buf_len = MAX_UCI_BUF_LEN;
    }

    if (snprintf_s(path, MAX_UCI_BUF_LEN, "beerocks.config.%s", param.c_str()) <= 0)
        return RETURN_ERR;

    return bpl_cfg_uci_get(path, buf, buf_len);
}

int bpl_cfg_get_beerocks_param_int(const std::string &param, int *buf)
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

int bpl_cfg_get_channel(int index, int *channel)
{
    if (!channel) {
        return RETURN_ERR;
    }

    char channel_num[MAX_UCI_BUF_LEN] = {0};
    int status = bpl_cfg_uci_get_wireless(TYPE_RADIO, index, "channel", channel_num);
    if (status == RETURN_ERR)
        return RETURN_ERR;

    std::string channel_str(channel_num);
    if (!channel_str.compare("auto")) {
        *channel = 0;
    } else {
        *channel = utils::stoi(channel_str);
    }

    return RETURN_OK;
}

int bpl_cfg_get_ssid_advertisement_enabled(int index, int *enabled)
{
    bool hidden;

    int status = bpl_cfg_uci_get_wireless_bool(TYPE_VAP, index, "hidden", &hidden);
    if (status == RETURN_OK) {
        *enabled = (!hidden) ? 1 : 0;
    }

    return status;
}

int bpl_cfg_get_wep_key(int index, int keyIndex, char *key)
{
    /*TODO: implement using d/s-pal apis*/
    return RETURN_OK;
}