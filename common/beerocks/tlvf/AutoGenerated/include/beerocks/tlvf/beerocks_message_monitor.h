///////////////////////////////////////
// AUTO GENERATED FILE - DO NOT EDIT //
///////////////////////////////////////

/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _BEEROCKS_TLVF_BEEROCKS_MESSAGE_MONITOR_H_
#define _BEEROCKS_TLVF_BEEROCKS_MESSAGE_MONITOR_H_

#include <cstddef>
#include <stdint.h>
#include <tlvf/swap.h>
#include <string.h>
#include <memory>
#include <tlvf/BaseClass.h>
#include <tlvf/ClassList.h>
#include <tuple>
#include "beerocks/tlvf/beerocks_message_common.h"

namespace beerocks_message {


class cACTION_MONITOR_HOSTAP_AP_DISABLED_NOTIFICATION : public BaseClass
{
    public:
        cACTION_MONITOR_HOSTAP_AP_DISABLED_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_MONITOR_HOSTAP_AP_DISABLED_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_MONITOR_HOSTAP_AP_DISABLED_NOTIFICATION();

        static eActionOp_MONITOR get_action_op(){
            return (eActionOp_MONITOR)(ACTION_MONITOR_HOSTAP_AP_DISABLED_NOTIFICATION);
        }
        int8_t& vap_id();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_MONITOR* m_action_op = nullptr;
        int8_t* m_vap_id = nullptr;
};

class cACTION_MONITOR_JOINED_NOTIFICATION : public BaseClass
{
    public:
        cACTION_MONITOR_JOINED_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_MONITOR_JOINED_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_MONITOR_JOINED_NOTIFICATION();

