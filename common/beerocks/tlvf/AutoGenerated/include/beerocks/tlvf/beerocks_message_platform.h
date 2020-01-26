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
#include <tlvf/ClassList.h>
#include <tuple>
#include <tlvf/tlvfutils.h>
#include "beerocks/tlvf/beerocks_message_common.h"

namespace beerocks_message {


class cACTION_PLATFORM_SON_SLAVE_BACKHAUL_CONNECTION_COMPLETE_NOTIFICATION : public BaseClass
{
    public:
        cACTION_PLATFORM_SON_SLAVE_BACKHAUL_CONNECTION_COMPLETE_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_PLATFORM_SON_SLAVE_BACKHAUL_CONNECTION_COMPLETE_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_PLATFORM_SON_SLAVE_BACKHAUL_CONNECTION_COMPLETE_NOTIFICATION();

        static eActionOp_PLATFORM get_action_op(){
            return (eActionOp_PLATFORM)(ACTION_PLATFORM_SON_SLAVE_BACKHAUL_CONNECTION_COMPLETE_NOTIFICATION);
        }
        uint8_t& is_backhaul_manager();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_PLATFORM* m_action_op = nullptr;
        uint8_t* m_is_backhaul_manager = nullptr;
};

class cACTION_PLATFORM_SON_SLAVE_REGISTER_REQUEST : public BaseClass
{
    public:
        cACTION_PLATFORM_SON_SLAVE_REGISTER_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_PLATFORM_SON_SLAVE_REGISTER_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_PLATFORM_SON_SLAVE_REGISTER_REQUEST();

        static eActionOp_PLATFORM get_action_op(){
            return (eActionOp_PLATFORM)(ACTION_PLATFORM_SON_SLAVE_REGISTER_REQUEST);
        }
        std::string iface_name_str();
        char* iface_name(size_t length = 0);
        bool set_iface_name(const std::string& str);
        bool set_iface_name(const char buffer[], size_t size);
        void class_swap() override;
        bool finalize() override;
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
        cACTION_PLATFORM_SON_SLAVE_REGISTER_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_PLATFORM_SON_SLAVE_REGISTER_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_PLATFORM_SON_SLAVE_REGISTER_RESPONSE();

        static eActionOp_PLATFORM get_action_op(){
            return (eActionOp_PLATFORM)(ACTION_PLATFORM_SON_SLAVE_REGISTER_RESPONSE);
        }
        sPlatformSettings& platform_settings();
        sWlanSettings& wlan_settings();
        uint32_t& valid();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_PLATFORM* m_action_op = nullptr;
        sPlatformSettings* m_platform_settings = nullptr;
        sWlanSettings* m_wlan_settings = nullptr;
        uint32_t* m_valid = nullptr;
};

class cACTION_PLATFORM_ARP_MONITOR_NOTIFICATION : public BaseClass
{
    public:
        cACTION_PLATFORM_ARP_MONITOR_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_PLATFORM_ARP_MONITOR_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_PLATFORM_ARP_MONITOR_NOTIFICATION();

        static eActionOp_PLATFORM get_action_op(){
            return (eActionOp_PLATFORM)(ACTION_PLATFORM_ARP_MONITOR_NOTIFICATION);
        }
        sArpMonitorData& params();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_PLATFORM* m_action_op = nullptr;
        sArpMonitorData* m_params = nullptr;
};

class cACTION_PLATFORM_WLAN_PARAMS_CHANGED_NOTIFICATION : public BaseClass
{
    public:
        cACTION_PLATFORM_WLAN_PARAMS_CHANGED_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_PLATFORM_WLAN_PARAMS_CHANGED_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_PLATFORM_WLAN_PARAMS_CHANGED_NOTIFICATION();

        static eActionOp_PLATFORM get_action_op(){
            return (eActionOp_PLATFORM)(ACTION_PLATFORM_WLAN_PARAMS_CHANGED_NOTIFICATION);
        }
        sWlanSettings& wlan_settings();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_PLATFORM* m_action_op = nullptr;
        sWlanSettings* m_wlan_settings = nullptr;
};

class cACTION_PLATFORM_DHCP_MONITOR_NOTIFICATION : public BaseClass
{
    public:
        cACTION_PLATFORM_DHCP_MONITOR_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_PLATFORM_DHCP_MONITOR_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_PLATFORM_DHCP_MONITOR_NOTIFICATION();

