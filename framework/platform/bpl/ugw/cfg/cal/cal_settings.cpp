/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef LOGGING_ID
#define LOGGING_ID WIDAN_SLAVE
#endif

#ifndef PACKAGE_ID
#define PACKAGE_ID "0"
#endif

#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <map>
#include <stdexcept>
#include <streambuf>
#include <string>
#include <unistd.h>
#include <vector>

#pragma GCC diagnostic ignored "-Wattributes"
#include "calpp.h"

#include "cal_settings.h"

#include "../../../common/utils/utils.h"
#include "../../../common/utils/utils_net.h"
#include <bpl/bpl_cfg.h>

#include <mapf/common/logger.h>

namespace beerocks {
namespace bpl {

const std::string DEVICE_IP_NAME("Device.IP");
const std::string DEVICE_IP_INTERFACE_NAME("Device.IP.Interface");
const std::string DEVICE_INFO_OBJECT_NAME("Device.DeviceInfo");
const std::string DEVICE_WIFI_OBJECT_NAME("Device.WiFi");
const std::string DEVICE_WIFI_RADIO_OBJECT_NAME("Device.WiFi.Radio");
const std::string DEVICE_WIFI_SSID_OBJECT_NAME("Device.WiFi.SSID");
const std::string DEVICE_WIFI_ACCESSPOINT_OBJECT_NAME("Device.WiFi.AccessPoint");
const std::string SECURITY_OBJECT_NAME("Security");

const std::string DEVICE_BEEROCKS_OBJECT_NAME("Device.X_INTEL_COM_BEEROCKS");
const std::string
    DEVICE_BEEROCKS_FRONT_WIFI_OBJECT_NAME("Device.X_INTEL_COM_BEEROCKS.WiFi.Front.Ap.1");
const std::string DEVICE_BEEROCKS_BACK_WIFI_OBJECT_NAME("Device.X_INTEL_COM_BEEROCKS.WiFi.Back");

const std::string DEVICE_BEEROCKS_USER_PASSWORD_OBJECT_NAME("Device.Users.User.1");

#define DEFAULT_INTERFACE_LENGTH 5

class cal_query_wifi {
public:
    cal_query_wifi(cal_interface *cal) : m_cal(cal), m_fake_bssid_suffix(0) {}

    int get_ip_interface_count()
    {
        cal_message msg(MOPT_GET);

        msg.add_get_object(DEVICE_IP_NAME).add_get_param("InterfaceNumberOfEntries");

        m_cal->cal_getValue(msg);

        auto obj_it             = msg.begin();
        auto ip_interface_count = utils::stoi(obj_it->get_param("InterfaceNumberOfEntries", "0"));

        // MAPF_DBG("ip interface count: " << ip_interface_count);
        return ip_interface_count;
    }

    bool compare_ssidrefs(int ap_number, const std::string &ssidref)
    {
        std::string object_name =
            DEVICE_WIFI_ACCESSPOINT_OBJECT_NAME + "." + std::to_string(ap_number);

        cal_message msg(MOPT_GET);

        msg.add_get_object(object_name).add_get_param("SSIDReference");

        int rv = m_cal->cal_getValue(msg);
        if (rv < 0) {
            MAPF_ERR("Getting ssidref values returned error : " << rv);
            return rv;
        }

        auto obj_it                  = msg.begin();
        std::string current_ssid_ref = obj_it->get_param("SSIDReference");

        if (current_ssid_ref == ssidref)
            return true;
        else
            return false;
    }

    int get_ap_index_using_ssidref(const std::string &ssidref)
    {
        cal_message msg(MOPT_GET, SOPT_LEAFNODE);
        int ap_index = -1;

        msg.add_get_object(DEVICE_WIFI_ACCESSPOINT_OBJECT_NAME);
        int rv = m_cal->cal_getValue(msg);
        if (rv < 0) {
            MAPF_ERR("Getting accesspoint entries values returned error : " << rv);
            return rv;
        }

        auto obj_list = msg.get_object_list();
        for (auto obj : obj_list) {
            auto name      = obj.get_name();
            auto index_str = name.substr(name.find_last_of('.') + 1);
            auto index     = atoi(index_str.c_str());
            if (compare_ssidrefs(index, ssidref)) {
                ap_index = index;
            }
        }

        return ap_index;
    }