        static eActionOp_MONITOR get_action_op(){
            return (eActionOp_MONITOR)(ACTION_MONITOR_JOINED_NOTIFICATION);
        }
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_MONITOR* m_action_op = nullptr;
};

class cACTION_MONITOR_SON_CONFIG_UPDATE : public BaseClass
{
    public:
        cACTION_MONITOR_SON_CONFIG_UPDATE(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_MONITOR_SON_CONFIG_UPDATE(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_MONITOR_SON_CONFIG_UPDATE();

        static eActionOp_MONITOR get_action_op(){
            return (eActionOp_MONITOR)(ACTION_MONITOR_SON_CONFIG_UPDATE);
        }
        sSonConfig& config();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_MONITOR* m_action_op = nullptr;
        sSonConfig* m_config = nullptr;
};

class cACTION_MONITOR_CHANGE_MODULE_LOGGING_LEVEL : public BaseClass
{
    public:
        cACTION_MONITOR_CHANGE_MODULE_LOGGING_LEVEL(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_MONITOR_CHANGE_MODULE_LOGGING_LEVEL(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_MONITOR_CHANGE_MODULE_LOGGING_LEVEL();

        static eActionOp_MONITOR get_action_op(){
            return (eActionOp_MONITOR)(ACTION_MONITOR_CHANGE_MODULE_LOGGING_LEVEL);
        }
        sLoggingLevelChange& params();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_MONITOR* m_action_op = nullptr;
        sLoggingLevelChange* m_params = nullptr;
};

class cACTION_MONITOR_ERROR_NOTIFICATION : public BaseClass
{
    public:
        cACTION_MONITOR_ERROR_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_MONITOR_ERROR_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_MONITOR_ERROR_NOTIFICATION();

        static eActionOp_MONITOR get_action_op(){
            return (eActionOp_MONITOR)(ACTION_MONITOR_ERROR_NOTIFICATION);
        }
        uint32_t& error_code();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_MONITOR* m_action_op = nullptr;
        uint32_t* m_error_code = nullptr;
};

class cACTION_MONITOR_ERROR_NOTIFICATION_ACK : public BaseClass
{
    public:
        cACTION_MONITOR_ERROR_NOTIFICATION_ACK(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_MONITOR_ERROR_NOTIFICATION_ACK(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_MONITOR_ERROR_NOTIFICATION_ACK();

        static eActionOp_MONITOR get_action_op(){
            return (eActionOp_MONITOR)(ACTION_MONITOR_ERROR_NOTIFICATION_ACK);
        }
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_MONITOR* m_action_op = nullptr;
};

class cACTION_MONITOR_HEARTBEAT_NOTIFICATION : public BaseClass
{
    public:
        cACTION_MONITOR_HEARTBEAT_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_MONITOR_HEARTBEAT_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_MONITOR_HEARTBEAT_NOTIFICATION();

        static eActionOp_MONITOR get_action_op(){
            return (eActionOp_MONITOR)(ACTION_MONITOR_HEARTBEAT_NOTIFICATION);
        }
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_MONITOR* m_action_op = nullptr;
};

class cACTION_MONITOR_CLIENT_START_MONITORING_REQUEST : public BaseClass
{
    public:
        cACTION_MONITOR_CLIENT_START_MONITORING_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_MONITOR_CLIENT_START_MONITORING_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_MONITOR_CLIENT_START_MONITORING_REQUEST();

        static eActionOp_MONITOR get_action_op(){
            return (eActionOp_MONITOR)(ACTION_MONITOR_CLIENT_START_MONITORING_REQUEST);
        }
        sClientMonitoringParams& params();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_MONITOR* m_action_op = nullptr;
        sClientMonitoringParams* m_params = nullptr;
};

class cACTION_MONITOR_CLIENT_START_MONITORING_RESPONSE : public BaseClass
{
    public:
        cACTION_MONITOR_CLIENT_START_MONITORING_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_MONITOR_CLIENT_START_MONITORING_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_MONITOR_CLIENT_START_MONITORING_RESPONSE();

        static eActionOp_MONITOR get_action_op(){
            return (eActionOp_MONITOR)(ACTION_MONITOR_CLIENT_START_MONITORING_RESPONSE);
        }
        uint8_t& success();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_MONITOR* m_action_op = nullptr;
        uint8_t* m_success = nullptr;
};

class cACTION_MONITOR_CLIENT_RX_RSSI_MEASUREMENT_REQUEST : public BaseClass
{
    public:
        cACTION_MONITOR_CLIENT_RX_RSSI_MEASUREMENT_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_MONITOR_CLIENT_RX_RSSI_MEASUREMENT_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_MONITOR_CLIENT_RX_RSSI_MEASUREMENT_REQUEST();

        static eActionOp_MONITOR get_action_op(){
            return (eActionOp_MONITOR)(ACTION_MONITOR_CLIENT_RX_RSSI_MEASUREMENT_REQUEST);
        }
        sNodeRssiMeasurementRequest& params();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_MONITOR* m_action_op = nullptr;
        sNodeRssiMeasurementRequest* m_params = nullptr;
};

class cACTION_MONITOR_CLIENT_DISCONNECT_REQUEST : public BaseClass
{
    public:
        cACTION_MONITOR_CLIENT_DISCONNECT_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_MONITOR_CLIENT_DISCONNECT_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_MONITOR_CLIENT_DISCONNECT_REQUEST();

        static eActionOp_MONITOR get_action_op(){
            return (eActionOp_MONITOR)(ACTION_MONITOR_CLIENT_DISCONNECT_REQUEST);
        }
        sMacAddr& mac();
        beerocks::net::sIpv4Addr& ipv4();
        uint8_t& channel();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_MONITOR* m_action_op = nullptr;
        sMacAddr* m_mac = nullptr;
        beerocks::net::sIpv4Addr* m_ipv4 = nullptr;
        uint8_t* m_channel = nullptr;
};

class cACTION_MONITOR_CLIENT_RX_RSSI_MEASUREMENT_NOTIFICATION : public BaseClass
{
    public:
        cACTION_MONITOR_CLIENT_RX_RSSI_MEASUREMENT_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_MONITOR_CLIENT_RX_RSSI_MEASUREMENT_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_MONITOR_CLIENT_RX_RSSI_MEASUREMENT_NOTIFICATION();

        static eActionOp_MONITOR get_action_op(){
            return (eActionOp_MONITOR)(ACTION_MONITOR_CLIENT_RX_RSSI_MEASUREMENT_NOTIFICATION);
        }
        sNodeRssiMeasurement& params();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_MONITOR* m_action_op = nullptr;
        sNodeRssiMeasurement* m_params = nullptr;
};

class cACTION_MONITOR_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE : public BaseClass
{
    public:
        cACTION_MONITOR_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_MONITOR_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_MONITOR_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE();

        static eActionOp_MONITOR get_action_op(){
            return (eActionOp_MONITOR)(ACTION_MONITOR_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE);
        }
        sNodeRssiMeasurement& params();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_MONITOR* m_action_op = nullptr;
        sNodeRssiMeasurement* m_params = nullptr;
};

class cACTION_MONITOR_CLIENT_NO_RESPONSE_NOTIFICATION : public BaseClass
{
    public:
        cACTION_MONITOR_CLIENT_NO_RESPONSE_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_MONITOR_CLIENT_NO_RESPONSE_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_MONITOR_CLIENT_NO_RESPONSE_NOTIFICATION();

        static eActionOp_MONITOR get_action_op(){
            return (eActionOp_MONITOR)(ACTION_MONITOR_CLIENT_NO_RESPONSE_NOTIFICATION);
        }
        sMacAddr& mac();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_MONITOR* m_action_op = nullptr;
        sMacAddr* m_mac = nullptr;
};

class cACTION_MONITOR_CLIENT_RX_RSSI_MEASUREMENT_START_NOTIFICATION : public BaseClass
{
    public:
        cACTION_MONITOR_CLIENT_RX_RSSI_MEASUREMENT_START_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_MONITOR_CLIENT_RX_RSSI_MEASUREMENT_START_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_MONITOR_CLIENT_RX_RSSI_MEASUREMENT_START_NOTIFICATION();

        static eActionOp_MONITOR get_action_op(){
            return (eActionOp_MONITOR)(ACTION_MONITOR_CLIENT_RX_RSSI_MEASUREMENT_START_NOTIFICATION);
        }
        sMacAddr& mac();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_MONITOR* m_action_op = nullptr;
        sMacAddr* m_mac = nullptr;
};

class cACTION_MONITOR_CLIENT_RX_RSSI_MEASUREMENT_CMD_RESPONSE : public BaseClass
{
    public:
        cACTION_MONITOR_CLIENT_RX_RSSI_MEASUREMENT_CMD_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_MONITOR_CLIENT_RX_RSSI_MEASUREMENT_CMD_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_MONITOR_CLIENT_RX_RSSI_MEASUREMENT_CMD_RESPONSE();

        static eActionOp_MONITOR get_action_op(){
            return (eActionOp_MONITOR)(ACTION_MONITOR_CLIENT_RX_RSSI_MEASUREMENT_CMD_RESPONSE);
        }
        sMacAddr& mac();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_MONITOR* m_action_op = nullptr;
        sMacAddr* m_mac = nullptr;
};

class cACTION_MONITOR_CLIENT_NO_ACTIVITY_NOTIFICATION : public BaseClass
{
    public:
        cACTION_MONITOR_CLIENT_NO_ACTIVITY_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_MONITOR_CLIENT_NO_ACTIVITY_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_MONITOR_CLIENT_NO_ACTIVITY_NOTIFICATION();

        static eActionOp_MONITOR get_action_op(){
            return (eActionOp_MONITOR)(ACTION_MONITOR_CLIENT_NO_ACTIVITY_NOTIFICATION);
        }
        sMacAddr& mac();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_MONITOR* m_action_op = nullptr;
        sMacAddr* m_mac = nullptr;
};

class cACTION_MONITOR_HOSTAP_ACTIVITY_NOTIFICATION : public BaseClass
{
    public:
        cACTION_MONITOR_HOSTAP_ACTIVITY_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_MONITOR_HOSTAP_ACTIVITY_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_MONITOR_HOSTAP_ACTIVITY_NOTIFICATION();

        static eActionOp_MONITOR get_action_op(){
            return (eActionOp_MONITOR)(ACTION_MONITOR_HOSTAP_ACTIVITY_NOTIFICATION);
        }
        sApActivityNotificationParams& params();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_MONITOR* m_action_op = nullptr;
        sApActivityNotificationParams* m_params = nullptr;
};

class cACTION_MONITOR_CLIENT_ASSOCIATED_STA_LINK_METRIC_REQUEST : public BaseClass
{
    public:
        cACTION_MONITOR_CLIENT_ASSOCIATED_STA_LINK_METRIC_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_MONITOR_CLIENT_ASSOCIATED_STA_LINK_METRIC_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_MONITOR_CLIENT_ASSOCIATED_STA_LINK_METRIC_REQUEST();

        static eActionOp_MONITOR get_action_op(){
            return (eActionOp_MONITOR)(ACTION_MONITOR_CLIENT_ASSOCIATED_STA_LINK_METRIC_REQUEST);
        }
        uint8_t& sync();
        sMacAddr& sta_mac();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_MONITOR* m_action_op = nullptr;
        uint8_t* m_sync = nullptr;
        sMacAddr* m_sta_mac = nullptr;
};

class cACTION_MONITOR_CLIENT_ASSOCIATED_STA_LINK_METRIC_RESPONSE : public BaseClass
{
    public:
        cACTION_MONITOR_CLIENT_ASSOCIATED_STA_LINK_METRIC_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_MONITOR_CLIENT_ASSOCIATED_STA_LINK_METRIC_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_MONITOR_CLIENT_ASSOCIATED_STA_LINK_METRIC_RESPONSE();

        static eActionOp_MONITOR get_action_op(){
            return (eActionOp_MONITOR)(ACTION_MONITOR_CLIENT_ASSOCIATED_STA_LINK_METRIC_RESPONSE);
        }
        const uint16_t& length();
        sMacAddr& sta_mac();
        uint8_t& bssid_info_list_length();
        std::tuple<bool, sBssidInfo&> bssid_info_list(size_t idx);
        bool alloc_bssid_info_list(size_t count = 1);
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_MONITOR* m_action_op = nullptr;
        uint16_t* m_length = nullptr;
        sMacAddr* m_sta_mac = nullptr;
        uint8_t* m_bssid_info_list_length = nullptr;
        sBssidInfo* m_bssid_info_list = nullptr;
        size_t m_bssid_info_list_idx__ = 0;
        int m_lock_order_counter__ = 0;
};

class cACTION_MONITOR_HOSTAP_STATS_MEASUREMENT_REQUEST : public BaseClass
{
    public:
        cACTION_MONITOR_HOSTAP_STATS_MEASUREMENT_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_MONITOR_HOSTAP_STATS_MEASUREMENT_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_MONITOR_HOSTAP_STATS_MEASUREMENT_REQUEST();

        static eActionOp_MONITOR get_action_op(){
            return (eActionOp_MONITOR)(ACTION_MONITOR_HOSTAP_STATS_MEASUREMENT_REQUEST);
        }
        uint8_t& sync();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_MONITOR* m_action_op = nullptr;
        uint8_t* m_sync = nullptr;
};

class cACTION_MONITOR_HOSTAP_STATS_MEASUREMENT_RESPONSE : public BaseClass
{
    public:
        cACTION_MONITOR_HOSTAP_STATS_MEASUREMENT_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_MONITOR_HOSTAP_STATS_MEASUREMENT_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_MONITOR_HOSTAP_STATS_MEASUREMENT_RESPONSE();

        static eActionOp_MONITOR get_action_op(){
            return (eActionOp_MONITOR)(ACTION_MONITOR_HOSTAP_STATS_MEASUREMENT_RESPONSE);
        }
        uint8_t& ap_stats_size();
        std::tuple<bool, sApStatsParams&> ap_stats(size_t idx);
        bool alloc_ap_stats(size_t count = 1);
        uint8_t& sta_stats_size();
        std::tuple<bool, sStaStatsParams&> sta_stats(size_t idx);
        bool alloc_sta_stats(size_t count = 1);
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_MONITOR* m_action_op = nullptr;
        uint8_t* m_ap_stats_size = nullptr;
        sApStatsParams* m_ap_stats = nullptr;
        size_t m_ap_stats_idx__ = 0;
        int m_lock_order_counter__ = 0;
        uint8_t* m_sta_stats_size = nullptr;
        sStaStatsParams* m_sta_stats = nullptr;
        size_t m_sta_stats_idx__ = 0;
};

class cACTION_MONITOR_HOSTAP_LOAD_MEASUREMENT_NOTIFICATION : public BaseClass
{
    public:
        cACTION_MONITOR_HOSTAP_LOAD_MEASUREMENT_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_MONITOR_HOSTAP_LOAD_MEASUREMENT_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_MONITOR_HOSTAP_LOAD_MEASUREMENT_NOTIFICATION();

        static eActionOp_MONITOR get_action_op(){
            return (eActionOp_MONITOR)(ACTION_MONITOR_HOSTAP_LOAD_MEASUREMENT_NOTIFICATION);
        }
        sApLoadNotificationParams& params();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_MONITOR* m_action_op = nullptr;
        sApLoadNotificationParams* m_params = nullptr;
};

class cACTION_MONITOR_CLIENT_BEACON_11K_REQUEST : public BaseClass
{
    public:
        cACTION_MONITOR_CLIENT_BEACON_11K_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_MONITOR_CLIENT_BEACON_11K_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_MONITOR_CLIENT_BEACON_11K_REQUEST();

        static eActionOp_MONITOR get_action_op(){
            return (eActionOp_MONITOR)(ACTION_MONITOR_CLIENT_BEACON_11K_REQUEST);
        }
        sBeaconRequest11k& params();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_MONITOR* m_action_op = nullptr;
        sBeaconRequest11k* m_params = nullptr;
};

class cACTION_MONITOR_CLIENT_BEACON_11K_RESPONSE : public BaseClass
{
    public:
        cACTION_MONITOR_CLIENT_BEACON_11K_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_MONITOR_CLIENT_BEACON_11K_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_MONITOR_CLIENT_BEACON_11K_RESPONSE();

        static eActionOp_MONITOR get_action_op(){
            return (eActionOp_MONITOR)(ACTION_MONITOR_CLIENT_BEACON_11K_RESPONSE);
        }
        sBeaconResponse11k& params();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_MONITOR* m_action_op = nullptr;
        sBeaconResponse11k* m_params = nullptr;
};

class cACTION_MONITOR_CLIENT_CHANNEL_LOAD_11K_REQUEST : public BaseClass
{
    public:
        cACTION_MONITOR_CLIENT_CHANNEL_LOAD_11K_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_MONITOR_CLIENT_CHANNEL_LOAD_11K_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_MONITOR_CLIENT_CHANNEL_LOAD_11K_REQUEST();

        static eActionOp_MONITOR get_action_op(){
            return (eActionOp_MONITOR)(ACTION_MONITOR_CLIENT_CHANNEL_LOAD_11K_REQUEST);
        }
        sStaChannelLoadRequest11k& params();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_MONITOR* m_action_op = nullptr;
        sStaChannelLoadRequest11k* m_params = nullptr;
};

class cACTION_MONITOR_CLIENT_CHANNEL_LOAD_11K_RESPONSE : public BaseClass
{
    public:
        cACTION_MONITOR_CLIENT_CHANNEL_LOAD_11K_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_MONITOR_CLIENT_CHANNEL_LOAD_11K_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_MONITOR_CLIENT_CHANNEL_LOAD_11K_RESPONSE();

        static eActionOp_MONITOR get_action_op(){
            return (eActionOp_MONITOR)(ACTION_MONITOR_CLIENT_CHANNEL_LOAD_11K_RESPONSE);
        }
        sStaChannelLoadResponse11k& params();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_MONITOR* m_action_op = nullptr;
        sStaChannelLoadResponse11k* m_params = nullptr;
};

class cACTION_MONITOR_CLIENT_NEW_IP_ADDRESS_NOTIFICATION : public BaseClass
{
    public:
        cACTION_MONITOR_CLIENT_NEW_IP_ADDRESS_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_MONITOR_CLIENT_NEW_IP_ADDRESS_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_MONITOR_CLIENT_NEW_IP_ADDRESS_NOTIFICATION();

        static eActionOp_MONITOR get_action_op(){
            return (eActionOp_MONITOR)(ACTION_MONITOR_CLIENT_NEW_IP_ADDRESS_NOTIFICATION);
        }
        sMacAddr& mac();
        beerocks::net::sIpv4Addr& ipv4();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_MONITOR* m_action_op = nullptr;
        sMacAddr* m_mac = nullptr;
        beerocks::net::sIpv4Addr* m_ipv4 = nullptr;
};

class cACTION_MONITOR_STEERING_CLIENT_SET_GROUP_REQUEST : public BaseClass
{
    public:
        cACTION_MONITOR_STEERING_CLIENT_SET_GROUP_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_MONITOR_STEERING_CLIENT_SET_GROUP_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_MONITOR_STEERING_CLIENT_SET_GROUP_REQUEST();

        static eActionOp_MONITOR get_action_op(){
            return (eActionOp_MONITOR)(ACTION_MONITOR_STEERING_CLIENT_SET_GROUP_REQUEST);
        }
        sSteeringSetGroupRequest& params();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_MONITOR* m_action_op = nullptr;
        sSteeringSetGroupRequest* m_params = nullptr;
};

class cACTION_MONITOR_STEERING_CLIENT_SET_GROUP_RESPONSE : public BaseClass
{
    public:
        cACTION_MONITOR_STEERING_CLIENT_SET_GROUP_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_MONITOR_STEERING_CLIENT_SET_GROUP_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_MONITOR_STEERING_CLIENT_SET_GROUP_RESPONSE();

        static eActionOp_MONITOR get_action_op(){
            return (eActionOp_MONITOR)(ACTION_MONITOR_STEERING_CLIENT_SET_GROUP_RESPONSE);
        }
        sSteeringSetGroupResponse& params();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_MONITOR* m_action_op = nullptr;
        sSteeringSetGroupResponse* m_params = nullptr;
};

class cACTION_MONITOR_STEERING_CLIENT_SET_REQUEST : public BaseClass
{
    public:
        cACTION_MONITOR_STEERING_CLIENT_SET_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_MONITOR_STEERING_CLIENT_SET_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_MONITOR_STEERING_CLIENT_SET_REQUEST();

        static eActionOp_MONITOR get_action_op(){
            return (eActionOp_MONITOR)(ACTION_MONITOR_STEERING_CLIENT_SET_REQUEST);
        }
        sSteeringClientSetRequest& params();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_MONITOR* m_action_op = nullptr;
        sSteeringClientSetRequest* m_params = nullptr;
};

class cACTION_MONITOR_STEERING_CLIENT_SET_RESPONSE : public BaseClass
{
    public:
        cACTION_MONITOR_STEERING_CLIENT_SET_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_MONITOR_STEERING_CLIENT_SET_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_MONITOR_STEERING_CLIENT_SET_RESPONSE();

        static eActionOp_MONITOR get_action_op(){
            return (eActionOp_MONITOR)(ACTION_MONITOR_STEERING_CLIENT_SET_RESPONSE);
        }
        sSteeringClientSetResponse& params();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_MONITOR* m_action_op = nullptr;
        sSteeringClientSetResponse* m_params = nullptr;
};

class cACTION_MONITOR_STEERING_EVENT_CLIENT_ACTIVITY_NOTIFICATION : public BaseClass
{
    public:
        cACTION_MONITOR_STEERING_EVENT_CLIENT_ACTIVITY_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_MONITOR_STEERING_EVENT_CLIENT_ACTIVITY_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_MONITOR_STEERING_EVENT_CLIENT_ACTIVITY_NOTIFICATION();

        static eActionOp_MONITOR get_action_op(){
            return (eActionOp_MONITOR)(ACTION_MONITOR_STEERING_EVENT_CLIENT_ACTIVITY_NOTIFICATION);
        }
        sSteeringEvActivity& params();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_MONITOR* m_action_op = nullptr;
        sSteeringEvActivity* m_params = nullptr;
};

class cACTION_MONITOR_STEERING_EVENT_SNR_XING_NOTIFICATION : public BaseClass
{
    public:
        cACTION_MONITOR_STEERING_EVENT_SNR_XING_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_MONITOR_STEERING_EVENT_SNR_XING_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_MONITOR_STEERING_EVENT_SNR_XING_NOTIFICATION();

        static eActionOp_MONITOR get_action_op(){
            return (eActionOp_MONITOR)(ACTION_MONITOR_STEERING_EVENT_SNR_XING_NOTIFICATION);
        }
        sSteeringEvSnrXing& params();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_MONITOR* m_action_op = nullptr;
        sSteeringEvSnrXing* m_params = nullptr;
};

class cACTION_MONITOR_CHANNEL_SCAN_TRIGGER_SCAN_REQUEST : public BaseClass
{
    public:
        cACTION_MONITOR_CHANNEL_SCAN_TRIGGER_SCAN_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_MONITOR_CHANNEL_SCAN_TRIGGER_SCAN_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_MONITOR_CHANNEL_SCAN_TRIGGER_SCAN_REQUEST();

        static eActionOp_MONITOR get_action_op(){
            return (eActionOp_MONITOR)(ACTION_MONITOR_CHANNEL_SCAN_TRIGGER_SCAN_REQUEST);
        }
        sTriggerChannelScanParams& scan_params();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_MONITOR* m_action_op = nullptr;
        sTriggerChannelScanParams* m_scan_params = nullptr;
};

class cACTION_MONITOR_CHANNEL_SCAN_TRIGGER_SCAN_RESPONSE : public BaseClass
{
    public:
        cACTION_MONITOR_CHANNEL_SCAN_TRIGGER_SCAN_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_MONITOR_CHANNEL_SCAN_TRIGGER_SCAN_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_MONITOR_CHANNEL_SCAN_TRIGGER_SCAN_RESPONSE();

        static eActionOp_MONITOR get_action_op(){
            return (eActionOp_MONITOR)(ACTION_MONITOR_CHANNEL_SCAN_TRIGGER_SCAN_RESPONSE);
        }
        uint8_t& success();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_MONITOR* m_action_op = nullptr;
        uint8_t* m_success = nullptr;
};

class cACTION_MONITOR_CHANNEL_SCAN_DUMP_RESULTS_REQUEST : public BaseClass
{
    public:
        cACTION_MONITOR_CHANNEL_SCAN_DUMP_RESULTS_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_MONITOR_CHANNEL_SCAN_DUMP_RESULTS_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_MONITOR_CHANNEL_SCAN_DUMP_RESULTS_REQUEST();

        static eActionOp_MONITOR get_action_op(){
            return (eActionOp_MONITOR)(ACTION_MONITOR_CHANNEL_SCAN_DUMP_RESULTS_REQUEST);
        }
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_MONITOR* m_action_op = nullptr;
};

class cACTION_MONITOR_CHANNEL_SCAN_DUMP_RESULTS_RESPONSE : public BaseClass
{
    public:
        cACTION_MONITOR_CHANNEL_SCAN_DUMP_RESULTS_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_MONITOR_CHANNEL_SCAN_DUMP_RESULTS_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_MONITOR_CHANNEL_SCAN_DUMP_RESULTS_RESPONSE();

        static eActionOp_MONITOR get_action_op(){
            return (eActionOp_MONITOR)(ACTION_MONITOR_CHANNEL_SCAN_DUMP_RESULTS_RESPONSE);
        }
        uint8_t& success();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_MONITOR* m_action_op = nullptr;
        uint8_t* m_success = nullptr;
};

class cACTION_MONITOR_CHANNEL_SCAN_TRIGGERED_NOTIFICATION : public BaseClass
{
    public:
        cACTION_MONITOR_CHANNEL_SCAN_TRIGGERED_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_MONITOR_CHANNEL_SCAN_TRIGGERED_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_MONITOR_CHANNEL_SCAN_TRIGGERED_NOTIFICATION();

        static eActionOp_MONITOR get_action_op(){
            return (eActionOp_MONITOR)(ACTION_MONITOR_CHANNEL_SCAN_TRIGGERED_NOTIFICATION);
        }
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_MONITOR* m_action_op = nullptr;
};

class cACTION_MONITOR_CHANNEL_SCAN_RESULTS_NOTIFICATION : public BaseClass
{
    public:
        cACTION_MONITOR_CHANNEL_SCAN_RESULTS_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_MONITOR_CHANNEL_SCAN_RESULTS_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_MONITOR_CHANNEL_SCAN_RESULTS_NOTIFICATION();

        static eActionOp_MONITOR get_action_op(){
            return (eActionOp_MONITOR)(ACTION_MONITOR_CHANNEL_SCAN_RESULTS_NOTIFICATION);
        }
        sChannelScanResults& scan_results();
        //1 - notification contains a result dump, 0 - notification that results are ready
        uint8_t& is_dump();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_MONITOR* m_action_op = nullptr;
        sChannelScanResults* m_scan_results = nullptr;
        uint8_t* m_is_dump = nullptr;
};

class cACTION_MONITOR_CHANNEL_SCAN_ABORT_NOTIFICATION : public BaseClass
{
    public:
        cACTION_MONITOR_CHANNEL_SCAN_ABORT_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_MONITOR_CHANNEL_SCAN_ABORT_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_MONITOR_CHANNEL_SCAN_ABORT_NOTIFICATION();

        static eActionOp_MONITOR get_action_op(){
            return (eActionOp_MONITOR)(ACTION_MONITOR_CHANNEL_SCAN_ABORT_NOTIFICATION);
        }
        uint8_t& reason();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_MONITOR* m_action_op = nullptr;
        uint8_t* m_reason = nullptr;
};

class cACTION_MONITOR_CHANNEL_SCAN_FINISHED_NOTIFICATION : public BaseClass
{
    public:
        cACTION_MONITOR_CHANNEL_SCAN_FINISHED_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_MONITOR_CHANNEL_SCAN_FINISHED_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_MONITOR_CHANNEL_SCAN_FINISHED_NOTIFICATION();

        static eActionOp_MONITOR get_action_op(){
            return (eActionOp_MONITOR)(ACTION_MONITOR_CHANNEL_SCAN_FINISHED_NOTIFICATION);
        }
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_MONITOR* m_action_op = nullptr;
};

}; // close namespace: beerocks_message

#endif //_BEEROCKS/TLVF_BEEROCKS_MESSAGE_MONITOR_H_