        static eActionOp_PLATFORM get_action_op(){
            return (eActionOp_PLATFORM)(ACTION_PLATFORM_DHCP_MONITOR_NOTIFICATION);
        }
        eDHCPOp& dhcp_op();
        uint32_t& op();
        sMacAddr& mac();
        beerocks::net::sIpv4Addr& ipv4();
        std::string hostname_str();
        char* hostname(size_t length = 0);
        bool set_hostname(const std::string& str);
        bool set_hostname(const char buffer[], size_t size);
        void class_swap() override;
        bool finalize() override;
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
        cACTION_PLATFORM_CHANGE_MODULE_LOGGING_LEVEL(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_PLATFORM_CHANGE_MODULE_LOGGING_LEVEL(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_PLATFORM_CHANGE_MODULE_LOGGING_LEVEL();

        static eActionOp_PLATFORM get_action_op(){
            return (eActionOp_PLATFORM)(ACTION_PLATFORM_CHANGE_MODULE_LOGGING_LEVEL);
        }
        sLoggingLevelChange& params();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_PLATFORM* m_action_op = nullptr;
        sLoggingLevelChange* m_params = nullptr;
};

class cACTION_PLATFORM_ARP_QUERY_REQUEST : public BaseClass
{
    public:
        cACTION_PLATFORM_ARP_QUERY_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_PLATFORM_ARP_QUERY_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_PLATFORM_ARP_QUERY_REQUEST();

        static eActionOp_PLATFORM get_action_op(){
            return (eActionOp_PLATFORM)(ACTION_PLATFORM_ARP_QUERY_REQUEST);
        }
        sArpQuery& params();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_PLATFORM* m_action_op = nullptr;
        sArpQuery* m_params = nullptr;
};

class cACTION_PLATFORM_ARP_QUERY_RESPONSE : public BaseClass
{
    public:
        cACTION_PLATFORM_ARP_QUERY_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_PLATFORM_ARP_QUERY_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_PLATFORM_ARP_QUERY_RESPONSE();

        static eActionOp_PLATFORM get_action_op(){
            return (eActionOp_PLATFORM)(ACTION_PLATFORM_ARP_QUERY_RESPONSE);
        }
        sArpMonitorData& params();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_PLATFORM* m_action_op = nullptr;
        sArpMonitorData* m_params = nullptr;
};

class cACTION_PLATFORM_ONBOARD_QUERY_REQUEST : public BaseClass
{
    public:
        cACTION_PLATFORM_ONBOARD_QUERY_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_PLATFORM_ONBOARD_QUERY_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_PLATFORM_ONBOARD_QUERY_REQUEST();

        static eActionOp_PLATFORM get_action_op(){
            return (eActionOp_PLATFORM)(ACTION_PLATFORM_ONBOARD_QUERY_REQUEST);
        }
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_PLATFORM* m_action_op = nullptr;
};

class cACTION_PLATFORM_ONBOARD_QUERY_RESPONSE : public BaseClass
{
    public:
        cACTION_PLATFORM_ONBOARD_QUERY_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_PLATFORM_ONBOARD_QUERY_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_PLATFORM_ONBOARD_QUERY_RESPONSE();

        static eActionOp_PLATFORM get_action_op(){
            return (eActionOp_PLATFORM)(ACTION_PLATFORM_ONBOARD_QUERY_RESPONSE);
        }
        sOnboarding& params();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_PLATFORM* m_action_op = nullptr;
        sOnboarding* m_params = nullptr;
};

class cACTION_PLATFORM_ONBOARD_SET_REQUEST : public BaseClass
{
    public:
        cACTION_PLATFORM_ONBOARD_SET_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_PLATFORM_ONBOARD_SET_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_PLATFORM_ONBOARD_SET_REQUEST();

        static eActionOp_PLATFORM get_action_op(){
            return (eActionOp_PLATFORM)(ACTION_PLATFORM_ONBOARD_SET_REQUEST);
        }
        sOnboarding& params();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_PLATFORM* m_action_op = nullptr;
        sOnboarding* m_params = nullptr;
};

class cACTION_PLATFORM_WPS_ONBOARDING_REQUEST : public BaseClass
{
    public:
        cACTION_PLATFORM_WPS_ONBOARDING_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_PLATFORM_WPS_ONBOARDING_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_PLATFORM_WPS_ONBOARDING_REQUEST();

        static eActionOp_PLATFORM get_action_op(){
            return (eActionOp_PLATFORM)(ACTION_PLATFORM_WPS_ONBOARDING_REQUEST);
        }
        std::string iface_name_str();
        char* iface_name(size_t length = 0);
        bool set_iface_name(const std::string& str);
        bool set_iface_name(const char buffer[], size_t size);
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_PLATFORM* m_action_op = nullptr;
        char* m_iface_name = nullptr;
        size_t m_iface_name_idx__ = 0;
        int m_lock_order_counter__ = 0;
};

class cACTION_PLATFORM_WIFI_CREDENTIALS_GET_REQUEST : public BaseClass
{
    public:
        cACTION_PLATFORM_WIFI_CREDENTIALS_GET_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_PLATFORM_WIFI_CREDENTIALS_GET_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_PLATFORM_WIFI_CREDENTIALS_GET_REQUEST();

        static eActionOp_PLATFORM get_action_op(){
            return (eActionOp_PLATFORM)(ACTION_PLATFORM_WIFI_CREDENTIALS_GET_REQUEST);
        }
        uint8_t& vap_id();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_PLATFORM* m_action_op = nullptr;
        uint8_t* m_vap_id = nullptr;
};

class cACTION_PLATFORM_WIFI_CREDENTIALS_GET_RESPONSE : public BaseClass
{
    public:
        cACTION_PLATFORM_WIFI_CREDENTIALS_GET_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_PLATFORM_WIFI_CREDENTIALS_GET_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_PLATFORM_WIFI_CREDENTIALS_GET_RESPONSE();

        static eActionOp_PLATFORM get_action_op(){
            return (eActionOp_PLATFORM)(ACTION_PLATFORM_WIFI_CREDENTIALS_GET_RESPONSE);
        }
        sWifiCredentials& front_params();
        sWifiCredentials& back_params();
        uint32_t& result();
        void class_swap() override;
        bool finalize() override;
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
        cACTION_PLATFORM_ADMIN_CREDENTIALS_GET_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_PLATFORM_ADMIN_CREDENTIALS_GET_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_PLATFORM_ADMIN_CREDENTIALS_GET_REQUEST();

        static eActionOp_PLATFORM get_action_op(){
            return (eActionOp_PLATFORM)(ACTION_PLATFORM_ADMIN_CREDENTIALS_GET_REQUEST);
        }
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_PLATFORM* m_action_op = nullptr;
};

class cACTION_PLATFORM_ADMIN_CREDENTIALS_GET_RESPONSE : public BaseClass
{
    public:
        cACTION_PLATFORM_ADMIN_CREDENTIALS_GET_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_PLATFORM_ADMIN_CREDENTIALS_GET_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_PLATFORM_ADMIN_CREDENTIALS_GET_RESPONSE();

        static eActionOp_PLATFORM get_action_op(){
            return (eActionOp_PLATFORM)(ACTION_PLATFORM_ADMIN_CREDENTIALS_GET_RESPONSE);
        }
        sAdminCredentials& params();
        uint32_t& result();
        void class_swap() override;
        bool finalize() override;
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
        cACTION_PLATFORM_DEVICE_INFO_GET_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_PLATFORM_DEVICE_INFO_GET_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_PLATFORM_DEVICE_INFO_GET_REQUEST();

        static eActionOp_PLATFORM get_action_op(){
            return (eActionOp_PLATFORM)(ACTION_PLATFORM_DEVICE_INFO_GET_REQUEST);
        }
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_PLATFORM* m_action_op = nullptr;
};

class cACTION_PLATFORM_DEVICE_INFO_GET_RESPONSE : public BaseClass
{
    public:
        cACTION_PLATFORM_DEVICE_INFO_GET_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_PLATFORM_DEVICE_INFO_GET_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_PLATFORM_DEVICE_INFO_GET_RESPONSE();

        static eActionOp_PLATFORM get_action_op(){
            return (eActionOp_PLATFORM)(ACTION_PLATFORM_DEVICE_INFO_GET_RESPONSE);
        }
        sDeviceInfo& params();
        uint32_t& result();
        void class_swap() override;
        bool finalize() override;
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
        cACTION_PLATFORM_LOCAL_MASTER_GET_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_PLATFORM_LOCAL_MASTER_GET_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_PLATFORM_LOCAL_MASTER_GET_REQUEST();

        static eActionOp_PLATFORM get_action_op(){
            return (eActionOp_PLATFORM)(ACTION_PLATFORM_LOCAL_MASTER_GET_REQUEST);
        }
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_PLATFORM* m_action_op = nullptr;
};

class cACTION_PLATFORM_LOCAL_MASTER_GET_RESPONSE : public BaseClass
{
    public:
        cACTION_PLATFORM_LOCAL_MASTER_GET_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_PLATFORM_LOCAL_MASTER_GET_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_PLATFORM_LOCAL_MASTER_GET_RESPONSE();

        static eActionOp_PLATFORM get_action_op(){
            return (eActionOp_PLATFORM)(ACTION_PLATFORM_LOCAL_MASTER_GET_RESPONSE);
        }
        uint8_t& local_master();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_PLATFORM* m_action_op = nullptr;
        uint8_t* m_local_master = nullptr;
};

class cACTION_PLATFORM_VERSION_MISMATCH_NOTIFICATION : public BaseClass
{
    public:
        cACTION_PLATFORM_VERSION_MISMATCH_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_PLATFORM_VERSION_MISMATCH_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_PLATFORM_VERSION_MISMATCH_NOTIFICATION();

        static eActionOp_PLATFORM get_action_op(){
            return (eActionOp_PLATFORM)(ACTION_PLATFORM_VERSION_MISMATCH_NOTIFICATION);
        }
        sVersions& versions();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_PLATFORM* m_action_op = nullptr;
        sVersions* m_versions = nullptr;
};

class cACTION_PLATFORM_MASTER_SLAVE_VERSIONS_NOTIFICATION : public BaseClass
{
    public:
        cACTION_PLATFORM_MASTER_SLAVE_VERSIONS_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_PLATFORM_MASTER_SLAVE_VERSIONS_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_PLATFORM_MASTER_SLAVE_VERSIONS_NOTIFICATION();

        static eActionOp_PLATFORM get_action_op(){
            return (eActionOp_PLATFORM)(ACTION_PLATFORM_MASTER_SLAVE_VERSIONS_NOTIFICATION);
        }
        sVersions& versions();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_PLATFORM* m_action_op = nullptr;
        sVersions* m_versions = nullptr;
};

class cACTION_PLATFORM_GET_MASTER_SLAVE_VERSIONS_REQUEST : public BaseClass
{
    public:
        cACTION_PLATFORM_GET_MASTER_SLAVE_VERSIONS_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_PLATFORM_GET_MASTER_SLAVE_VERSIONS_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_PLATFORM_GET_MASTER_SLAVE_VERSIONS_REQUEST();

        static eActionOp_PLATFORM get_action_op(){
            return (eActionOp_PLATFORM)(ACTION_PLATFORM_GET_MASTER_SLAVE_VERSIONS_REQUEST);
        }
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_PLATFORM* m_action_op = nullptr;
};

class cACTION_PLATFORM_GET_MASTER_SLAVE_VERSIONS_RESPONSE : public BaseClass
{
    public:
        cACTION_PLATFORM_GET_MASTER_SLAVE_VERSIONS_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_PLATFORM_GET_MASTER_SLAVE_VERSIONS_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_PLATFORM_GET_MASTER_SLAVE_VERSIONS_RESPONSE();

        static eActionOp_PLATFORM get_action_op(){
            return (eActionOp_PLATFORM)(ACTION_PLATFORM_GET_MASTER_SLAVE_VERSIONS_RESPONSE);
        }
        sVersions& versions();
        uint32_t& result();
        void class_swap() override;
        bool finalize() override;
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
        cACTION_PLATFORM_ERROR_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_PLATFORM_ERROR_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_PLATFORM_ERROR_NOTIFICATION();

        static eActionOp_PLATFORM get_action_op(){
            return (eActionOp_PLATFORM)(ACTION_PLATFORM_ERROR_NOTIFICATION);
        }
        uint32_t& code();
        std::string data_str();
        char* data(size_t length = 0);
        bool set_data(const std::string& str);
        bool set_data(const char buffer[], size_t size);
        void class_swap() override;
        bool finalize() override;
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
        cACTION_PLATFORM_WIFI_INTERFACE_STATUS_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_PLATFORM_WIFI_INTERFACE_STATUS_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_PLATFORM_WIFI_INTERFACE_STATUS_NOTIFICATION();

        static eActionOp_PLATFORM get_action_op(){
            return (eActionOp_PLATFORM)(ACTION_PLATFORM_WIFI_INTERFACE_STATUS_NOTIFICATION);
        }
        std::string iface_name_ap_str();
        char* iface_name_ap(size_t length = 0);
        bool set_iface_name_ap(const std::string& str);
        bool set_iface_name_ap(const char buffer[], size_t size);
        std::string iface_name_bh_str();
        char* iface_name_bh(size_t length = 0);
        bool set_iface_name_bh(const std::string& str);
        bool set_iface_name_bh(const char buffer[], size_t size);
        uint8_t& status_ap();
        uint8_t& status_bh();
        uint8_t& status_bh_wired();
        uint8_t& is_bh_manager();
        uint8_t& status_operational();
        void class_swap() override;
        bool finalize() override;
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

class cACTION_PLATFORM_OPERATIONAL_NOTIFICATION : public BaseClass
{
    public:
        cACTION_PLATFORM_OPERATIONAL_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_PLATFORM_OPERATIONAL_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_PLATFORM_OPERATIONAL_NOTIFICATION();

        static eActionOp_PLATFORM get_action_op(){
            return (eActionOp_PLATFORM)(ACTION_PLATFORM_OPERATIONAL_NOTIFICATION);
        }
        uint8_t& operational();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_PLATFORM* m_action_op = nullptr;
        uint8_t* m_operational = nullptr;
};

}; // close namespace: beerocks_message

#endif //_BEEROCKS/TLVF_BEEROCKS_MESSAGE_PLATFORM_H_