    bool get_wifi_ssid_and_security_entry(int index, cal_settings::SAPSettings &ap_settings)
    {
        std::string object_name = DEVICE_WIFI_SSID_OBJECT_NAME + "." + std::to_string(index);

        cal_message msg(MOPT_GET);
        msg.add_get_object(object_name)
            .add_get_param("Enable")
            .add_get_param("Status")
            .add_get_param("Name")
            .add_get_param("LowerLayers")
            .add_get_param("BSSID")
            .add_get_param("MACAddress")
            .add_get_param("SSID");

        int rv      = m_cal->cal_getValue(msg);
        auto obj_it = msg.begin();

        // Set the SSID
        ap_settings.ssid = obj_it->get_param("SSID");

        cal_message radio_msg(MOPT_GET);
        msg.add_get_object(obj_it->get_param("LowerLayers")).add_get_param("Name");
        auto radio_obj_it = msg.begin();

        // Finding the index of ap whose ssidref corresponds to this ssid
        // Getting it here, so that we can fill the full ap_Settings
        // and return as a whole

        int index_ap = get_ap_index_using_ssidref(object_name);

        if (index_ap > 0) {
            auto index_suffix = std::to_string(index_ap);
            std::string object_name_accesspoint =
                DEVICE_WIFI_ACCESSPOINT_OBJECT_NAME + "." + index_suffix;

            cal_message msg_accesspoint(MOPT_GET);
            msg_accesspoint.add_get_object(object_name_accesspoint)
                .add_get_param("SSIDAdvertisementEnabled");

            rv                      = m_cal->cal_getValue(msg_accesspoint);
            auto accesspoint_obj_it = msg_accesspoint.begin();

            std::string object_name_security = DEVICE_WIFI_ACCESSPOINT_OBJECT_NAME + "." +
                                               index_suffix + "." + SECURITY_OBJECT_NAME;

            cal_message msg_security(MOPT_GET, SOPT_OBJVALUE | GET_PWD_VAL);
            msg_security.add_get_object(object_name_security)
                .add_get_param("ModeEnabled")
                .add_get_param("WEPKey")
                .add_get_param("PreSharedKey")
                .add_get_param("KeyPassphrase")
                .add_get_param("RekeyingInterval");

            rv = m_cal->cal_getValue(msg_security);
            if (rv < 0) {
                MAPF_ERR("Getting security values returned error : " << rv);
                return false;
            }

            // debug_log(msg_security);

            auto security_obj_it = msg_security.begin();

            ap_settings.sec_mode = security_obj_it->get_param("ModeEnabled");
            ap_settings.pass     = security_obj_it->get_param("KeyPassphrase");
            // ap.set_wep_key(security_obj_it->get_param("WEPKey"));
            // ap.set_psk(security_obj_it->get_param("PreSharedKey"));
            // ap.set_rekeying_interval(atoi(security_obj_it->get_param("RekeyingInterval").c_str()));

            return true;

        } else {
            MAPF_ERR("No AP found with the SSIDRef, security entries will not be sent ");
        }

        return false;
    }

