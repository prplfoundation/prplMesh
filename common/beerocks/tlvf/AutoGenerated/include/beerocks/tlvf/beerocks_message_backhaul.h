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

#ifndef _BEEROCKS_TLVF_BEEROCKS_MESSAGE_BACKHAUL_H_
#define _BEEROCKS_TLVF_BEEROCKS_MESSAGE_BACKHAUL_H_

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


class cACTION_BACKHAUL_REGISTER_REQUEST : public BaseClass
{
    public:
        cACTION_BACKHAUL_REGISTER_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_BACKHAUL_REGISTER_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_BACKHAUL_REGISTER_REQUEST();

        static eActionOp_BACKHAUL get_action_op(){
            return (eActionOp_BACKHAUL)(ACTION_BACKHAUL_REGISTER_REQUEST);
        }
        std::string sta_iface_str();
        char* sta_iface(size_t length = 0);
        bool set_sta_iface(const std::string& str);
        bool set_sta_iface(const char buffer[], size_t size);
        std::string hostap_iface_str();
        char* hostap_iface(size_t length = 0);
        bool set_hostap_iface(const std::string& str);
        bool set_hostap_iface(const char buffer[], size_t size);
        uint8_t& local_master();
        uint8_t& local_gw();
        uint8_t& sta_iface_filter_low();
        uint8_t& onboarding();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BACKHAUL* m_action_op = nullptr;
        char* m_sta_iface = nullptr;
        size_t m_sta_iface_idx__ = 0;
        int m_lock_order_counter__ = 0;
        char* m_hostap_iface = nullptr;
        size_t m_hostap_iface_idx__ = 0;
        uint8_t* m_local_master = nullptr;
        uint8_t* m_local_gw = nullptr;
        uint8_t* m_sta_iface_filter_low = nullptr;
        uint8_t* m_onboarding = nullptr;
};

class cACTION_BACKHAUL_REGISTER_RESPONSE : public BaseClass
{
    public:
        cACTION_BACKHAUL_REGISTER_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_BACKHAUL_REGISTER_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_BACKHAUL_REGISTER_RESPONSE();

        static eActionOp_BACKHAUL get_action_op(){
            return (eActionOp_BACKHAUL)(ACTION_BACKHAUL_REGISTER_RESPONSE);
        }
        uint8_t& is_backhaul_manager();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BACKHAUL* m_action_op = nullptr;
        uint8_t* m_is_backhaul_manager = nullptr;
};

class cACTION_BACKHAUL_BUSY_NOTIFICATION : public BaseClass
{
    public:
        cACTION_BACKHAUL_BUSY_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_BACKHAUL_BUSY_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_BACKHAUL_BUSY_NOTIFICATION();

        static eActionOp_BACKHAUL get_action_op(){
            return (eActionOp_BACKHAUL)(ACTION_BACKHAUL_BUSY_NOTIFICATION);
        }
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BACKHAUL* m_action_op = nullptr;
};

