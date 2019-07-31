///////////////////////////////////////
// AUTO GENERATED FILE - DO NOT EDIT //
///////////////////////////////////////

/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _BEEROCKS_TLVF_BEEROCKS_MESSAGE_PLATFORM_H_
#define _BEEROCKS_TLVF_BEEROCKS_MESSAGE_PLATFORM_H_

#include <cstddef>
#include <stdint.h>
#include <tlvf/swap.h>
#include <string.h>
#include <memory>
#include <tlvf/BaseClass.h>
#include <tuple>
#include <tlvf/tlvfutils.h>
#include "beerocks/tlvf/beerocks_message_common.h"

namespace beerocks_message {


class cACTION_PLATFORM_SON_SLAVE_BACKHAUL_CONNECTION_COMPLETE_NOTIFICATION : public BaseClass
{
    public:
        cACTION_PLATFORM_SON_SLAVE_BACKHAUL_CONNECTION_COMPLETE_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_PLATFORM_SON_SLAVE_BACKHAUL_CONNECTION_COMPLETE_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_PLATFORM_SON_SLAVE_BACKHAUL_CONNECTION_COMPLETE_NOTIFICATION();

        static eActionOp_PLATFORM get_action_op(){
            return (eActionOp_PLATFORM)(ACTION_PLATFORM_SON_SLAVE_BACKHAUL_CONNECTION_COMPLETE_NOTIFICATION);
        }
        uint8_t& is_backhaul_manager();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_PLATFORM* m_action_op = nullptr;
        uint8_t* m_is_backhaul_manager = nullptr;
};

class cACTION_PLATFORM_SON_SLAVE_REGISTER_REQUEST : public BaseClass
{
    public:
        cACTION_PLATFORM_SON_SLAVE_REGISTER_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_PLATFORM_SON_SLAVE_REGISTER_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_PLATFORM_SON_SLAVE_REGISTER_REQUEST();

        static eActionOp_PLATFORM get_action_op(){
            return (eActionOp_PLATFORM)(ACTION_PLATFORM_SON_SLAVE_REGISTER_REQUEST);
        }
        char* iface_name(size_t length = 0);
        bool set_iface_name(std::string& str);
        bool set_iface_name(const std::string& str);
        bool set_iface_name(const char buffer[], size_t size);
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_PLATFORM* m_action_op = nullptr;
        char* m_iface_name = nullptr;
        size_t m_iface_name_idx__ = 0;
        int m_lock_order_counter__ = 0;
};

class cACTION_PLATFORM_SON_SLAVE_REGISTER_RESPONSE : public BaseClass
{
    public:
        cACTION_PLATFORM_SON_SLAVE_REGISTER_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_PLATFORM_SON_SLAVE_REGISTER_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_PLATFORM_SON_SLAVE_REGISTER_RESPONSE();

        static eActionOp_PLATFORM get_action_op(){
            return (eActionOp_PLATFORM)(ACTION_PLATFORM_SON_SLAVE_REGISTER_RESPONSE);
        }
        sPlatformSettings& platform_settings();
        sWlanSettings& wlan_settings();
        uint32_t& valid();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_PLATFORM* m_action_op = nullptr;
        sPlatformSettings* m_platform_settings = nullptr;
        sWlanSettings* m_wlan_settings = nullptr;
        uint32_t* m_valid = nullptr;
};

class cACTION_PLATFORM_POST_INIT_CONFIG_REQUEST : public BaseClass
{
    public:
        cACTION_PLATFORM_POST_INIT_CONFIG_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_PLATFORM_POST_INIT_CONFIG_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_PLATFORM_POST_INIT_CONFIG_REQUEST();

        static eActionOp_PLATFORM get_action_op(){
            return (eActionOp_PLATFORM)(ACTION_PLATFORM_POST_INIT_CONFIG_REQUEST);
        }
        char* iface_name(size_t length = 0);
        bool set_iface_name(std::string& str);
        bool set_iface_name(const std::string& str);
        bool set_iface_name(const char buffer[], size_t size);
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_PLATFORM* m_action_op = nullptr;
        char* m_iface_name = nullptr;
        size_t m_iface_name_idx__ = 0;
        int m_lock_order_counter__ = 0;
};

class cACTION_PLATFORM_POST_INIT_CONFIG_RESPONSE : public BaseClass
{
    public:
        cACTION_PLATFORM_POST_INIT_CONFIG_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_PLATFORM_POST_INIT_CONFIG_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_PLATFORM_POST_INIT_CONFIG_RESPONSE();