    bool get_wifi_radio_entry(std::string iface_name, cal_settings::SAPSettings &ap_settings)
    {
        // TODO: int=>string should use std::to_string(line), but not working on GRX toolchain
        auto ip_interface_count = get_ip_interface_count();

        for (int i = 1; i < ip_interface_count; i++) {
            std::string object_name = DEVICE_WIFI_RADIO_OBJECT_NAME + "." + std::to_string(i);

            cal_message msg(MOPT_GET);
            msg.add_get_object(object_name)
                .add_get_param("Name")
                .add_get_param("Enable")
                // .add_get_param("Status")
                // .add_get_param("OperatingFrequencyBand")
                // .add_get_param("SupportedFrequencyBands")
                // .add_get_param("PossibleChannels")
                // .add_get_param("ChannelsInUse")
                // .add_get_param("Channel")
                // .add_get_param("AutoChannelRefreshPeriod")
                // .add_get_param("OperatingChannelBandwidth")
                // .add_get_param("ExtensionChannel")
                .add_get_param("AutoChannelEnable");

            int rv = m_cal->cal_getValue(msg);

            // Dump Returned Contents to stdout to see what we got
            // MAPF_INFO("get_wifi_radio_entry[" << i << "] result: " << rv);
            // debug_log(msg);

            // ap_radio_settings radio;
            if (0 == rv) {
                auto obj_it = msg.begin();

                // Skip not the requested interfaces
                if (iface_name != obj_it->get_param("Name", ""))
                    continue;

                // TODO: Can "Enable" equal "true" as well?
                ap_settings.enabled =
                    (obj_it->get_param("Enable") == "1") || (obj_it->get_param("Enable") == "true");
                ap_settings.acs = (obj_it->get_param("AutoChannelEnable") == "true");

                // Get SSID and Password
                return get_wifi_ssid_and_security_entry(i, ap_settings);
            }
        }

        return false;
    }

    bool get_backhaul_params(int &max_vaps, int &network_enabled, int &prefered_radio_band)
    {

        cal_message msg(MOPT_GET, SOPT_OBJVALUE);
        msg.add_get_object(DEVICE_BEEROCKS_BACK_WIFI_OBJECT_NAME)
            .add_get_param("MaxBackVaps")
            .add_get_param("BackhaulPreferedRadioBand")
            .add_get_param("BackhaulNetworkEnabled");

        int ret = m_cal->cal_getValue(msg);
        if (ret < 0) {
            MAPF_ERR("Failed reading CAL value: " << ret);
            return false;
        }

        // Dump Returned Contents to stdout to see what we got
        // debug_log(msg);

        auto obj_it = msg.begin();

        max_vaps        = utils::stoi(obj_it->get_param("MaxBackVaps"));
        network_enabled = (obj_it->get_param("BackhaulNetworkEnabled") == "true");
        prefered_radio_band =
            (obj_it->get_param("BackhaulPreferedRadioBand") == "5G" ? BPL_RADIO_BAND_5G
                                                                    : BPL_RADIO_BAND_2G);

        return true;
    }

    int get_backhaul_vaps(char *backhaul_vaps_buf, const int buf_len)
    {
        char *buff_last = backhaul_vaps_buf + buf_len;
        for (uint8_t i = 1; i <= BPL_BACK_VAPS_GROUPS; i++) {
            cal_message msg(MOPT_GET, SOPT_OBJVALUE);
            msg.add_get_object(DEVICE_BEEROCKS_BACK_WIFI_OBJECT_NAME + ".Ap." + std::to_string(i))
                .add_get_param("BSSID_2G")
                .add_get_param("BSSID_5G")
                .add_get_param("BSSID_5G_Secondary");

            int ret = m_cal->cal_getValue(msg);
            if (ret < 0) {
                MAPF_ERR("Failed reading CAL value: " << ret);
                return false;
            }

            // Dump Returned Contents to stdout to see what we got
            // debug_log(msg);

            auto obj_it = msg.begin();

            utils::mac_from_string((uint8_t *)backhaul_vaps_buf, obj_it->get_param("BSSID_2G"));
            backhaul_vaps_buf += BPL_MAC_ADDR_OCTETS_LEN;
            utils::mac_from_string((uint8_t *)backhaul_vaps_buf, obj_it->get_param("BSSID_5G"));
            backhaul_vaps_buf += BPL_MAC_ADDR_OCTETS_LEN;
            utils::mac_from_string((uint8_t *)backhaul_vaps_buf,
                                   obj_it->get_param("BSSID_5G_Secondary"));
            backhaul_vaps_buf += BPL_MAC_ADDR_OCTETS_LEN;
        }

        if (backhaul_vaps_buf > buff_last) {
            MAPF_ERR("detected memory segementation violation!");
        }

        return true;
    }