class cACTION_BACKHAUL_ENABLE : public BaseClass
{
    public:
        cACTION_BACKHAUL_ENABLE(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_BACKHAUL_ENABLE(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_BACKHAUL_ENABLE();

        static eActionOp_BACKHAUL get_action_op(){
            return (eActionOp_BACKHAUL)(ACTION_BACKHAUL_ENABLE);
        }
        std::string bridge_iface_str();
        char* bridge_iface(size_t length = 0);
        bool set_bridge_iface(const std::string& str);
        bool set_bridge_iface(const char buffer[], size_t size);
        sMacAddr& iface_mac();
        uint8_t& iface_is_5ghz();
        std::string wire_iface_str();
        char* wire_iface(size_t length = 0);
        bool set_wire_iface(const std::string& str);
        bool set_wire_iface(const char buffer[], size_t size);
        std::string sta_iface_str();
        char* sta_iface(size_t length = 0);
        bool set_sta_iface(const std::string& str);
        bool set_sta_iface(const char buffer[], size_t size);
        std::string ap_iface_str();
        char* ap_iface(size_t length = 0);
        bool set_ap_iface(const std::string& str);
        bool set_ap_iface(const char buffer[], size_t size);
        std::string ssid_str();
        char* ssid(size_t length = 0);
        bool set_ssid(const std::string& str);
        bool set_ssid(const char buffer[], size_t size);
        std::string pass_str();
        char* pass(size_t length = 0);
        bool set_pass(const std::string& str);
        bool set_pass(const char buffer[], size_t size);
        uint32_t& security_type();
        sMacAddr& preferred_bssid();
        uint8_t& wire_iface_type();
        uint8_t& wireless_iface_type();
        uint8_t& wired_backhaul();
        uint8_t& mem_only_psk();
        uint8_t& backhaul_preferred_radio_band();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BACKHAUL* m_action_op = nullptr;
        char* m_bridge_iface = nullptr;
        size_t m_bridge_iface_idx__ = 0;
        int m_lock_order_counter__ = 0;
        sMacAddr* m_iface_mac = nullptr;
        uint8_t* m_iface_is_5ghz = nullptr;
        char* m_wire_iface = nullptr;
        size_t m_wire_iface_idx__ = 0;
        char* m_sta_iface = nullptr;
        size_t m_sta_iface_idx__ = 0;
        char* m_ap_iface = nullptr;
        size_t m_ap_iface_idx__ = 0;
        char* m_ssid = nullptr;
        size_t m_ssid_idx__ = 0;
        char* m_pass = nullptr;
        size_t m_pass_idx__ = 0;
        uint32_t* m_security_type = nullptr;
        sMacAddr* m_preferred_bssid = nullptr;
        uint8_t* m_wire_iface_type = nullptr;
        uint8_t* m_wireless_iface_type = nullptr;
        uint8_t* m_wired_backhaul = nullptr;
        uint8_t* m_mem_only_psk = nullptr;
        uint8_t* m_backhaul_preferred_radio_band = nullptr;
};

class cACTION_BACKHAUL_CONNECTED_NOTIFICATION : public BaseClass
{
    public:
        cACTION_BACKHAUL_CONNECTED_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_BACKHAUL_CONNECTED_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_BACKHAUL_CONNECTED_NOTIFICATION();

        static eActionOp_BACKHAUL get_action_op(){
            return (eActionOp_BACKHAUL)(ACTION_BACKHAUL_CONNECTED_NOTIFICATION);
        }
        sBackhaulParams& params();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BACKHAUL* m_action_op = nullptr;
        sBackhaulParams* m_params = nullptr;
};

class cACTION_BACKHAUL_DISCONNECTED_NOTIFICATION : public BaseClass
{
    public:
        cACTION_BACKHAUL_DISCONNECTED_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_BACKHAUL_DISCONNECTED_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_BACKHAUL_DISCONNECTED_NOTIFICATION();

        static eActionOp_BACKHAUL get_action_op(){
            return (eActionOp_BACKHAUL)(ACTION_BACKHAUL_DISCONNECTED_NOTIFICATION);
        }
        uint8_t& stopped();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BACKHAUL* m_action_op = nullptr;
        uint8_t* m_stopped = nullptr;
};

class cACTION_BACKHAUL_ENABLE_APS_REQUEST : public BaseClass
{
    public:
        cACTION_BACKHAUL_ENABLE_APS_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_BACKHAUL_ENABLE_APS_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_BACKHAUL_ENABLE_APS_REQUEST();

        static eActionOp_BACKHAUL get_action_op(){
            return (eActionOp_BACKHAUL)(ACTION_BACKHAUL_ENABLE_APS_REQUEST);
        }
        uint8_t& channel();
        uint32_t& bandwidth();
        uint8_t& center_channel();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BACKHAUL* m_action_op = nullptr;
        uint8_t* m_channel = nullptr;
        uint32_t* m_bandwidth = nullptr;
        uint8_t* m_center_channel = nullptr;
};

class cACTION_BACKHAUL_ROAM_REQUEST : public BaseClass
{
    public:
        cACTION_BACKHAUL_ROAM_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_BACKHAUL_ROAM_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_BACKHAUL_ROAM_REQUEST();

        static eActionOp_BACKHAUL get_action_op(){
            return (eActionOp_BACKHAUL)(ACTION_BACKHAUL_ROAM_REQUEST);
        }
        sBackhaulRoam& params();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BACKHAUL* m_action_op = nullptr;
        sBackhaulRoam* m_params = nullptr;
};

class cACTION_BACKHAUL_ROAM_RESPONSE : public BaseClass
{
    public:
        cACTION_BACKHAUL_ROAM_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_BACKHAUL_ROAM_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_BACKHAUL_ROAM_RESPONSE();

        static eActionOp_BACKHAUL get_action_op(){
            return (eActionOp_BACKHAUL)(ACTION_BACKHAUL_ROAM_RESPONSE);
        }
        uint8_t& connected();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BACKHAUL* m_action_op = nullptr;
        uint8_t* m_connected = nullptr;
};

class cACTION_BACKHAUL_RESET : public BaseClass
{
    public:
        cACTION_BACKHAUL_RESET(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_BACKHAUL_RESET(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_BACKHAUL_RESET();

        static eActionOp_BACKHAUL get_action_op(){
            return (eActionOp_BACKHAUL)(ACTION_BACKHAUL_RESET);
        }
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BACKHAUL* m_action_op = nullptr;
};

class cACTION_BACKHAUL_4ADDR_CONNECTED : public BaseClass
{
    public:
        cACTION_BACKHAUL_4ADDR_CONNECTED(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_BACKHAUL_4ADDR_CONNECTED(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_BACKHAUL_4ADDR_CONNECTED();

        static eActionOp_BACKHAUL get_action_op(){
            return (eActionOp_BACKHAUL)(ACTION_BACKHAUL_4ADDR_CONNECTED);
        }
        sMacAddr& mac();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BACKHAUL* m_action_op = nullptr;
        sMacAddr* m_mac = nullptr;
};

class cACTION_BACKHAUL_DL_RSSI_REPORT_NOTIFICATION : public BaseClass
{
    public:
        cACTION_BACKHAUL_DL_RSSI_REPORT_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_BACKHAUL_DL_RSSI_REPORT_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_BACKHAUL_DL_RSSI_REPORT_NOTIFICATION();

        static eActionOp_BACKHAUL get_action_op(){
            return (eActionOp_BACKHAUL)(ACTION_BACKHAUL_DL_RSSI_REPORT_NOTIFICATION);
        }
        sBackhaulRssi& params();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BACKHAUL* m_action_op = nullptr;
        sBackhaulRssi* m_params = nullptr;
};

class cACTION_BACKHAUL_UPDATE_STOP_ON_FAILURE_ATTEMPTS_REQUEST : public BaseClass
{
    public:
        cACTION_BACKHAUL_UPDATE_STOP_ON_FAILURE_ATTEMPTS_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_BACKHAUL_UPDATE_STOP_ON_FAILURE_ATTEMPTS_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_BACKHAUL_UPDATE_STOP_ON_FAILURE_ATTEMPTS_REQUEST();

        static eActionOp_BACKHAUL get_action_op(){
            return (eActionOp_BACKHAUL)(ACTION_BACKHAUL_UPDATE_STOP_ON_FAILURE_ATTEMPTS_REQUEST);
        }
        uint32_t& attempts();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BACKHAUL* m_action_op = nullptr;
        uint32_t* m_attempts = nullptr;
};

class cACTION_BACKHAUL_CLIENT_RX_RSSI_MEASUREMENT_REQUEST : public BaseClass
{
    public:
        cACTION_BACKHAUL_CLIENT_RX_RSSI_MEASUREMENT_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_BACKHAUL_CLIENT_RX_RSSI_MEASUREMENT_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_BACKHAUL_CLIENT_RX_RSSI_MEASUREMENT_REQUEST();

        static eActionOp_BACKHAUL get_action_op(){
            return (eActionOp_BACKHAUL)(ACTION_BACKHAUL_CLIENT_RX_RSSI_MEASUREMENT_REQUEST);
        }
        sNodeRssiMeasurementRequest& params();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BACKHAUL* m_action_op = nullptr;
        sNodeRssiMeasurementRequest* m_params = nullptr;
};

class cACTION_BACKHAUL_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE : public BaseClass
{
    public:
        cACTION_BACKHAUL_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_BACKHAUL_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_BACKHAUL_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE();

        static eActionOp_BACKHAUL get_action_op(){
            return (eActionOp_BACKHAUL)(ACTION_BACKHAUL_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE);
        }
        sNodeRssiMeasurement& params();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BACKHAUL* m_action_op = nullptr;
        sNodeRssiMeasurement* m_params = nullptr;
};

class cACTION_BACKHAUL_CLIENT_RX_RSSI_MEASUREMENT_CMD_RESPONSE : public BaseClass
{
    public:
        cACTION_BACKHAUL_CLIENT_RX_RSSI_MEASUREMENT_CMD_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_BACKHAUL_CLIENT_RX_RSSI_MEASUREMENT_CMD_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_BACKHAUL_CLIENT_RX_RSSI_MEASUREMENT_CMD_RESPONSE();

        static eActionOp_BACKHAUL get_action_op(){
            return (eActionOp_BACKHAUL)(ACTION_BACKHAUL_CLIENT_RX_RSSI_MEASUREMENT_CMD_RESPONSE);
        }
        sMacAddr& mac();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BACKHAUL* m_action_op = nullptr;
        sMacAddr* m_mac = nullptr;
};

}; // close namespace: beerocks_message

#endif //_BEEROCKS/TLVF_BEEROCKS_MESSAGE_BACKHAUL_H_