        static eActionOp_PLATFORM get_action_op(){
            return (eActionOp_PLATFORM)(ACTION_PLATFORM_POST_INIT_CONFIG_RESPONSE);
        }
        uint8_t& result();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_PLATFORM* m_action_op = nullptr;
        uint8_t* m_result = nullptr;
};

class cACTION_PLATFORM_ARP_MONITOR_NOTIFICATION : public BaseClass
{
    public:
        cACTION_PLATFORM_ARP_MONITOR_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_PLATFORM_ARP_MONITOR_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_PLATFORM_ARP_MONITOR_NOTIFICATION();

        static eActionOp_PLATFORM get_action_op(){
            return (eActionOp_PLATFORM)(ACTION_PLATFORM_ARP_MONITOR_NOTIFICATION);
        }
        sArpMonitorData& params();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_PLATFORM* m_action_op = nullptr;
        sArpMonitorData* m_params = nullptr;
};

class cACTION_PLATFORM_WLAN_PARAMS_CHANGED_NOTIFICATION : public BaseClass
{
    public:
        cACTION_PLATFORM_WLAN_PARAMS_CHANGED_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_PLATFORM_WLAN_PARAMS_CHANGED_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_PLATFORM_WLAN_PARAMS_CHANGED_NOTIFICATION();

        static eActionOp_PLATFORM get_action_op(){
            return (eActionOp_PLATFORM)(ACTION_PLATFORM_WLAN_PARAMS_CHANGED_NOTIFICATION);
        }
        sWlanSettings& wlan_settings();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_PLATFORM* m_action_op = nullptr;
        sWlanSettings* m_wlan_settings = nullptr;
};

class cACTION_PLATFORM_DHCP_MONITOR_NOTIFICATION : public BaseClass
{
    public:
        cACTION_PLATFORM_DHCP_MONITOR_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_PLATFORM_DHCP_MONITOR_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_PLATFORM_DHCP_MONITOR_NOTIFICATION();