    int beerocks_is_master()
    {
        cal_message msg(MOPT_GET);
        std::string param("ManagementMode");
        msg.add_get_object(DEVICE_BEEROCKS_OBJECT_NAME).add_get_param(param);

        int ret = m_cal->cal_getValue(msg);
        if (ret < 0) {
            MAPF_ERR("Failed reading CAL value: " << ret);
            return -1;
        }

        // Dump Returned Contents to stdout to see what we got
        // debug_log(msg);

        auto obj_it   = msg.begin();
        auto mode_str = obj_it->get_param(param);
        if (mode_str == "Multi-AP-Controller-and-Agent") {
            return 1;
        } else if (mode_str == "Multi-AP-Agent") {
            return 0;
        } else {
            debug_log(msg);
            return -1;
        }
    }

    int beerocks_get_operating_mode()
    {
        cal_message msg(MOPT_GET);
        std::string param("OperatingMode");
        msg.add_get_object(DEVICE_BEEROCKS_OBJECT_NAME).add_get_param(param);

        int ret = m_cal->cal_getValue(msg);
        if (ret < 0) {
            MAPF_ERR("Failed reading CAL value: " << ret);
            return -1;
        }

        // Dump Returned Contents to stdout to see what we got
        // debug_log(msg);

        auto obj_it   = msg.begin();
        auto mode_str = obj_it->get_param(param);
        if (mode_str == "Gateway") {
            return BPL_OPER_MODE_GATEWAY;
        } else if (mode_str == "Gateway-WISP") {
            return BPL_OPER_MODE_GATEWAY_WISP;
        } else if (mode_str == "WDS-Extender") {
            return BPL_OPER_MODE_WDS_EXTENDER;
        } else if (mode_str == "WDS-Repeater") {
            return BPL_OPER_MODE_WDS_REPEATER;
        } else if (mode_str == "L2NAT-Client") {
            return BPL_OPER_MODE_L2NAT_CLIENT;
        } else {
            debug_log(msg);
            return -1;
        }
    }

    int beerocks_read_bool_value(std::string param)
    {
        cal_message msg(MOPT_GET);
        msg.add_get_object(DEVICE_BEEROCKS_OBJECT_NAME).add_get_param(param);

        int ret = m_cal->cal_getValue(msg);
        if (ret < 0) {
            MAPF_ERR("Failed reading CAL value: " << ret);
            return -1;
        }

        // Dump Returned Contents to stdout to see what we got
        // debug_log(msg);

        auto obj_it   = msg.begin();
        auto mode_str = obj_it->get_param(param, "true");
        if (mode_str == "true") {
            return 1;
        } else if (mode_str == "false") {
            return 0;
        } else {
            debug_log(msg);
            return -1;
        }
    }

    int beerocks_get_enable() { return (beerocks_read_bool_value("Enable")); }

    int beerocks_is_onboarding() { return (beerocks_read_bool_value("Onboarding")); }

    int beerocks_get_band_steering() { return (beerocks_read_bool_value("BandSteering")); }

    int beerocks_get_client_roaming() { return (beerocks_read_bool_value("ClientRoaming")); }

