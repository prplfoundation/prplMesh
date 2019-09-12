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

#ifndef _BEEROCKS_TLVF_BEEROCKS_MESSAGE_CLI_H_
#define _BEEROCKS_TLVF_BEEROCKS_MESSAGE_CLI_H_

#include <cstddef>
#include <stdint.h>
#include <tlvf/swap.h>
#include <string.h>
#include <memory>
#include <tlvf/BaseClass.h>
#include <tuple>
#include <tlvf/tlvfutils.h>
#include "beerocks/tlvf/beerocks_message_common.h"
#include "beerocks/tlvf/beerocks_message_cli_net_map.h"

namespace beerocks_message {


class cACTION_CLI_ENABLE_DIAGNOSTICS_MEASUREMENTS : public BaseClass
{
    public:
        cACTION_CLI_ENABLE_DIAGNOSTICS_MEASUREMENTS(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_CLI_ENABLE_DIAGNOSTICS_MEASUREMENTS(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_CLI_ENABLE_DIAGNOSTICS_MEASUREMENTS();

        static eActionOp_CLI get_action_op(){
            return (eActionOp_CLI)(ACTION_CLI_ENABLE_DIAGNOSTICS_MEASUREMENTS);
        }
        int8_t& isEnable();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_CLI* m_action_op = nullptr;
        int8_t* m_isEnable = nullptr;
};

class cACTION_CLI_ENABLE_LOAD_BALANCER : public BaseClass
{
    public:
        cACTION_CLI_ENABLE_LOAD_BALANCER(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_CLI_ENABLE_LOAD_BALANCER(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_CLI_ENABLE_LOAD_BALANCER();

        static eActionOp_CLI get_action_op(){
            return (eActionOp_CLI)(ACTION_CLI_ENABLE_LOAD_BALANCER);
        }
        int8_t& isEnable();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_CLI* m_action_op = nullptr;
        int8_t* m_isEnable = nullptr;
};

class cACTION_CLI_ENABLE_DEBUG : public BaseClass
{
    public:
        cACTION_CLI_ENABLE_DEBUG(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_CLI_ENABLE_DEBUG(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_CLI_ENABLE_DEBUG();

        static eActionOp_CLI get_action_op(){
            return (eActionOp_CLI)(ACTION_CLI_ENABLE_DEBUG);
        }
        int8_t& isEnable();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_CLI* m_action_op = nullptr;
        int8_t* m_isEnable = nullptr;
};

class cACTION_CLI_SET_SLAVES_STOP_ON_FAILURE_ATTEMPTS : public BaseClass
{
    public:
        cACTION_CLI_SET_SLAVES_STOP_ON_FAILURE_ATTEMPTS(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_CLI_SET_SLAVES_STOP_ON_FAILURE_ATTEMPTS(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_CLI_SET_SLAVES_STOP_ON_FAILURE_ATTEMPTS();

        static eActionOp_CLI get_action_op(){
            return (eActionOp_CLI)(ACTION_CLI_SET_SLAVES_STOP_ON_FAILURE_ATTEMPTS);
        }
        int32_t& attempts();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_CLI* m_action_op = nullptr;
        int32_t* m_attempts = nullptr;
};

class cACTION_CLI_RESPONSE_INT : public BaseClass
{
    public:
        cACTION_CLI_RESPONSE_INT(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_CLI_RESPONSE_INT(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_CLI_RESPONSE_INT();

        static eActionOp_CLI get_action_op(){
            return (eActionOp_CLI)(ACTION_CLI_RESPONSE_INT);
        }
        uint8_t& isOK();
        int8_t& currentValue();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_CLI* m_action_op = nullptr;
        uint8_t* m_isOK = nullptr;
        int8_t* m_currentValue = nullptr;
};

class cACTION_CLI_RESPONSE_STR : public BaseClass
{
    public:
        cACTION_CLI_RESPONSE_STR(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_CLI_RESPONSE_STR(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_CLI_RESPONSE_STR();

        static eActionOp_CLI get_action_op(){
            return (eActionOp_CLI)(ACTION_CLI_RESPONSE_STR);
        }
        uint32_t& buffer_size();
        std::string buffer_str();
        char* buffer(size_t length = 0);
        bool set_buffer(const std::string& str);
        bool set_buffer(const char buffer[], size_t size);
        bool alloc_buffer(size_t count = 1);
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_CLI* m_action_op = nullptr;
        uint32_t* m_buffer_size = nullptr;
        char* m_buffer = nullptr;
        size_t m_buffer_idx__ = 0;
        int m_lock_order_counter__ = 0;
};

class cACTION_CLI_CROSS_RX_RSSI_MEASUREMENT : public BaseClass
{
    public:
        cACTION_CLI_CROSS_RX_RSSI_MEASUREMENT(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_CLI_CROSS_RX_RSSI_MEASUREMENT(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_CLI_CROSS_RX_RSSI_MEASUREMENT();

        static eActionOp_CLI get_action_op(){
            return (eActionOp_CLI)(ACTION_CLI_CROSS_RX_RSSI_MEASUREMENT);
        }
        sMacAddr& client_mac();
        sMacAddr& hostap_mac();
        uint16_t& center_frequency();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_CLI* m_action_op = nullptr;
        sMacAddr* m_client_mac = nullptr;
        sMacAddr* m_hostap_mac = nullptr;
        uint16_t* m_center_frequency = nullptr;
};

class cACTION_CLI_OPTIMAL_PATH_TASK : public BaseClass
{
    public:
        cACTION_CLI_OPTIMAL_PATH_TASK(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_CLI_OPTIMAL_PATH_TASK(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_CLI_OPTIMAL_PATH_TASK();

        static eActionOp_CLI get_action_op(){
            return (eActionOp_CLI)(ACTION_CLI_OPTIMAL_PATH_TASK);
        }
        sMacAddr& client_mac();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_CLI* m_action_op = nullptr;
        sMacAddr* m_client_mac = nullptr;
};

class cACTION_CLI_LOAD_BALANCER_TASK : public BaseClass
{
    public:
        cACTION_CLI_LOAD_BALANCER_TASK(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_CLI_LOAD_BALANCER_TASK(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_CLI_LOAD_BALANCER_TASK();

        static eActionOp_CLI get_action_op(){
            return (eActionOp_CLI)(ACTION_CLI_LOAD_BALANCER_TASK);
        }
        sMacAddr& ap_mac();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_CLI* m_action_op = nullptr;
        sMacAddr* m_ap_mac = nullptr;
};

class cACTION_CLI_IRE_NETWORK_OPTIMIZATION_TASK : public BaseClass
{
    public:
        cACTION_CLI_IRE_NETWORK_OPTIMIZATION_TASK(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_CLI_IRE_NETWORK_OPTIMIZATION_TASK(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_CLI_IRE_NETWORK_OPTIMIZATION_TASK();

        static eActionOp_CLI get_action_op(){
            return (eActionOp_CLI)(ACTION_CLI_IRE_NETWORK_OPTIMIZATION_TASK);
        }
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_CLI* m_action_op = nullptr;
};

class cACTION_CLI_DUMP_NODE_INFO : public BaseClass
{
    public:
        cACTION_CLI_DUMP_NODE_INFO(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_CLI_DUMP_NODE_INFO(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_CLI_DUMP_NODE_INFO();

        static eActionOp_CLI get_action_op(){
            return (eActionOp_CLI)(ACTION_CLI_DUMP_NODE_INFO);
        }
        sMacAddr& mac();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_CLI* m_action_op = nullptr;
        sMacAddr* m_mac = nullptr;
};

class cACTION_CLI_PING_SLAVE_REQUEST : public BaseClass
{
    public:
        cACTION_CLI_PING_SLAVE_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_CLI_PING_SLAVE_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_CLI_PING_SLAVE_REQUEST();

        static eActionOp_CLI get_action_op(){
            return (eActionOp_CLI)(ACTION_CLI_PING_SLAVE_REQUEST);
        }
        sMacAddr& mac();
        uint16_t& num_of_req();
        uint16_t& size();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_CLI* m_action_op = nullptr;
        sMacAddr* m_mac = nullptr;
        uint16_t* m_num_of_req = nullptr;
        uint16_t* m_size = nullptr;
};

class cACTION_CLI_PING_ALL_SLAVES_REQUEST : public BaseClass
{
    public:
        cACTION_CLI_PING_ALL_SLAVES_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_CLI_PING_ALL_SLAVES_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_CLI_PING_ALL_SLAVES_REQUEST();

        static eActionOp_CLI get_action_op(){
            return (eActionOp_CLI)(ACTION_CLI_PING_ALL_SLAVES_REQUEST);
        }
        uint16_t& num_of_req();
        uint16_t& size();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_CLI* m_action_op = nullptr;
        uint16_t* m_num_of_req = nullptr;
        uint16_t* m_size = nullptr;
};

class cACTION_CLI_BACKHAUL_SCAN_RESULTS : public BaseClass
{
    public:
        cACTION_CLI_BACKHAUL_SCAN_RESULTS(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_CLI_BACKHAUL_SCAN_RESULTS(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_CLI_BACKHAUL_SCAN_RESULTS();

        static eActionOp_CLI get_action_op(){
            return (eActionOp_CLI)(ACTION_CLI_BACKHAUL_SCAN_RESULTS);
        }
        sMacAddr& mac();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_CLI* m_action_op = nullptr;
        sMacAddr* m_mac = nullptr;
};

class cACTION_CLI_BACKHAUL_ROAM_REQUEST : public BaseClass
{
    public:
        cACTION_CLI_BACKHAUL_ROAM_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_CLI_BACKHAUL_ROAM_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_CLI_BACKHAUL_ROAM_REQUEST();

        static eActionOp_CLI get_action_op(){
            return (eActionOp_CLI)(ACTION_CLI_BACKHAUL_ROAM_REQUEST);
        }
        sMacAddr& slave_mac();
        sMacAddr& bssid();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_CLI* m_action_op = nullptr;
        sMacAddr* m_slave_mac = nullptr;
        sMacAddr* m_bssid = nullptr;
};

class cACTION_CLI_CLIENT_ALLOW_REQUEST : public BaseClass
{
    public:
        cACTION_CLI_CLIENT_ALLOW_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_CLI_CLIENT_ALLOW_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_CLI_CLIENT_ALLOW_REQUEST();

        static eActionOp_CLI get_action_op(){
            return (eActionOp_CLI)(ACTION_CLI_CLIENT_ALLOW_REQUEST);
        }
        sMacAddr& client_mac();
        sMacAddr& hostap_mac();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_CLI* m_action_op = nullptr;
        sMacAddr* m_client_mac = nullptr;
        sMacAddr* m_hostap_mac = nullptr;
};

class cACTION_CLI_CLIENT_DISALLOW_REQUEST : public BaseClass
{
    public:
        cACTION_CLI_CLIENT_DISALLOW_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_CLI_CLIENT_DISALLOW_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_CLI_CLIENT_DISALLOW_REQUEST();

        static eActionOp_CLI get_action_op(){
            return (eActionOp_CLI)(ACTION_CLI_CLIENT_DISALLOW_REQUEST);
        }
        sMacAddr& client_mac();
        sMacAddr& hostap_mac();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_CLI* m_action_op = nullptr;
        sMacAddr* m_client_mac = nullptr;
        sMacAddr* m_hostap_mac = nullptr;
};

class cACTION_CLI_CLIENT_DISCONNECT_REQUEST : public BaseClass
{
    public:
        cACTION_CLI_CLIENT_DISCONNECT_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_CLI_CLIENT_DISCONNECT_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_CLI_CLIENT_DISCONNECT_REQUEST();

        static eActionOp_CLI get_action_op(){
            return (eActionOp_CLI)(ACTION_CLI_CLIENT_DISCONNECT_REQUEST);
        }
        sMacAddr& client_mac();
        uint32_t& type();
        uint32_t& reason();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_CLI* m_action_op = nullptr;
        sMacAddr* m_client_mac = nullptr;
        uint32_t* m_type = nullptr;
        uint32_t* m_reason = nullptr;
};

class cACTION_CLI_CLIENT_BSS_STEER_REQUEST : public BaseClass
{
    public:
        cACTION_CLI_CLIENT_BSS_STEER_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_CLI_CLIENT_BSS_STEER_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_CLI_CLIENT_BSS_STEER_REQUEST();

        static eActionOp_CLI get_action_op(){
            return (eActionOp_CLI)(ACTION_CLI_CLIENT_BSS_STEER_REQUEST);
        }
        sMacAddr& client_mac();
        sMacAddr& bssid();
        uint32_t& disassoc_timer();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_CLI* m_action_op = nullptr;
        sMacAddr* m_client_mac = nullptr;
        sMacAddr* m_bssid = nullptr;
        uint32_t* m_disassoc_timer = nullptr;
};

class cACTION_CLI_CLIENT_LINK_MEASUREMENT_11K_REQUEST : public BaseClass
{
    public:
        cACTION_CLI_CLIENT_LINK_MEASUREMENT_11K_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_CLI_CLIENT_LINK_MEASUREMENT_11K_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_CLI_CLIENT_LINK_MEASUREMENT_11K_REQUEST();

        static eActionOp_CLI get_action_op(){
            return (eActionOp_CLI)(ACTION_CLI_CLIENT_LINK_MEASUREMENT_11K_REQUEST);
        }
        sMacAddr& hostap_mac();
        sMacAddr& client_mac();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_CLI* m_action_op = nullptr;
        sMacAddr* m_hostap_mac = nullptr;
        sMacAddr* m_client_mac = nullptr;
};

class cACTION_CLI_CLIENT_CHANNEL_LOAD_11K_REQUEST : public BaseClass
{
    public:
        cACTION_CLI_CLIENT_CHANNEL_LOAD_11K_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_CLI_CLIENT_CHANNEL_LOAD_11K_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_CLI_CLIENT_CHANNEL_LOAD_11K_REQUEST();

        static eActionOp_CLI get_action_op(){
            return (eActionOp_CLI)(ACTION_CLI_CLIENT_CHANNEL_LOAD_11K_REQUEST);
        }
        sMacAddr& hostap_mac();
        sMacAddr& client_mac();
        uint8_t& channel();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_CLI* m_action_op = nullptr;
        sMacAddr* m_hostap_mac = nullptr;
        sMacAddr* m_client_mac = nullptr;
        uint8_t* m_channel = nullptr;
};

class cACTION_CLI_CLIENT_BEACON_11K_REQUEST : public BaseClass
{
    public:
        cACTION_CLI_CLIENT_BEACON_11K_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_CLI_CLIENT_BEACON_11K_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_CLI_CLIENT_BEACON_11K_REQUEST();

        static eActionOp_CLI get_action_op(){
            return (eActionOp_CLI)(ACTION_CLI_CLIENT_BEACON_11K_REQUEST);
        }
        sMacAddr& client_mac();
        sMacAddr& bssid();
        uint8_t* ssid(size_t idx = 0);
        uint8_t& use_optional_ssid();
        uint8_t& channel();
        uint8_t& measurement_mode();
        uint16_t& duration();
        uint16_t& rand_ival();
        uint16_t& repeats();
        int16_t& op_class();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_CLI* m_action_op = nullptr;
        sMacAddr* m_client_mac = nullptr;
        sMacAddr* m_bssid = nullptr;
        uint8_t* m_ssid = nullptr;
        size_t m_ssid_idx__ = 0;
        int m_lock_order_counter__ = 0;
        uint8_t* m_use_optional_ssid = nullptr;
        uint8_t* m_channel = nullptr;
        uint8_t* m_measurement_mode = nullptr;
        uint16_t* m_duration = nullptr;
        uint16_t* m_rand_ival = nullptr;
        uint16_t* m_repeats = nullptr;
        int16_t* m_op_class = nullptr;
};

class cACTION_CLI_CLIENT_STATISTICS_11K_REQUEST : public BaseClass
{
    public:
        cACTION_CLI_CLIENT_STATISTICS_11K_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_CLI_CLIENT_STATISTICS_11K_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_CLI_CLIENT_STATISTICS_11K_REQUEST();

        static eActionOp_CLI get_action_op(){
            return (eActionOp_CLI)(ACTION_CLI_CLIENT_STATISTICS_11K_REQUEST);
        }
        sMacAddr& hostap_mac();
        sMacAddr& client_mac();
        sMacAddr& peer_mac();
        uint8_t& group_identity();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_CLI* m_action_op = nullptr;
        sMacAddr* m_hostap_mac = nullptr;
        sMacAddr* m_client_mac = nullptr;
        sMacAddr* m_peer_mac = nullptr;
        uint8_t* m_group_identity = nullptr;
};

class cACTION_CLI_HOSTAP_CHANNEL_SWITCH_REQUEST : public BaseClass
{
    public:
        cACTION_CLI_HOSTAP_CHANNEL_SWITCH_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_CLI_HOSTAP_CHANNEL_SWITCH_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_CLI_HOSTAP_CHANNEL_SWITCH_REQUEST();

        static eActionOp_CLI get_action_op(){
            return (eActionOp_CLI)(ACTION_CLI_HOSTAP_CHANNEL_SWITCH_REQUEST);
        }
        sMacAddr& mac();
        sApChannelSwitch& cs_params();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_CLI* m_action_op = nullptr;
        sMacAddr* m_mac = nullptr;
        sApChannelSwitch* m_cs_params = nullptr;
};

class cACTION_CLI_HOSTAP_TX_ON_REQUEST : public BaseClass
{
    public:
        cACTION_CLI_HOSTAP_TX_ON_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_CLI_HOSTAP_TX_ON_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_CLI_HOSTAP_TX_ON_REQUEST();

        static eActionOp_CLI get_action_op(){
            return (eActionOp_CLI)(ACTION_CLI_HOSTAP_TX_ON_REQUEST);
        }
        sMacAddr& ap_mac();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_CLI* m_action_op = nullptr;
        sMacAddr* m_ap_mac = nullptr;
};

class cACTION_CLI_HOSTAP_TX_OFF_REQUEST : public BaseClass
{
    public:
        cACTION_CLI_HOSTAP_TX_OFF_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_CLI_HOSTAP_TX_OFF_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_CLI_HOSTAP_TX_OFF_REQUEST();

        static eActionOp_CLI get_action_op(){
            return (eActionOp_CLI)(ACTION_CLI_HOSTAP_TX_OFF_REQUEST);
        }
        sMacAddr& ap_mac();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_CLI* m_action_op = nullptr;
        sMacAddr* m_ap_mac = nullptr;
};

class cACTION_CLI_HOSTAP_SET_NEIGHBOR_11K_REQUEST : public BaseClass
{
    public:
        cACTION_CLI_HOSTAP_SET_NEIGHBOR_11K_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_CLI_HOSTAP_SET_NEIGHBOR_11K_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_CLI_HOSTAP_SET_NEIGHBOR_11K_REQUEST();

        static eActionOp_CLI get_action_op(){
            return (eActionOp_CLI)(ACTION_CLI_HOSTAP_SET_NEIGHBOR_11K_REQUEST);
        }
        sMacAddr& ap_mac();
        sMacAddr& bssid();
        uint8_t& channel();
        int8_t& vap_id();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_CLI* m_action_op = nullptr;
        sMacAddr* m_ap_mac = nullptr;
        sMacAddr* m_bssid = nullptr;
        uint8_t* m_channel = nullptr;
        int8_t* m_vap_id = nullptr;
};

class cACTION_CLI_HOSTAP_REMOVE_NEIGHBOR_11K_REQUEST : public BaseClass
{
    public:
        cACTION_CLI_HOSTAP_REMOVE_NEIGHBOR_11K_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_CLI_HOSTAP_REMOVE_NEIGHBOR_11K_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_CLI_HOSTAP_REMOVE_NEIGHBOR_11K_REQUEST();

        static eActionOp_CLI get_action_op(){
            return (eActionOp_CLI)(ACTION_CLI_HOSTAP_REMOVE_NEIGHBOR_11K_REQUEST);
        }
        sMacAddr& ap_mac();
        sMacAddr& bssid();
        int8_t& vap_id();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_CLI* m_action_op = nullptr;
        sMacAddr* m_ap_mac = nullptr;
        sMacAddr* m_bssid = nullptr;
        int8_t* m_vap_id = nullptr;
};

class cACTION_CLI_HOSTAP_STATS_MEASUREMENT : public BaseClass
{
    public:
        cACTION_CLI_HOSTAP_STATS_MEASUREMENT(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_CLI_HOSTAP_STATS_MEASUREMENT(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_CLI_HOSTAP_STATS_MEASUREMENT();

        static eActionOp_CLI get_action_op(){
            return (eActionOp_CLI)(ACTION_CLI_HOSTAP_STATS_MEASUREMENT);
        }
        sMacAddr& ap_mac();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_CLI* m_action_op = nullptr;
        sMacAddr* m_ap_mac = nullptr;
};

}; // close namespace: beerocks_message

#endif //_BEEROCKS/TLVF_BEEROCKS_MESSAGE_CLI_H_