        static eActionOp_PLATFORM get_action_op(){
            return (eActionOp_PLATFORM)(ACTION_PLATFORM_DHCP_MONITOR_NOTIFICATION);
        }
        eDHCPOp& dhcp_op();
        uint32_t& op();
        sMacAddr& mac();
        beerocks::net::sIpv4Addr& ipv4();
        char* hostname(size_t length = 0);
        bool set_hostname(std::string& str);
        bool set_hostname(const std::string& str);
        bool set_hostname(const char buffer[], size_t size);
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_PLATFORM* m_action_op = nullptr;
        eDHCPOp* m_dhcp_op = nullptr;
        uint32_t* m_op = nullptr;
        sMacAddr* m_mac = nullptr;
        beerocks::net::sIpv4Addr* m_ipv4 = nullptr;
        char* m_hostname = nullptr;
        size_t m_hostname_idx__ = 0;
        int m_lock_order_counter__ = 0;
};

class cACTION_PLATFORM_CHANGE_MODULE_LOGGING_LEVEL : public BaseClass
{
    public:
        cACTION_PLATFORM_CHANGE_MODULE_LOGGING_LEVEL(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_PLATFORM_CHANGE_MODULE_LOGGING_LEVEL(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_PLATFORM_CHANGE_MODULE_LOGGING_LEVEL();

        static eActionOp_PLATFORM get_action_op(){
            return (eActionOp_PLATFORM)(ACTION_PLATFORM_CHANGE_MODULE_LOGGING_LEVEL);
        }
        sLoggingLevelChange& params();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_PLATFORM* m_action_op = nullptr;
        sLoggingLevelChange* m_params = nullptr;
};

class cACTION_PLATFORM_ARP_QUERY_REQUEST : public BaseClass
{
    public:
        cACTION_PLATFORM_ARP_QUERY_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_PLATFORM_ARP_QUERY_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_PLATFORM_ARP_QUERY_REQUEST();

        static eActionOp_PLATFORM get_action_op(){
            return (eActionOp_PLATFORM)(ACTION_PLATFORM_ARP_QUERY_REQUEST);
        }
        sArpQuery& params();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_PLATFORM* m_action_op = nullptr;
        sArpQuery* m_params = nullptr;
};

class cACTION_PLATFORM_ARP_QUERY_RESPONSE : public BaseClass
{
    public:
        cACTION_PLATFORM_ARP_QUERY_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_PLATFORM_ARP_QUERY_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_PLATFORM_ARP_QUERY_RESPONSE();

        static eActionOp_PLATFORM get_action_op(){
            return (eActionOp_PLATFORM)(ACTION_PLATFORM_ARP_QUERY_RESPONSE);
        }
        sArpMonitorData& params();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_PLATFORM* m_action_op = nullptr;
        sArpMonitorData* m_params = nullptr;
};

class cACTION_PLATFORM_ONBOARD_QUERY_REQUEST : public BaseClass
{
    public:
        cACTION_PLATFORM_ONBOARD_QUERY_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_PLATFORM_ONBOARD_QUERY_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_PLATFORM_ONBOARD_QUERY_REQUEST();

        static eActionOp_PLATFORM get_action_op(){
            return (eActionOp_PLATFORM)(ACTION_PLATFORM_ONBOARD_QUERY_REQUEST);
        }
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_PLATFORM* m_action_op = nullptr;
};

class cACTION_PLATFORM_ONBOARD_QUERY_RESPONSE : public BaseClass
{
    public:
        cACTION_PLATFORM_ONBOARD_QUERY_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_PLATFORM_ONBOARD_QUERY_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_PLATFORM_ONBOARD_QUERY_RESPONSE();

        static eActionOp_PLATFORM get_action_op(){
            return (eActionOp_PLATFORM)(ACTION_PLATFORM_ONBOARD_QUERY_RESPONSE);
        }
        sOnboarding& params();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_PLATFORM* m_action_op = nullptr;
        sOnboarding* m_params = nullptr;
};

class cACTION_PLATFORM_ONBOARD_SET_REQUEST : public BaseClass
{
    public:
        cACTION_PLATFORM_ONBOARD_SET_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_PLATFORM_ONBOARD_SET_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_PLATFORM_ONBOARD_SET_REQUEST();

        static eActionOp_PLATFORM get_action_op(){
            return (eActionOp_PLATFORM)(ACTION_PLATFORM_ONBOARD_SET_REQUEST);
        }
        sOnboarding& params();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_PLATFORM* m_action_op = nullptr;
        sOnboarding* m_params = nullptr;
};

class cACTION_PLATFORM_WPS_ONBOARDING_REQUEST : public BaseClass
{
    public:
        cACTION_PLATFORM_WPS_ONBOARDING_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_PLATFORM_WPS_ONBOARDING_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_PLATFORM_WPS_ONBOARDING_REQUEST();

        static eActionOp_PLATFORM get_action_op(){
            return (eActionOp_PLATFORM)(ACTION_PLATFORM_WPS_ONBOARDING_REQUEST);
        }
        char* iface_name(size_t length = 0);
        bool set_iface_name(std::string& str);
        bool set_iface_name(const std::string& str);
        bool set_iface_name(const char buffer[], size_t size);
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_PLATFORM* m_action_op = nullptr;
        char* m_iface_name = nullptr;
        size_t m_iface_name_idx__ = 0;
        int m_lock_order_counter__ = 0;
};

class cACTION_PLATFORM_BEEROCKS_CREDENTIALS_UPDATE_REQUEST : public BaseClass
{
    public:
        cACTION_PLATFORM_BEEROCKS_CREDENTIALS_UPDATE_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_PLATFORM_BEEROCKS_CREDENTIALS_UPDATE_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_PLATFORM_BEEROCKS_CREDENTIALS_UPDATE_REQUEST();

        static eActionOp_PLATFORM get_action_op(){
            return (eActionOp_PLATFORM)(ACTION_PLATFORM_BEEROCKS_CREDENTIALS_UPDATE_REQUEST);
        }
        sWifiCredentials& params();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_PLATFORM* m_action_op = nullptr;
        sWifiCredentials* m_params = nullptr;
};

class cACTION_PLATFORM_BEEROCKS_CREDENTIALS_UPDATE_RESPONSE : public BaseClass
{
    public:
        cACTION_PLATFORM_BEEROCKS_CREDENTIALS_UPDATE_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_PLATFORM_BEEROCKS_CREDENTIALS_UPDATE_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_PLATFORM_BEEROCKS_CREDENTIALS_UPDATE_RESPONSE();

        static eActionOp_PLATFORM get_action_op(){
            return (eActionOp_PLATFORM)(ACTION_PLATFORM_BEEROCKS_CREDENTIALS_UPDATE_RESPONSE);
        }
        uint32_t& result();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_PLATFORM* m_action_op = nullptr;
        uint32_t* m_result = nullptr;
};

class cACTION_PLATFORM_WIFI_CREDENTIALS_SET_REQUEST : public BaseClass
{
    public:
        cACTION_PLATFORM_WIFI_CREDENTIALS_SET_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_PLATFORM_WIFI_CREDENTIALS_SET_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_PLATFORM_WIFI_CREDENTIALS_SET_REQUEST();

        static eActionOp_PLATFORM get_action_op(){
            return (eActionOp_PLATFORM)(ACTION_PLATFORM_WIFI_CREDENTIALS_SET_REQUEST);
        }
        char* iface_name(size_t length = 0);
        bool set_iface_name(std::string& str);
        bool set_iface_name(const std::string& str);
        bool set_iface_name(const char buffer[], size_t size);
        char* ssid(size_t length = 0);
        bool set_ssid(std::string& str);
        bool set_ssid(const std::string& str);
        bool set_ssid(const char buffer[], size_t size);
        char* pass(size_t length = 0);
        bool set_pass(std::string& str);
        bool set_pass(const std::string& str);
        bool set_pass(const char buffer[], size_t size);
        char* security_type(size_t length = 0);
        bool set_security_type(std::string& str);
        bool set_security_type(const std::string& str);
        bool set_security_type(const char buffer[], size_t size);
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_PLATFORM* m_action_op = nullptr;
        char* m_iface_name = nullptr;
        size_t m_iface_name_idx__ = 0;
        int m_lock_order_counter__ = 0;
        char* m_ssid = nullptr;
        size_t m_ssid_idx__ = 0;
        char* m_pass = nullptr;
        size_t m_pass_idx__ = 0;
        char* m_security_type = nullptr;
        size_t m_security_type_idx__ = 0;
};

class cACTION_PLATFORM_WIFI_CREDENTIALS_SET_RESPONSE : public BaseClass
{
    public:
        cACTION_PLATFORM_WIFI_CREDENTIALS_SET_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_PLATFORM_WIFI_CREDENTIALS_SET_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_PLATFORM_WIFI_CREDENTIALS_SET_RESPONSE();

        static eActionOp_PLATFORM get_action_op(){
            return (eActionOp_PLATFORM)(ACTION_PLATFORM_WIFI_CREDENTIALS_SET_RESPONSE);
        }
        char* iface_name(size_t length = 0);
        bool set_iface_name(std::string& str);
        bool set_iface_name(const std::string& str);
        bool set_iface_name(const char buffer[], size_t size);
        uint8_t& success();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_PLATFORM* m_action_op = nullptr;
        char* m_iface_name = nullptr;
        size_t m_iface_name_idx__ = 0;
        int m_lock_order_counter__ = 0;
        uint8_t* m_success = nullptr;
};

class cACTION_PLATFORM_ADVERTISE_SSID_FLAG_UPDATE_REQUEST : public BaseClass
{
    public:
        cACTION_PLATFORM_ADVERTISE_SSID_FLAG_UPDATE_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_PLATFORM_ADVERTISE_SSID_FLAG_UPDATE_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_PLATFORM_ADVERTISE_SSID_FLAG_UPDATE_REQUEST();

        static eActionOp_PLATFORM get_action_op(){
            return (eActionOp_PLATFORM)(ACTION_PLATFORM_ADVERTISE_SSID_FLAG_UPDATE_REQUEST);
        }
        uint32_t& flag();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_PLATFORM* m_action_op = nullptr;
        uint32_t* m_flag = nullptr;
};

class cACTION_PLATFORM_ADVERTISE_SSID_FLAG_UPDATE_RESPONSE : public BaseClass
{
    public:
        cACTION_PLATFORM_ADVERTISE_SSID_FLAG_UPDATE_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_PLATFORM_ADVERTISE_SSID_FLAG_UPDATE_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_PLATFORM_ADVERTISE_SSID_FLAG_UPDATE_RESPONSE();

        static eActionOp_PLATFORM get_action_op(){
            return (eActionOp_PLATFORM)(ACTION_PLATFORM_ADVERTISE_SSID_FLAG_UPDATE_RESPONSE);
        }
        uint32_t& result();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_PLATFORM* m_action_op = nullptr;
        uint32_t* m_result = nullptr;
};

class cACTION_PLATFORM_GET_WLAN_READY_STATUS_REQUEST : public BaseClass
{
    public:
        cACTION_PLATFORM_GET_WLAN_READY_STATUS_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_PLATFORM_GET_WLAN_READY_STATUS_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_PLATFORM_GET_WLAN_READY_STATUS_REQUEST();

        static eActionOp_PLATFORM get_action_op(){
            return (eActionOp_PLATFORM)(ACTION_PLATFORM_GET_WLAN_READY_STATUS_REQUEST);
        }
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_PLATFORM* m_action_op = nullptr;
};

class cACTION_PLATFORM_GET_WLAN_READY_STATUS_RESPONSE : public BaseClass
{
    public:
        cACTION_PLATFORM_GET_WLAN_READY_STATUS_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_PLATFORM_GET_WLAN_READY_STATUS_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_PLATFORM_GET_WLAN_READY_STATUS_RESPONSE();

        static eActionOp_PLATFORM get_action_op(){
            return (eActionOp_PLATFORM)(ACTION_PLATFORM_GET_WLAN_READY_STATUS_RESPONSE);
        }
        uint8_t& result();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_PLATFORM* m_action_op = nullptr;
        uint8_t* m_result = nullptr;
};

class cACTION_PLATFORM_WIFI_CREDENTIALS_GET_REQUEST : public BaseClass
{
    public:
        cACTION_PLATFORM_WIFI_CREDENTIALS_GET_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_PLATFORM_WIFI_CREDENTIALS_GET_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_PLATFORM_WIFI_CREDENTIALS_GET_REQUEST();

        static eActionOp_PLATFORM get_action_op(){
            return (eActionOp_PLATFORM)(ACTION_PLATFORM_WIFI_CREDENTIALS_GET_REQUEST);
        }
        uint8_t& vap_id();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_PLATFORM* m_action_op = nullptr;
        uint8_t* m_vap_id = nullptr;
};

class cACTION_PLATFORM_WIFI_CREDENTIALS_GET_RESPONSE : public BaseClass
{
    public:
        cACTION_PLATFORM_WIFI_CREDENTIALS_GET_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_PLATFORM_WIFI_CREDENTIALS_GET_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_PLATFORM_WIFI_CREDENTIALS_GET_RESPONSE();

        static eActionOp_PLATFORM get_action_op(){
            return (eActionOp_PLATFORM)(ACTION_PLATFORM_WIFI_CREDENTIALS_GET_RESPONSE);
        }
        sWifiCredentials& front_params();
        sWifiCredentials& back_params();
        uint32_t& result();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_PLATFORM* m_action_op = nullptr;
        sWifiCredentials* m_front_params = nullptr;
        sWifiCredentials* m_back_params = nullptr;
        uint32_t* m_result = nullptr;
};

class cACTION_PLATFORM_ADMIN_CREDENTIALS_GET_REQUEST : public BaseClass
{
    public:
        cACTION_PLATFORM_ADMIN_CREDENTIALS_GET_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_PLATFORM_ADMIN_CREDENTIALS_GET_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_PLATFORM_ADMIN_CREDENTIALS_GET_REQUEST();

        static eActionOp_PLATFORM get_action_op(){
            return (eActionOp_PLATFORM)(ACTION_PLATFORM_ADMIN_CREDENTIALS_GET_REQUEST);
        }
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_PLATFORM* m_action_op = nullptr;
};

class cACTION_PLATFORM_ADMIN_CREDENTIALS_GET_RESPONSE : public BaseClass
{
    public:
        cACTION_PLATFORM_ADMIN_CREDENTIALS_GET_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_PLATFORM_ADMIN_CREDENTIALS_GET_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_PLATFORM_ADMIN_CREDENTIALS_GET_RESPONSE();

        static eActionOp_PLATFORM get_action_op(){
            return (eActionOp_PLATFORM)(ACTION_PLATFORM_ADMIN_CREDENTIALS_GET_RESPONSE);
        }
        sAdminCredentials& params();
        uint32_t& result();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_PLATFORM* m_action_op = nullptr;
        sAdminCredentials* m_params = nullptr;
        uint32_t* m_result = nullptr;
};

class cACTION_PLATFORM_DEVICE_INFO_GET_REQUEST : public BaseClass
{
    public:
        cACTION_PLATFORM_DEVICE_INFO_GET_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_PLATFORM_DEVICE_INFO_GET_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_PLATFORM_DEVICE_INFO_GET_REQUEST();

        static eActionOp_PLATFORM get_action_op(){
            return (eActionOp_PLATFORM)(ACTION_PLATFORM_DEVICE_INFO_GET_REQUEST);
        }
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_PLATFORM* m_action_op = nullptr;
};

class cACTION_PLATFORM_DEVICE_INFO_GET_RESPONSE : public BaseClass
{
    public:
        cACTION_PLATFORM_DEVICE_INFO_GET_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_PLATFORM_DEVICE_INFO_GET_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_PLATFORM_DEVICE_INFO_GET_RESPONSE();

        static eActionOp_PLATFORM get_action_op(){
            return (eActionOp_PLATFORM)(ACTION_PLATFORM_DEVICE_INFO_GET_RESPONSE);
        }
        sDeviceInfo& params();
        uint32_t& result();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_PLATFORM* m_action_op = nullptr;
        sDeviceInfo* m_params = nullptr;
        uint32_t* m_result = nullptr;
};

class cACTION_PLATFORM_LOCAL_MASTER_GET_REQUEST : public BaseClass
{
    public:
        cACTION_PLATFORM_LOCAL_MASTER_GET_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_PLATFORM_LOCAL_MASTER_GET_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_PLATFORM_LOCAL_MASTER_GET_REQUEST();

        static eActionOp_PLATFORM get_action_op(){
            return (eActionOp_PLATFORM)(ACTION_PLATFORM_LOCAL_MASTER_GET_REQUEST);
        }
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_PLATFORM* m_action_op = nullptr;
};

class cACTION_PLATFORM_LOCAL_MASTER_GET_RESPONSE : public BaseClass
{
    public:
        cACTION_PLATFORM_LOCAL_MASTER_GET_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_PLATFORM_LOCAL_MASTER_GET_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_PLATFORM_LOCAL_MASTER_GET_RESPONSE();

        static eActionOp_PLATFORM get_action_op(){
            return (eActionOp_PLATFORM)(ACTION_PLATFORM_LOCAL_MASTER_GET_RESPONSE);
        }
        uint8_t& local_master();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_PLATFORM* m_action_op = nullptr;
        uint8_t* m_local_master = nullptr;
};

class cACTION_PLATFORM_WIFI_SET_IFACE_STATE_REQUEST : public BaseClass
{
    public:
        cACTION_PLATFORM_WIFI_SET_IFACE_STATE_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_PLATFORM_WIFI_SET_IFACE_STATE_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_PLATFORM_WIFI_SET_IFACE_STATE_REQUEST();

        static eActionOp_PLATFORM get_action_op(){
            return (eActionOp_PLATFORM)(ACTION_PLATFORM_WIFI_SET_IFACE_STATE_REQUEST);
        }
        char* iface_name(size_t length = 0);
        bool set_iface_name(std::string& str);
        bool set_iface_name(const std::string& str);
        bool set_iface_name(const char buffer[], size_t size);
        int8_t& iface_operation();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_PLATFORM* m_action_op = nullptr;
        char* m_iface_name = nullptr;
        size_t m_iface_name_idx__ = 0;
        int m_lock_order_counter__ = 0;
        int8_t* m_iface_operation = nullptr;
};

class cACTION_PLATFORM_WIFI_SET_IFACE_STATE_RESPONSE : public BaseClass
{
    public:
        cACTION_PLATFORM_WIFI_SET_IFACE_STATE_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_PLATFORM_WIFI_SET_IFACE_STATE_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_PLATFORM_WIFI_SET_IFACE_STATE_RESPONSE();

        static eActionOp_PLATFORM get_action_op(){
            return (eActionOp_PLATFORM)(ACTION_PLATFORM_WIFI_SET_IFACE_STATE_RESPONSE);
        }
        char* iface_name(size_t length = 0);
        bool set_iface_name(std::string& str);
        bool set_iface_name(const std::string& str);
        bool set_iface_name(const char buffer[], size_t size);
        int8_t& iface_operation();
        uint8_t& success();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_PLATFORM* m_action_op = nullptr;
        char* m_iface_name = nullptr;
        size_t m_iface_name_idx__ = 0;
        int m_lock_order_counter__ = 0;
        int8_t* m_iface_operation = nullptr;
        uint8_t* m_success = nullptr;
};

class cACTION_PLATFORM_WIFI_SET_RADIO_TX_STATE_REQUEST : public BaseClass
{
    public:
        cACTION_PLATFORM_WIFI_SET_RADIO_TX_STATE_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_PLATFORM_WIFI_SET_RADIO_TX_STATE_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_PLATFORM_WIFI_SET_RADIO_TX_STATE_REQUEST();

        static eActionOp_PLATFORM get_action_op(){
            return (eActionOp_PLATFORM)(ACTION_PLATFORM_WIFI_SET_RADIO_TX_STATE_REQUEST);
        }
        char* iface_name(size_t length = 0);
        bool set_iface_name(std::string& str);
        bool set_iface_name(const std::string& str);
        bool set_iface_name(const char buffer[], size_t size);
        uint8_t& enable();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_PLATFORM* m_action_op = nullptr;
        char* m_iface_name = nullptr;
        size_t m_iface_name_idx__ = 0;
        int m_lock_order_counter__ = 0;
        uint8_t* m_enable = nullptr;
};

class cACTION_PLATFORM_WIFI_SET_RADIO_TX_STATE_RESPONSE : public BaseClass
{
    public:
        cACTION_PLATFORM_WIFI_SET_RADIO_TX_STATE_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_PLATFORM_WIFI_SET_RADIO_TX_STATE_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_PLATFORM_WIFI_SET_RADIO_TX_STATE_RESPONSE();

        static eActionOp_PLATFORM get_action_op(){
            return (eActionOp_PLATFORM)(ACTION_PLATFORM_WIFI_SET_RADIO_TX_STATE_RESPONSE);
        }
        char* iface_name(size_t length = 0);
        bool set_iface_name(std::string& str);
        bool set_iface_name(const std::string& str);
        bool set_iface_name(const char buffer[], size_t size);
        uint8_t& enable();
        uint8_t& success();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_PLATFORM* m_action_op = nullptr;
        char* m_iface_name = nullptr;
        size_t m_iface_name_idx__ = 0;
        int m_lock_order_counter__ = 0;
        uint8_t* m_enable = nullptr;
        uint8_t* m_success = nullptr;
};

class cACTION_PLATFORM_VERSION_MISMATCH_NOTIFICATION : public BaseClass
{
    public:
        cACTION_PLATFORM_VERSION_MISMATCH_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_PLATFORM_VERSION_MISMATCH_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_PLATFORM_VERSION_MISMATCH_NOTIFICATION();

        static eActionOp_PLATFORM get_action_op(){
            return (eActionOp_PLATFORM)(ACTION_PLATFORM_VERSION_MISMATCH_NOTIFICATION);
        }
        sVersions& versions();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_PLATFORM* m_action_op = nullptr;
        sVersions* m_versions = nullptr;
};

class cACTION_PLATFORM_MASTER_SLAVE_VERSIONS_NOTIFICATION : public BaseClass
{
    public:
        cACTION_PLATFORM_MASTER_SLAVE_VERSIONS_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_PLATFORM_MASTER_SLAVE_VERSIONS_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_PLATFORM_MASTER_SLAVE_VERSIONS_NOTIFICATION();

        static eActionOp_PLATFORM get_action_op(){
            return (eActionOp_PLATFORM)(ACTION_PLATFORM_MASTER_SLAVE_VERSIONS_NOTIFICATION);
        }
        sVersions& versions();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_PLATFORM* m_action_op = nullptr;
        sVersions* m_versions = nullptr;
};

class cACTION_PLATFORM_GET_MASTER_SLAVE_VERSIONS_REQUEST : public BaseClass
{
    public:
        cACTION_PLATFORM_GET_MASTER_SLAVE_VERSIONS_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_PLATFORM_GET_MASTER_SLAVE_VERSIONS_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_PLATFORM_GET_MASTER_SLAVE_VERSIONS_REQUEST();

        static eActionOp_PLATFORM get_action_op(){
            return (eActionOp_PLATFORM)(ACTION_PLATFORM_GET_MASTER_SLAVE_VERSIONS_REQUEST);
        }
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_PLATFORM* m_action_op = nullptr;
};

class cACTION_PLATFORM_GET_MASTER_SLAVE_VERSIONS_RESPONSE : public BaseClass
{
    public:
        cACTION_PLATFORM_GET_MASTER_SLAVE_VERSIONS_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_PLATFORM_GET_MASTER_SLAVE_VERSIONS_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_PLATFORM_GET_MASTER_SLAVE_VERSIONS_RESPONSE();

        static eActionOp_PLATFORM get_action_op(){
            return (eActionOp_PLATFORM)(ACTION_PLATFORM_GET_MASTER_SLAVE_VERSIONS_RESPONSE);
        }
        sVersions& versions();
        uint32_t& result();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_PLATFORM* m_action_op = nullptr;
        sVersions* m_versions = nullptr;
        uint32_t* m_result = nullptr;
};

class cACTION_PLATFORM_ERROR_NOTIFICATION : public BaseClass
{
    public:
        cACTION_PLATFORM_ERROR_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_PLATFORM_ERROR_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_PLATFORM_ERROR_NOTIFICATION();

        static eActionOp_PLATFORM get_action_op(){
            return (eActionOp_PLATFORM)(ACTION_PLATFORM_ERROR_NOTIFICATION);
        }
        uint32_t& code();
        char* data(size_t length = 0);
        bool set_data(std::string& str);
        bool set_data(const std::string& str);
        bool set_data(const char buffer[], size_t size);
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_PLATFORM* m_action_op = nullptr;
        uint32_t* m_code = nullptr;
        char* m_data = nullptr;
        size_t m_data_idx__ = 0;
        int m_lock_order_counter__ = 0;
};

class cACTION_PLATFORM_WIFI_INTERFACE_STATUS_NOTIFICATION : public BaseClass
{
    public:
        cACTION_PLATFORM_WIFI_INTERFACE_STATUS_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_PLATFORM_WIFI_INTERFACE_STATUS_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_PLATFORM_WIFI_INTERFACE_STATUS_NOTIFICATION();

        static eActionOp_PLATFORM get_action_op(){
            return (eActionOp_PLATFORM)(ACTION_PLATFORM_WIFI_INTERFACE_STATUS_NOTIFICATION);
        }
        char* iface_name_ap(size_t length = 0);
        bool set_iface_name_ap(std::string& str);
        bool set_iface_name_ap(const std::string& str);
        bool set_iface_name_ap(const char buffer[], size_t size);
        char* iface_name_bh(size_t length = 0);
        bool set_iface_name_bh(std::string& str);
        bool set_iface_name_bh(const std::string& str);
        bool set_iface_name_bh(const char buffer[], size_t size);
        uint8_t& status_ap();
        uint8_t& status_bh();
        uint8_t& status_bh_wired();
        uint8_t& is_bh_manager();
        uint8_t& status_operational();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_PLATFORM* m_action_op = nullptr;
        char* m_iface_name_ap = nullptr;
        size_t m_iface_name_ap_idx__ = 0;
        int m_lock_order_counter__ = 0;
        char* m_iface_name_bh = nullptr;
        size_t m_iface_name_bh_idx__ = 0;
        uint8_t* m_status_ap = nullptr;
        uint8_t* m_status_bh = nullptr;
        uint8_t* m_status_bh_wired = nullptr;
        uint8_t* m_is_bh_manager = nullptr;
        uint8_t* m_status_operational = nullptr;
};

class cACTION_PLATFORM_WIFI_CONFIGURATION_UPDATE_REQUEST : public BaseClass
{
    public:
        cACTION_PLATFORM_WIFI_CONFIGURATION_UPDATE_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_PLATFORM_WIFI_CONFIGURATION_UPDATE_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_PLATFORM_WIFI_CONFIGURATION_UPDATE_REQUEST();

        static eActionOp_PLATFORM get_action_op(){
            return (eActionOp_PLATFORM)(ACTION_PLATFORM_WIFI_CONFIGURATION_UPDATE_REQUEST);
        }
        uint8_t& config_start();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_PLATFORM* m_action_op = nullptr;
        uint8_t* m_config_start = nullptr;
};

class cACTION_PLATFORM_WIFI_CONFIGURATION_UPDATE_RESPONSE : public BaseClass
{
    public:
        cACTION_PLATFORM_WIFI_CONFIGURATION_UPDATE_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_PLATFORM_WIFI_CONFIGURATION_UPDATE_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_PLATFORM_WIFI_CONFIGURATION_UPDATE_RESPONSE();

        static eActionOp_PLATFORM get_action_op(){
            return (eActionOp_PLATFORM)(ACTION_PLATFORM_WIFI_CONFIGURATION_UPDATE_RESPONSE);
        }
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_PLATFORM* m_action_op = nullptr;
};

class cACTION_PLATFORM_OPERATIONAL_NOTIFICATION : public BaseClass
{
    public:
        cACTION_PLATFORM_OPERATIONAL_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_PLATFORM_OPERATIONAL_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_PLATFORM_OPERATIONAL_NOTIFICATION();

        static eActionOp_PLATFORM get_action_op(){
            return (eActionOp_PLATFORM)(ACTION_PLATFORM_OPERATIONAL_NOTIFICATION);
        }
        uint8_t& operational();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_PLATFORM* m_action_op = nullptr;
        uint8_t* m_operational = nullptr;
};

}; // close namespace: beerocks_message

#endif //_BEEROCKS/TLVF_BEEROCKS_MESSAGE_PLATFORM_H_