    int beerocks_get_device_info(cal_settings::SDeviceInfo &device_info)
    {
        cal_message msg(MOPT_GET, SOPT_OBJVALUE);

        // 1st object - Generic Info
        msg.add_get_object(DEVICE_INFO_OBJECT_NAME)
            .add_get_param("Manufacturer")
            .add_get_param("ModelName")
            .add_get_param("SerialNumber");

        // 2nd object - Device Interfaces
        msg.add_get_object(DEVICE_IP_INTERFACE_NAME);

        int ret = m_cal->cal_getValue(msg);
        if (ret < 0) {
            MAPF_ERR("Failed reading CAL value: " << ret);
            return -1;
        }

        std::string lan_iface_ip_obj;
        std::string wan_iface_ip_obj;

        auto obj_list = msg.get_object_list();
        for (auto obj : obj_list) {
            auto name = obj.get_name();

            // Generic info
            if (name == "Device.DeviceInfo.") {
                device_info.manufacturer  = obj.get_param("Manufacturer");
                device_info.model_name    = obj.get_param("ModelName");
                device_info.serial_number = obj.get_param("SerialNumber");
            } else {

                // LAN - Look for the bridge interface
                if (obj.get_param("X_LANTIQ_COM_UpStream") == "bridge") {
                    device_info.lan_interface_name = obj.get_param("Name");
                    lan_iface_ip_obj               = "Device.IP.Interface." +
                                       name.substr(name.find_last_of('.') + 1) + ".IPv4Address.1";
                } else if (name == lan_iface_ip_obj) {
                    if (!device_info.lan_interface_name.empty()) {
                        device_info.lan_interface_ip      = obj.get_param("IPAddress");
                        device_info.lan_interface_netmask = obj.get_param("SubnetMask");
                    }
                }

                // WAN - Look for the "Default Gateway" interface
                if (obj.get_param("X_LANTIQ_COM_DefaultGateway") == "true") {
                    device_info.wan_interface_name = obj.get_param("Name");
                    wan_iface_ip_obj               = "Device.IP.Interface." +
                                       name.substr(name.find_last_of('.') + 1) + ".IPv4Address.1";
                } else if (name == wan_iface_ip_obj) {
                    if (!device_info.wan_interface_name.empty()) {
                        device_info.wan_interface_ip      = obj.get_param("IPAddress");
                        device_info.wan_interface_netmask = obj.get_param("SubnetMask");
                    }
                }
            }

            // auto index_str = name.substr(name.find_last_of('.') + 1);
            // MAPF_DBG("Object Name: " << name << ", Index: " << index_str);
            // MAPF_DBG("Name: " << obj.get_param("Name"));
            // MAPF_DBG("IPAddress: " << obj.get_param("IPAddress"));
        }

        // MAPF_DBG("LAN Interface: " << device_info.lan_interface_name << " (" << device_info.lan_interface_ip << ")");
        // MAPF_DBG("WAN Interface: " << device_info.wan_interface_name << " (" << device_info.wan_interface_ip << ")");

        return 0;
    }

    int beerocks_get_wifi_credentials(const int radio_dir, std::string &ssid, std::string &pass,
                                      std::string &sec)
    {
        cal_message msg(MOPT_GET, SOPT_OBJVALUE | GET_PWD_VAL);
        msg.add_get_object((radio_dir ? DEVICE_BEEROCKS_BACK_WIFI_OBJECT_NAME
                                      : DEVICE_BEEROCKS_FRONT_WIFI_OBJECT_NAME))
            .add_get_param("ModeEnabled")
            .add_get_param("SSID")
            .add_get_param("WEPKey")
            .add_get_param("KeyPassphrase");

        int ret = m_cal->cal_getValue(msg);
        if (ret < 0) {
            MAPF_ERR("Failed reading CAL value: " << ret);
            return -1;
        }

        // Dump Returned Contents to stdout to see what we got
        // debug_log(msg);

        auto obj_it = msg.begin();

        ssid = obj_it->get_param("SSID");
        sec  = obj_it->get_param("ModeEnabled");

        if (sec == "None") {
            pass = "";
        } else if (sec == "WEP-64" || sec == "WEP-128") {
            pass = obj_it->get_param("WEPKey");
        } else {
            pass = obj_it->get_param("KeyPassphrase");
        }

        // MAPF_INFO("*** TEMP *** SSID: " << ssid);
        // MAPF_INFO("*** TEMP *** Pass: " << pass);
        // MAPF_INFO("*** TEMP *** Sec: " << sec);

        return 0;
    }

    int beerocks_get_administrator_credentials(std::string &pass)
    {
        cal_message msg(MOPT_GET, SOPT_OBJVALUE | GET_PWD_VAL);
        msg.add_get_object(DEVICE_BEEROCKS_USER_PASSWORD_OBJECT_NAME).add_get_param("Password");

        int ret = m_cal->cal_getValue(msg);
        if (ret < 0) {
            MAPF_ERR("Failed reading CAL value: " << ret);
            return -1;
        }

        // Dump Returned Contents to stdout to see what we got
        // debug_log(msg);

        auto obj_it = msg.begin();

        pass = obj_it->get_param("Password");

        // MAPF_INFO("*** TEMP *** Pass: " << pass);

        return 0;
    }

