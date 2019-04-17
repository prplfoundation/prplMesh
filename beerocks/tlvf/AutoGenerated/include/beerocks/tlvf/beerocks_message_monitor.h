///////////////////////////////////////
// AUTO GENERATED FILE - DO NOT EDIT //
///////////////////////////////////////

/*
#############################################################################
# INTEL CONFIDENTIAL
# Copyright 2018 Intel Corporation All Rights Reserved.
#
# The source code contained or described herein and all documents related to
# the source code ("Material") are owned by Intel Corporation or its
# suppliers or licensors.  Title to the Material remains with Intel
# Corporation or its suppliers and licensors.  The Material contains trade
# secrets and proprietary and confidential information of Intel or its
# suppliers and licensors.  The Material is protected by worldwide copyright
# and trade secret laws and treaty provisions. No part of the Material may
# be used, copied, reproduced, modified, published, uploaded, posted,
# transmitted, distributed, or disclosed in any way without Intel's prior
# express written permission.
#
# No license under any patent, copyright, trade secret or other intellectual
# property right is granted to or conferred upon you by disclosure or
# delivery of the Materials,  either expressly, by implication, inducement,
# estoppel or otherwise.  Any license under such intellectual property
# rights must be express and approved by Intel in writing.
#############################################################################
*/

#ifndef _BEEROCKS_TLVF_BEEROCKS_MESSAGE_MONITOR_H_
#define _BEEROCKS_TLVF_BEEROCKS_MESSAGE_MONITOR_H_

#include <cstddef>
#include <stdint.h>
#include <tlvf/swap.h>
#include <string.h>
#include <memory>
#include <tlvf/BaseClass.h>
#include <tuple>
#include "beerocks/tlvf/beerocks_message_common.h"

namespace beerocks_message {


class cACTION_MONITOR_HOSTAP_AP_DISABLED_NOTIFICATION : public BaseClass
{
    public:
        cACTION_MONITOR_HOSTAP_AP_DISABLED_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_MONITOR_HOSTAP_AP_DISABLED_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_MONITOR_HOSTAP_AP_DISABLED_NOTIFICATION();

        static eActionOp_MONITOR get_action_op(){
            return (eActionOp_MONITOR)(ACTION_MONITOR_HOSTAP_AP_DISABLED_NOTIFICATION);
        }
        int8_t& vap_id();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_MONITOR* m_action_op = nullptr;
        int8_t* m_vap_id = nullptr;
};

class cACTION_MONITOR_JOINED_NOTIFICATION : public BaseClass
{
    public:
        cACTION_MONITOR_JOINED_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_MONITOR_JOINED_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_MONITOR_JOINED_NOTIFICATION();