    int beerocks_is_wired_backhaul() { return (beerocks_read_bool_value("WiredBackhaul")); }

private:
    cal_interface *m_cal;
    uint8_t m_fake_bssid_suffix;
};

//============================================================================
// cal_settings implementation
//============================================================================

cal_settings::cal_settings(cal_interface *cal) : m_cal(cal)
{
    if (nullptr == cal) {
        m_cal_owner.reset(new cal_interface);
        m_cal = m_cal_owner.get();
    }
}

cal_settings::~cal_settings() {}

int cal_settings::beerocks_is_enabled()
{
    cal_query_wifi cal(m_cal);
    return (cal.beerocks_get_enable());
}

int cal_settings::beerocks_is_master()
{
    cal_query_wifi cal(m_cal);
    return (cal.beerocks_is_master());
}

int cal_settings::beerocks_get_operating_mode()
{
    cal_query_wifi cal(m_cal);
    return (cal.beerocks_get_operating_mode());
}

int cal_settings::beerocks_is_onboarding()
{
    cal_query_wifi cal(m_cal);
    return (cal.beerocks_is_onboarding());
}

int cal_settings::beerocks_get_band_steering()
{
    cal_query_wifi cal(m_cal);
    return (cal.beerocks_get_band_steering());
}

int cal_settings::beerocks_get_client_roaming()
{
    cal_query_wifi cal(m_cal);
    return (cal.beerocks_get_client_roaming());
}

int cal_settings::beerocks_get_device_info(SDeviceInfo &device_info)
{
    cal_query_wifi cal(m_cal);
    return (cal.beerocks_get_device_info(device_info));
}

int cal_settings::beerocks_get_wifi_params(std::string iface_name,
                                           cal_settings::SAPSettings &ap_settings)
{
    cal_query_wifi cal_query(m_cal);

    if (!cal_query.get_wifi_radio_entry(iface_name, ap_settings)) {
        return -1;
    }

    return 0;
}

int cal_settings::beerocks_get_backhaul_params(int &max_vaps, int &network_enabled,
                                               int &prefered_radio_band)
{
    cal_query_wifi cal_query(m_cal);

    if (!cal_query.get_backhaul_params(max_vaps, network_enabled, prefered_radio_band)) {
        return -1;
    }

    return 0;
}

int cal_settings::beerocks_get_backhaul_vaps(char *backhaul_vaps_buf, const int buf_len)
{
    cal_query_wifi cal_query(m_cal);

    if (!cal_query.get_backhaul_vaps(backhaul_vaps_buf, buf_len)) {
        return -1;
    }

    return 0;
}

int cal_settings::beerocks_get_wifi_credentials(int radio_dir, std::string &ssid, std::string &pass,
                                                std::string &sec)
{
    cal_query_wifi cal_query(m_cal);
    return (cal_query.beerocks_get_wifi_credentials(radio_dir, ssid, pass, sec));
}

int cal_settings::beerocks_set_onboarding(int enable)
{
    cal_message msg(MOPT_SET, SOPT_OBJVALUE,
                    OWN_WEB); // set OWN_WEB will make the platform to onboard
    auto cal_obj = msg.add_set_object(DEVICE_BEEROCKS_OBJECT_NAME, OBJOPT_MODIFY);
    cal_obj.add_set_param("Onboarding", enable > 0 ? "true" : "false");
    int rv = m_cal->cal_setValue(msg);

    if (rv < 0) {
        MAPF_ERR("Failed setting Onboarding val: " << rv);
        return -1;
    }

    return 0;
}

int cal_settings::beerocks_get_administrator_credentials(std::string &pass)
{
    cal_query_wifi cal(m_cal);
    return (cal.beerocks_get_administrator_credentials(pass));
}

int cal_settings::beerocks_is_wired_backhaul()
{
    cal_query_wifi cal(m_cal);
    return (cal.beerocks_is_wired_backhaul());
}

} // namespace bpl
} // namespace beerocks