        static eActionOp_MONITOR get_action_op(){
            return (eActionOp_MONITOR)(ACTION_MONITOR_JOINED_NOTIFICATION);
        }
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_MONITOR* m_action_op = nullptr;
};

class cACTION_MONITOR_SON_CONFIG_UPDATE : public BaseClass
{
    public:
        cACTION_MONITOR_SON_CONFIG_UPDATE(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_MONITOR_SON_CONFIG_UPDATE(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_MONITOR_SON_CONFIG_UPDATE();

        static eActionOp_MONITOR get_action_op(){
            return (eActionOp_MONITOR)(ACTION_MONITOR_SON_CONFIG_UPDATE);
        }
        sSonConfig& config();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_MONITOR* m_action_op = nullptr;
        sSonConfig* m_config = nullptr;
};

class cACTION_MONITOR_CHANGE_MODULE_LOGGING_LEVEL : public BaseClass
{
    public:
        cACTION_MONITOR_CHANGE_MODULE_LOGGING_LEVEL(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_MONITOR_CHANGE_MODULE_LOGGING_LEVEL(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_MONITOR_CHANGE_MODULE_LOGGING_LEVEL();

        static eActionOp_MONITOR get_action_op(){
            return (eActionOp_MONITOR)(ACTION_MONITOR_CHANGE_MODULE_LOGGING_LEVEL);
        }
        sLoggingLevelChange& params();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_MONITOR* m_action_op = nullptr;
        sLoggingLevelChange* m_params = nullptr;
};

class cACTION_MONITOR_ERROR_NOTIFICATION : public BaseClass
{
    public:
        cACTION_MONITOR_ERROR_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_MONITOR_ERROR_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_MONITOR_ERROR_NOTIFICATION();

        static eActionOp_MONITOR get_action_op(){
            return (eActionOp_MONITOR)(ACTION_MONITOR_ERROR_NOTIFICATION);
        }
        uint32_t& error_code();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_MONITOR* m_action_op = nullptr;
        uint32_t* m_error_code = nullptr;
};

class cACTION_MONITOR_ERROR_NOTIFICATION_ACK : public BaseClass
{
    public:
        cACTION_MONITOR_ERROR_NOTIFICATION_ACK(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_MONITOR_ERROR_NOTIFICATION_ACK(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_MONITOR_ERROR_NOTIFICATION_ACK();

        static eActionOp_MONITOR get_action_op(){
            return (eActionOp_MONITOR)(ACTION_MONITOR_ERROR_NOTIFICATION_ACK);
        }
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_MONITOR* m_action_op = nullptr;
};

class cACTION_MONITOR_HEARTBEAT_NOTIFICATION : public BaseClass
{
    public:
        cACTION_MONITOR_HEARTBEAT_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_MONITOR_HEARTBEAT_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_MONITOR_HEARTBEAT_NOTIFICATION();

        static eActionOp_MONITOR get_action_op(){
            return (eActionOp_MONITOR)(ACTION_MONITOR_HEARTBEAT_NOTIFICATION);
        }
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_MONITOR* m_action_op = nullptr;
};

class cACTION_MONITOR_CLIENT_START_MONITORING_REQUEST : public BaseClass
{
    public:
        cACTION_MONITOR_CLIENT_START_MONITORING_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_MONITOR_CLIENT_START_MONITORING_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_MONITOR_CLIENT_START_MONITORING_REQUEST();

        static eActionOp_MONITOR get_action_op(){
            return (eActionOp_MONITOR)(ACTION_MONITOR_CLIENT_START_MONITORING_REQUEST);
        }
        sClientMonitoringParams& params();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_MONITOR* m_action_op = nullptr;
        sClientMonitoringParams* m_params = nullptr;
};

class cACTION_MONITOR_CLIENT_STOP_MONITORING_REQUEST : public BaseClass
{
    public:
        cACTION_MONITOR_CLIENT_STOP_MONITORING_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_MONITOR_CLIENT_STOP_MONITORING_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_MONITOR_CLIENT_STOP_MONITORING_REQUEST();

        static eActionOp_MONITOR get_action_op(){
            return (eActionOp_MONITOR)(ACTION_MONITOR_CLIENT_STOP_MONITORING_REQUEST);
        }
        beerocks::net::sMacAddr& mac();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_MONITOR* m_action_op = nullptr;
        beerocks::net::sMacAddr* m_mac = nullptr;
};

class cACTION_MONITOR_CLIENT_RX_RSSI_MEASUREMENT_REQUEST : public BaseClass
{
    public:
        cACTION_MONITOR_CLIENT_RX_RSSI_MEASUREMENT_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_MONITOR_CLIENT_RX_RSSI_MEASUREMENT_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_MONITOR_CLIENT_RX_RSSI_MEASUREMENT_REQUEST();

        static eActionOp_MONITOR get_action_op(){
            return (eActionOp_MONITOR)(ACTION_MONITOR_CLIENT_RX_RSSI_MEASUREMENT_REQUEST);
        }
        sNodeRssiMeasurementRequest& params();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_MONITOR* m_action_op = nullptr;
        sNodeRssiMeasurementRequest* m_params = nullptr;
};

class cACTION_MONITOR_CLIENT_DISCONNECT_REQUEST : public BaseClass
{
    public:
        cACTION_MONITOR_CLIENT_DISCONNECT_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_MONITOR_CLIENT_DISCONNECT_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_MONITOR_CLIENT_DISCONNECT_REQUEST();

        static eActionOp_MONITOR get_action_op(){
            return (eActionOp_MONITOR)(ACTION_MONITOR_CLIENT_DISCONNECT_REQUEST);
        }
        beerocks::net::sMacAddr& mac();
        beerocks::net::sIpv4Addr& ipv4();
        uint8_t& channel();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_MONITOR* m_action_op = nullptr;
        beerocks::net::sMacAddr* m_mac = nullptr;
        beerocks::net::sIpv4Addr* m_ipv4 = nullptr;
        uint8_t* m_channel = nullptr;
};

class cACTION_MONITOR_CLIENT_RX_RSSI_MEASUREMENT_NOTIFICATION : public BaseClass
{
    public:
        cACTION_MONITOR_CLIENT_RX_RSSI_MEASUREMENT_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_MONITOR_CLIENT_RX_RSSI_MEASUREMENT_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_MONITOR_CLIENT_RX_RSSI_MEASUREMENT_NOTIFICATION();

        static eActionOp_MONITOR get_action_op(){
            return (eActionOp_MONITOR)(ACTION_MONITOR_CLIENT_RX_RSSI_MEASUREMENT_NOTIFICATION);
        }
        sNodeRssiMeasurement& params();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_MONITOR* m_action_op = nullptr;
        sNodeRssiMeasurement* m_params = nullptr;
};

class cACTION_MONITOR_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE : public BaseClass
{
    public:
        cACTION_MONITOR_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_MONITOR_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_MONITOR_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE();

        static eActionOp_MONITOR get_action_op(){
            return (eActionOp_MONITOR)(ACTION_MONITOR_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE);
        }
        sNodeRssiMeasurement& params();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_MONITOR* m_action_op = nullptr;
        sNodeRssiMeasurement* m_params = nullptr;
};

class cACTION_MONITOR_CLIENT_NO_RESPONSE_NOTIFICATION : public BaseClass
{
    public:
        cACTION_MONITOR_CLIENT_NO_RESPONSE_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_MONITOR_CLIENT_NO_RESPONSE_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_MONITOR_CLIENT_NO_RESPONSE_NOTIFICATION();

        static eActionOp_MONITOR get_action_op(){
            return (eActionOp_MONITOR)(ACTION_MONITOR_CLIENT_NO_RESPONSE_NOTIFICATION);
        }
        beerocks::net::sMacAddr& mac();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_MONITOR* m_action_op = nullptr;
        beerocks::net::sMacAddr* m_mac = nullptr;
};

class cACTION_MONITOR_CLIENT_RX_RSSI_MEASUREMENT_START_NOTIFICATION : public BaseClass
{
    public:
        cACTION_MONITOR_CLIENT_RX_RSSI_MEASUREMENT_START_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_MONITOR_CLIENT_RX_RSSI_MEASUREMENT_START_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_MONITOR_CLIENT_RX_RSSI_MEASUREMENT_START_NOTIFICATION();

        static eActionOp_MONITOR get_action_op(){
            return (eActionOp_MONITOR)(ACTION_MONITOR_CLIENT_RX_RSSI_MEASUREMENT_START_NOTIFICATION);
        }
        beerocks::net::sMacAddr& mac();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_MONITOR* m_action_op = nullptr;
        beerocks::net::sMacAddr* m_mac = nullptr;
};

class cACTION_MONITOR_CLIENT_RX_RSSI_MEASUREMENT_CMD_RESPONSE : public BaseClass
{
    public:
        cACTION_MONITOR_CLIENT_RX_RSSI_MEASUREMENT_CMD_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_MONITOR_CLIENT_RX_RSSI_MEASUREMENT_CMD_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_MONITOR_CLIENT_RX_RSSI_MEASUREMENT_CMD_RESPONSE();

        static eActionOp_MONITOR get_action_op(){
            return (eActionOp_MONITOR)(ACTION_MONITOR_CLIENT_RX_RSSI_MEASUREMENT_CMD_RESPONSE);
        }
        beerocks::net::sMacAddr& mac();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_MONITOR* m_action_op = nullptr;
        beerocks::net::sMacAddr* m_mac = nullptr;
};

class cACTION_MONITOR_CLIENT_NO_ACTIVITY_NOTIFICATION : public BaseClass
{
    public:
        cACTION_MONITOR_CLIENT_NO_ACTIVITY_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_MONITOR_CLIENT_NO_ACTIVITY_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_MONITOR_CLIENT_NO_ACTIVITY_NOTIFICATION();

        static eActionOp_MONITOR get_action_op(){
            return (eActionOp_MONITOR)(ACTION_MONITOR_CLIENT_NO_ACTIVITY_NOTIFICATION);
        }
        beerocks::net::sMacAddr& mac();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_MONITOR* m_action_op = nullptr;
        beerocks::net::sMacAddr* m_mac = nullptr;
};

class cACTION_MONITOR_HOSTAP_ACTIVITY_NOTIFICATION : public BaseClass
{
    public:
        cACTION_MONITOR_HOSTAP_ACTIVITY_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_MONITOR_HOSTAP_ACTIVITY_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_MONITOR_HOSTAP_ACTIVITY_NOTIFICATION();

        static eActionOp_MONITOR get_action_op(){
            return (eActionOp_MONITOR)(ACTION_MONITOR_HOSTAP_ACTIVITY_NOTIFICATION);
        }
        sApActivityNotificationParams& params();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_MONITOR* m_action_op = nullptr;
        sApActivityNotificationParams* m_params = nullptr;
};

class cACTION_MONITOR_HOSTAP_STATS_MEASUREMENT_REQUEST : public BaseClass
{
    public:
        cACTION_MONITOR_HOSTAP_STATS_MEASUREMENT_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_MONITOR_HOSTAP_STATS_MEASUREMENT_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_MONITOR_HOSTAP_STATS_MEASUREMENT_REQUEST();

        static eActionOp_MONITOR get_action_op(){
            return (eActionOp_MONITOR)(ACTION_MONITOR_HOSTAP_STATS_MEASUREMENT_REQUEST);
        }
        uint8_t& sync();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_MONITOR* m_action_op = nullptr;
        uint8_t* m_sync = nullptr;
};

class cACTION_MONITOR_HOSTAP_STATUS_CHANGED_NOTIFICATION : public BaseClass
{
    public:
        cACTION_MONITOR_HOSTAP_STATUS_CHANGED_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_MONITOR_HOSTAP_STATUS_CHANGED_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_MONITOR_HOSTAP_STATUS_CHANGED_NOTIFICATION();

        static eActionOp_MONITOR get_action_op(){
            return (eActionOp_MONITOR)(ACTION_MONITOR_HOSTAP_STATUS_CHANGED_NOTIFICATION);
        }
        int8_t& new_tx_state();
        int8_t& new_hostap_enabled_state();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_MONITOR* m_action_op = nullptr;
        int8_t* m_new_tx_state = nullptr;
        int8_t* m_new_hostap_enabled_state = nullptr;
};

class cACTION_MONITOR_HOSTAP_STATS_MEASUREMENT_RESPONSE : public BaseClass
{
    public:
        cACTION_MONITOR_HOSTAP_STATS_MEASUREMENT_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_MONITOR_HOSTAP_STATS_MEASUREMENT_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_MONITOR_HOSTAP_STATS_MEASUREMENT_RESPONSE();

        static eActionOp_MONITOR get_action_op(){
            return (eActionOp_MONITOR)(ACTION_MONITOR_HOSTAP_STATS_MEASUREMENT_RESPONSE);
        }
        sApStatsParams& ap_stats();
        uint8_t& sta_stats_size();
        std::tuple<bool, sStaStatsParams&> sta_stats(size_t idx);
        bool alloc_sta_stats(size_t count = 1);
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_MONITOR* m_action_op = nullptr;
        sApStatsParams* m_ap_stats = nullptr;
        uint8_t* m_sta_stats_size = nullptr;
        sStaStatsParams* m_sta_stats = nullptr;
        size_t m_sta_stats_idx__ = 0;
};

class cACTION_MONITOR_HOSTAP_LOAD_MEASUREMENT_NOTIFICATION : public BaseClass
{
    public:
        cACTION_MONITOR_HOSTAP_LOAD_MEASUREMENT_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_MONITOR_HOSTAP_LOAD_MEASUREMENT_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_MONITOR_HOSTAP_LOAD_MEASUREMENT_NOTIFICATION();

        static eActionOp_MONITOR get_action_op(){
            return (eActionOp_MONITOR)(ACTION_MONITOR_HOSTAP_LOAD_MEASUREMENT_NOTIFICATION);
        }
        sApLoadNotificationParams& params();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_MONITOR* m_action_op = nullptr;
        sApLoadNotificationParams* m_params = nullptr;
};

class cACTION_MONITOR_CLIENT_BEACON_11K_REQUEST : public BaseClass
{
    public:
        cACTION_MONITOR_CLIENT_BEACON_11K_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_MONITOR_CLIENT_BEACON_11K_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_MONITOR_CLIENT_BEACON_11K_REQUEST();

        static eActionOp_MONITOR get_action_op(){
            return (eActionOp_MONITOR)(ACTION_MONITOR_CLIENT_BEACON_11K_REQUEST);
        }
        sBeaconRequest11k& params();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_MONITOR* m_action_op = nullptr;
        sBeaconRequest11k* m_params = nullptr;
};

class cACTION_MONITOR_CLIENT_BEACON_11K_RESPONSE : public BaseClass
{
    public:
        cACTION_MONITOR_CLIENT_BEACON_11K_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_MONITOR_CLIENT_BEACON_11K_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_MONITOR_CLIENT_BEACON_11K_RESPONSE();

        static eActionOp_MONITOR get_action_op(){
            return (eActionOp_MONITOR)(ACTION_MONITOR_CLIENT_BEACON_11K_RESPONSE);
        }
        sBeaconResponse11k& params();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_MONITOR* m_action_op = nullptr;
        sBeaconResponse11k* m_params = nullptr;
};

class cACTION_MONITOR_CLIENT_CHANNEL_LOAD_11K_REQUEST : public BaseClass
{
    public:
        cACTION_MONITOR_CLIENT_CHANNEL_LOAD_11K_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_MONITOR_CLIENT_CHANNEL_LOAD_11K_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_MONITOR_CLIENT_CHANNEL_LOAD_11K_REQUEST();

        static eActionOp_MONITOR get_action_op(){
            return (eActionOp_MONITOR)(ACTION_MONITOR_CLIENT_CHANNEL_LOAD_11K_REQUEST);
        }
        sStaChannelLoadRequest11k& params();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_MONITOR* m_action_op = nullptr;
        sStaChannelLoadRequest11k* m_params = nullptr;
};

class cACTION_MONITOR_CLIENT_CHANNEL_LOAD_11K_RESPONSE : public BaseClass
{
    public:
        cACTION_MONITOR_CLIENT_CHANNEL_LOAD_11K_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_MONITOR_CLIENT_CHANNEL_LOAD_11K_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_MONITOR_CLIENT_CHANNEL_LOAD_11K_RESPONSE();

        static eActionOp_MONITOR get_action_op(){
            return (eActionOp_MONITOR)(ACTION_MONITOR_CLIENT_CHANNEL_LOAD_11K_RESPONSE);
        }
        sStaChannelLoadResponse11k& params();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_MONITOR* m_action_op = nullptr;
        sStaChannelLoadResponse11k* m_params = nullptr;
};

class cACTION_MONITOR_CLIENT_STATISTICS_11K_REQUEST : public BaseClass
{
    public:
        cACTION_MONITOR_CLIENT_STATISTICS_11K_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_MONITOR_CLIENT_STATISTICS_11K_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_MONITOR_CLIENT_STATISTICS_11K_REQUEST();

        static eActionOp_MONITOR get_action_op(){
            return (eActionOp_MONITOR)(ACTION_MONITOR_CLIENT_STATISTICS_11K_REQUEST);
        }
        sStatisticsRequest11k& params();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_MONITOR* m_action_op = nullptr;
        sStatisticsRequest11k* m_params = nullptr;
};

class cACTION_MONITOR_CLIENT_STATISTICS_11K_RESPONSE : public BaseClass
{
    public:
        cACTION_MONITOR_CLIENT_STATISTICS_11K_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_MONITOR_CLIENT_STATISTICS_11K_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_MONITOR_CLIENT_STATISTICS_11K_RESPONSE();

        static eActionOp_MONITOR get_action_op(){
            return (eActionOp_MONITOR)(ACTION_MONITOR_CLIENT_STATISTICS_11K_RESPONSE);
        }
        sStatisticsResponse11k& params();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_MONITOR* m_action_op = nullptr;
        sStatisticsResponse11k* m_params = nullptr;
};

class cACTION_MONITOR_CLIENT_LINK_MEASUREMENT_11K_REQUEST : public BaseClass
{
    public:
        cACTION_MONITOR_CLIENT_LINK_MEASUREMENT_11K_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_MONITOR_CLIENT_LINK_MEASUREMENT_11K_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_MONITOR_CLIENT_LINK_MEASUREMENT_11K_REQUEST();

        static eActionOp_MONITOR get_action_op(){
            return (eActionOp_MONITOR)(ACTION_MONITOR_CLIENT_LINK_MEASUREMENT_11K_REQUEST);
        }
        beerocks::net::sMacAddr& mac();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_MONITOR* m_action_op = nullptr;
        beerocks::net::sMacAddr* m_mac = nullptr;
};

class cACTION_MONITOR_CLIENT_LINK_MEASUREMENTS_11K_RESPONSE : public BaseClass
{
    public:
        cACTION_MONITOR_CLIENT_LINK_MEASUREMENTS_11K_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_MONITOR_CLIENT_LINK_MEASUREMENTS_11K_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_MONITOR_CLIENT_LINK_MEASUREMENTS_11K_RESPONSE();

        static eActionOp_MONITOR get_action_op(){
            return (eActionOp_MONITOR)(ACTION_MONITOR_CLIENT_LINK_MEASUREMENTS_11K_RESPONSE);
        }
        sLinkMeasurementsResponse11k& params();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_MONITOR* m_action_op = nullptr;
        sLinkMeasurementsResponse11k* m_params = nullptr;
};

class cACTION_MONITOR_STEERING_CLIENT_SET_GROUP_REQUEST : public BaseClass
{
    public:
        cACTION_MONITOR_STEERING_CLIENT_SET_GROUP_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_MONITOR_STEERING_CLIENT_SET_GROUP_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_MONITOR_STEERING_CLIENT_SET_GROUP_REQUEST();

        static eActionOp_MONITOR get_action_op(){
            return (eActionOp_MONITOR)(ACTION_MONITOR_STEERING_CLIENT_SET_GROUP_REQUEST);
        }
        sSteeringSetGroupRequest& params();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_MONITOR* m_action_op = nullptr;
        sSteeringSetGroupRequest* m_params = nullptr;
};

class cACTION_MONITOR_STEERING_CLIENT_SET_GROUP_RESPONSE : public BaseClass
{
    public:
        cACTION_MONITOR_STEERING_CLIENT_SET_GROUP_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_MONITOR_STEERING_CLIENT_SET_GROUP_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_MONITOR_STEERING_CLIENT_SET_GROUP_RESPONSE();

        static eActionOp_MONITOR get_action_op(){
            return (eActionOp_MONITOR)(ACTION_MONITOR_STEERING_CLIENT_SET_GROUP_RESPONSE);
        }
        sSteeringSetGroupResponse& params();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_MONITOR* m_action_op = nullptr;
        sSteeringSetGroupResponse* m_params = nullptr;
};

class cACTION_MONITOR_STEERING_CLIENT_SET_REQUEST : public BaseClass
{
    public:
        cACTION_MONITOR_STEERING_CLIENT_SET_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_MONITOR_STEERING_CLIENT_SET_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_MONITOR_STEERING_CLIENT_SET_REQUEST();

        static eActionOp_MONITOR get_action_op(){
            return (eActionOp_MONITOR)(ACTION_MONITOR_STEERING_CLIENT_SET_REQUEST);
        }
        sSteeringClientSetRequest& params();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_MONITOR* m_action_op = nullptr;
        sSteeringClientSetRequest* m_params = nullptr;
};

class cACTION_MONITOR_STEERING_CLIENT_SET_RESPONSE : public BaseClass
{
    public:
        cACTION_MONITOR_STEERING_CLIENT_SET_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_MONITOR_STEERING_CLIENT_SET_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_MONITOR_STEERING_CLIENT_SET_RESPONSE();

        static eActionOp_MONITOR get_action_op(){
            return (eActionOp_MONITOR)(ACTION_MONITOR_STEERING_CLIENT_SET_RESPONSE);
        }
        sSteeringClientSetResponse& params();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_MONITOR* m_action_op = nullptr;
        sSteeringClientSetResponse* m_params = nullptr;
};

class cACTION_MONITOR_STEERING_EVENT_CLIENT_ACTIVITY_NOTIFICATION : public BaseClass
{
    public:
        cACTION_MONITOR_STEERING_EVENT_CLIENT_ACTIVITY_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_MONITOR_STEERING_EVENT_CLIENT_ACTIVITY_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_MONITOR_STEERING_EVENT_CLIENT_ACTIVITY_NOTIFICATION();

        static eActionOp_MONITOR get_action_op(){
            return (eActionOp_MONITOR)(ACTION_MONITOR_STEERING_EVENT_CLIENT_ACTIVITY_NOTIFICATION);
        }
        sSteeringEvActivity& params();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_MONITOR* m_action_op = nullptr;
        sSteeringEvActivity* m_params = nullptr;
};

class cACTION_MONITOR_STEERING_EVENT_SNR_XING_NOTIFICATION : public BaseClass
{
    public:
        cACTION_MONITOR_STEERING_EVENT_SNR_XING_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_MONITOR_STEERING_EVENT_SNR_XING_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_MONITOR_STEERING_EVENT_SNR_XING_NOTIFICATION();

        static eActionOp_MONITOR get_action_op(){
            return (eActionOp_MONITOR)(ACTION_MONITOR_STEERING_EVENT_SNR_XING_NOTIFICATION);
        }
        sSteeringEvSnrXing& params();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_MONITOR* m_action_op = nullptr;
        sSteeringEvSnrXing* m_params = nullptr;
};

}; // close namespace: beerocks_message

#endif //_BEEROCKS/TLVF_BEEROCKS_MESSAGE_MONITOR_H_
