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

#ifndef _BEEROCKS_TLVF_BEEROCKS_MESSAGE_APMANAGER_H_
#define _BEEROCKS_TLVF_BEEROCKS_MESSAGE_APMANAGER_H_

#include <cstddef>
#include <stdint.h>
#include <tlvf/swap.h>
#include <string.h>
#include <memory>
#include <tlvf/BaseClass.h>
#include <tuple>
#include "beerocks/tlvf/beerocks_message_common.h"

namespace beerocks_message {


class cACTION_APMANAGER_4ADDR_STA_JOINED : public BaseClass
{
    public:
        cACTION_APMANAGER_4ADDR_STA_JOINED(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_APMANAGER_4ADDR_STA_JOINED(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_APMANAGER_4ADDR_STA_JOINED();

        static eActionOp_APMANAGER get_action_op(){
            return (eActionOp_APMANAGER)(ACTION_APMANAGER_4ADDR_STA_JOINED);
        }
        beerocks::net::sMacAddr& src_mac();
        beerocks::net::sMacAddr& dst_mac();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_APMANAGER* m_action_op = nullptr;
        beerocks::net::sMacAddr* m_src_mac = nullptr;
        beerocks::net::sMacAddr* m_dst_mac = nullptr;
};

class cACTION_APMANAGER_JOINED_NOTIFICATION : public BaseClass
{
    public:
        cACTION_APMANAGER_JOINED_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_APMANAGER_JOINED_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_APMANAGER_JOINED_NOTIFICATION();

        static eActionOp_APMANAGER get_action_op(){
            return (eActionOp_APMANAGER)(ACTION_APMANAGER_JOINED_NOTIFICATION);
        }
        sNodeHostap& params();
        sApChannelSwitch& cs_params();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_APMANAGER* m_action_op = nullptr;
        sNodeHostap* m_params = nullptr;
        sApChannelSwitch* m_cs_params = nullptr;
};

class cACTION_APMANAGER_INIT_DONE_NOTIFICATION : public BaseClass
{
    public:
        cACTION_APMANAGER_INIT_DONE_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_APMANAGER_INIT_DONE_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_APMANAGER_INIT_DONE_NOTIFICATION();

        static eActionOp_APMANAGER get_action_op(){
            return (eActionOp_APMANAGER)(ACTION_APMANAGER_INIT_DONE_NOTIFICATION);
        }
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_APMANAGER* m_action_op = nullptr;
};

class cACTION_APMANAGER_HOSTAP_SET_RESTRICTED_FAILSAFE_CHANNEL_REQUEST : public BaseClass
{
    public:
        cACTION_APMANAGER_HOSTAP_SET_RESTRICTED_FAILSAFE_CHANNEL_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_APMANAGER_HOSTAP_SET_RESTRICTED_FAILSAFE_CHANNEL_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_APMANAGER_HOSTAP_SET_RESTRICTED_FAILSAFE_CHANNEL_REQUEST();

        static eActionOp_APMANAGER get_action_op(){
            return (eActionOp_APMANAGER)(ACTION_APMANAGER_HOSTAP_SET_RESTRICTED_FAILSAFE_CHANNEL_REQUEST);
        }
        sApSetRestrictedFailsafe& params();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_APMANAGER* m_action_op = nullptr;
        sApSetRestrictedFailsafe* m_params = nullptr;
};

class cACTION_APMANAGER_HOSTAP_SET_RESTRICTED_FAILSAFE_CHANNEL_RESPONSE : public BaseClass
{
    public:
        cACTION_APMANAGER_HOSTAP_SET_RESTRICTED_FAILSAFE_CHANNEL_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_APMANAGER_HOSTAP_SET_RESTRICTED_FAILSAFE_CHANNEL_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_APMANAGER_HOSTAP_SET_RESTRICTED_FAILSAFE_CHANNEL_RESPONSE();

        static eActionOp_APMANAGER get_action_op(){
            return (eActionOp_APMANAGER)(ACTION_APMANAGER_HOSTAP_SET_RESTRICTED_FAILSAFE_CHANNEL_RESPONSE);
        }
        uint8_t& success();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_APMANAGER* m_action_op = nullptr;
        uint8_t* m_success = nullptr;
};

class cACTION_APMANAGER_HOSTAP_AP_DISABLED_NOTIFICATION : public BaseClass
{
    public:
        cACTION_APMANAGER_HOSTAP_AP_DISABLED_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_APMANAGER_HOSTAP_AP_DISABLED_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_APMANAGER_HOSTAP_AP_DISABLED_NOTIFICATION();

        static eActionOp_APMANAGER get_action_op(){
            return (eActionOp_APMANAGER)(ACTION_APMANAGER_HOSTAP_AP_DISABLED_NOTIFICATION);
        }
        int8_t& vap_id();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_APMANAGER* m_action_op = nullptr;
        int8_t* m_vap_id = nullptr;
};

class cACTION_APMANAGER_HOSTAP_AP_ENABLED_NOTIFICATION : public BaseClass
{
    public:
        cACTION_APMANAGER_HOSTAP_AP_ENABLED_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_APMANAGER_HOSTAP_AP_ENABLED_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_APMANAGER_HOSTAP_AP_ENABLED_NOTIFICATION();

        static eActionOp_APMANAGER get_action_op(){
            return (eActionOp_APMANAGER)(ACTION_APMANAGER_HOSTAP_AP_ENABLED_NOTIFICATION);
        }
        int8_t& vap_id();
        sVapInfo& vap_info();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_APMANAGER* m_action_op = nullptr;
        int8_t* m_vap_id = nullptr;
        sVapInfo* m_vap_info = nullptr;
};

class cACTION_APMANAGER_HOSTAP_VAPS_LIST_UPDATE_REQUEST : public BaseClass
{
    public:
        cACTION_APMANAGER_HOSTAP_VAPS_LIST_UPDATE_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_APMANAGER_HOSTAP_VAPS_LIST_UPDATE_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_APMANAGER_HOSTAP_VAPS_LIST_UPDATE_REQUEST();

        static eActionOp_APMANAGER get_action_op(){
            return (eActionOp_APMANAGER)(ACTION_APMANAGER_HOSTAP_VAPS_LIST_UPDATE_REQUEST);
        }
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_APMANAGER* m_action_op = nullptr;
};

class cACTION_APMANAGER_HOSTAP_VAPS_LIST_UPDATE_NOTIFICATION : public BaseClass
{
    public:
        cACTION_APMANAGER_HOSTAP_VAPS_LIST_UPDATE_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_APMANAGER_HOSTAP_VAPS_LIST_UPDATE_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_APMANAGER_HOSTAP_VAPS_LIST_UPDATE_NOTIFICATION();

        static eActionOp_APMANAGER get_action_op(){
            return (eActionOp_APMANAGER)(ACTION_APMANAGER_HOSTAP_VAPS_LIST_UPDATE_NOTIFICATION);
        }
        sVapsList& params();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_APMANAGER* m_action_op = nullptr;
        sVapsList* m_params = nullptr;
};

class cACTION_APMANAGER_HOSTAP_CHANNEL_SWITCH_ACS_START : public BaseClass
{
    public:
        cACTION_APMANAGER_HOSTAP_CHANNEL_SWITCH_ACS_START(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_APMANAGER_HOSTAP_CHANNEL_SWITCH_ACS_START(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_APMANAGER_HOSTAP_CHANNEL_SWITCH_ACS_START();

        static eActionOp_APMANAGER get_action_op(){
            return (eActionOp_APMANAGER)(ACTION_APMANAGER_HOSTAP_CHANNEL_SWITCH_ACS_START);
        }
        sApChannelSwitch& cs_params();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_APMANAGER* m_action_op = nullptr;
        sApChannelSwitch* m_cs_params = nullptr;
};

class cACTION_APMANAGER_HOSTAP_CSA_ERROR_NOTIFICATION : public BaseClass
{
    public:
        cACTION_APMANAGER_HOSTAP_CSA_ERROR_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_APMANAGER_HOSTAP_CSA_ERROR_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_APMANAGER_HOSTAP_CSA_ERROR_NOTIFICATION();

        static eActionOp_APMANAGER get_action_op(){
            return (eActionOp_APMANAGER)(ACTION_APMANAGER_HOSTAP_CSA_ERROR_NOTIFICATION);
        }
        sApChannelSwitch& cs_params();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_APMANAGER* m_action_op = nullptr;
        sApChannelSwitch* m_cs_params = nullptr;
};

class cACTION_APMANAGER_HOSTAP_CSA_NOTIFICATION : public BaseClass
{
    public:
        cACTION_APMANAGER_HOSTAP_CSA_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_APMANAGER_HOSTAP_CSA_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_APMANAGER_HOSTAP_CSA_NOTIFICATION();

        static eActionOp_APMANAGER get_action_op(){
            return (eActionOp_APMANAGER)(ACTION_APMANAGER_HOSTAP_CSA_NOTIFICATION);
        }
        sApChannelSwitch& cs_params();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_APMANAGER* m_action_op = nullptr;
        sApChannelSwitch* m_cs_params = nullptr;
};

class cACTION_APMANAGER_HOSTAP_ACS_ERROR_NOTIFICATION : public BaseClass
{
    public:
        cACTION_APMANAGER_HOSTAP_ACS_ERROR_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_APMANAGER_HOSTAP_ACS_ERROR_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_APMANAGER_HOSTAP_ACS_ERROR_NOTIFICATION();

        static eActionOp_APMANAGER get_action_op(){
            return (eActionOp_APMANAGER)(ACTION_APMANAGER_HOSTAP_ACS_ERROR_NOTIFICATION);
        }
        sApChannelSwitch& cs_params();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_APMANAGER* m_action_op = nullptr;
        sApChannelSwitch* m_cs_params = nullptr;
};

class cACTION_APMANAGER_HOSTAP_ACS_NOTIFICATION : public BaseClass
{
    public:
        cACTION_APMANAGER_HOSTAP_ACS_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_APMANAGER_HOSTAP_ACS_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_APMANAGER_HOSTAP_ACS_NOTIFICATION();

        static eActionOp_APMANAGER get_action_op(){
            return (eActionOp_APMANAGER)(ACTION_APMANAGER_HOSTAP_ACS_NOTIFICATION);
        }
        sApChannelSwitch& cs_params();
        std::tuple<bool, sWifiChannel&> supported_channels_list(size_t idx);
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_APMANAGER* m_action_op = nullptr;
        sApChannelSwitch* m_cs_params = nullptr;
        sWifiChannel* m_supported_channels_list = nullptr;
        size_t m_supported_channels_list_idx__ = 0;
};

class cACTION_APMANAGER_HOSTAP_DFS_CAC_COMPLETED_NOTIFICATION : public BaseClass
{
    public:
        cACTION_APMANAGER_HOSTAP_DFS_CAC_COMPLETED_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_APMANAGER_HOSTAP_DFS_CAC_COMPLETED_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_APMANAGER_HOSTAP_DFS_CAC_COMPLETED_NOTIFICATION();

        static eActionOp_APMANAGER get_action_op(){
            return (eActionOp_APMANAGER)(ACTION_APMANAGER_HOSTAP_DFS_CAC_COMPLETED_NOTIFICATION);
        }
        sDfsCacCompleted& params();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_APMANAGER* m_action_op = nullptr;
        sDfsCacCompleted* m_params = nullptr;
};

class cACTION_APMANAGER_HOSTAP_DFS_CHANNEL_AVAILABLE_NOTIFICATION : public BaseClass
{
    public:
        cACTION_APMANAGER_HOSTAP_DFS_CHANNEL_AVAILABLE_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_APMANAGER_HOSTAP_DFS_CHANNEL_AVAILABLE_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_APMANAGER_HOSTAP_DFS_CHANNEL_AVAILABLE_NOTIFICATION();

        static eActionOp_APMANAGER get_action_op(){
            return (eActionOp_APMANAGER)(ACTION_APMANAGER_HOSTAP_DFS_CHANNEL_AVAILABLE_NOTIFICATION);
        }
        sDfsChannelAvailable& params();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_APMANAGER* m_action_op = nullptr;
        sDfsChannelAvailable* m_params = nullptr;
};

class cACTION_APMANAGER_HOSTAP_ADD_4ADDR_STA_UPDATE : public BaseClass
{
    public:
        cACTION_APMANAGER_HOSTAP_ADD_4ADDR_STA_UPDATE(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_APMANAGER_HOSTAP_ADD_4ADDR_STA_UPDATE(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_APMANAGER_HOSTAP_ADD_4ADDR_STA_UPDATE();

        static eActionOp_APMANAGER get_action_op(){
            return (eActionOp_APMANAGER)(ACTION_APMANAGER_HOSTAP_ADD_4ADDR_STA_UPDATE);
        }
        beerocks::net::sMacAddr& mac();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_APMANAGER* m_action_op = nullptr;
        beerocks::net::sMacAddr* m_mac = nullptr;
};

class cACTION_APMANAGER_HOSTAP_DEL_4ADDR_STA_UPDATE : public BaseClass
{
    public:
        cACTION_APMANAGER_HOSTAP_DEL_4ADDR_STA_UPDATE(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_APMANAGER_HOSTAP_DEL_4ADDR_STA_UPDATE(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_APMANAGER_HOSTAP_DEL_4ADDR_STA_UPDATE();

        static eActionOp_APMANAGER get_action_op(){
            return (eActionOp_APMANAGER)(ACTION_APMANAGER_HOSTAP_DEL_4ADDR_STA_UPDATE);
        }
        beerocks::net::sMacAddr& mac();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_APMANAGER* m_action_op = nullptr;
        beerocks::net::sMacAddr* m_mac = nullptr;
};

class cACTION_APMANAGER_HOSTAP_SET_NEIGHBOR_11K_REQUEST : public BaseClass
{
    public:
        cACTION_APMANAGER_HOSTAP_SET_NEIGHBOR_11K_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_APMANAGER_HOSTAP_SET_NEIGHBOR_11K_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_APMANAGER_HOSTAP_SET_NEIGHBOR_11K_REQUEST();

        static eActionOp_APMANAGER get_action_op(){
            return (eActionOp_APMANAGER)(ACTION_APMANAGER_HOSTAP_SET_NEIGHBOR_11K_REQUEST);
        }
        sNeighborSetParams11k& params();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_APMANAGER* m_action_op = nullptr;
        sNeighborSetParams11k* m_params = nullptr;
};

class cACTION_APMANAGER_HOSTAP_REMOVE_NEIGHBOR_11K_REQUEST : public BaseClass
{
    public:
        cACTION_APMANAGER_HOSTAP_REMOVE_NEIGHBOR_11K_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_APMANAGER_HOSTAP_REMOVE_NEIGHBOR_11K_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_APMANAGER_HOSTAP_REMOVE_NEIGHBOR_11K_REQUEST();

        static eActionOp_APMANAGER get_action_op(){
            return (eActionOp_APMANAGER)(ACTION_APMANAGER_HOSTAP_REMOVE_NEIGHBOR_11K_REQUEST);
        }
        sNeighborRemoveParams11k& params();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_APMANAGER* m_action_op = nullptr;
        sNeighborRemoveParams11k* m_params = nullptr;
};

class cACTION_APMANAGER_CLIENT_ASSOCIATED_NOTIFICATION : public BaseClass
{
    public:
        cACTION_APMANAGER_CLIENT_ASSOCIATED_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_APMANAGER_CLIENT_ASSOCIATED_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_APMANAGER_CLIENT_ASSOCIATED_NOTIFICATION();

        static eActionOp_APMANAGER get_action_op(){
            return (eActionOp_APMANAGER)(ACTION_APMANAGER_CLIENT_ASSOCIATED_NOTIFICATION);
        }
        sClientAssociationParams& params();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_APMANAGER* m_action_op = nullptr;
        sClientAssociationParams* m_params = nullptr;
};

class cACTION_APMANAGER_CLIENT_DISCONNECTED_NOTIFICATION : public BaseClass
{
    public:
        cACTION_APMANAGER_CLIENT_DISCONNECTED_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_APMANAGER_CLIENT_DISCONNECTED_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_APMANAGER_CLIENT_DISCONNECTED_NOTIFICATION();

        static eActionOp_APMANAGER get_action_op(){
            return (eActionOp_APMANAGER)(ACTION_APMANAGER_CLIENT_DISCONNECTED_NOTIFICATION);
        }
        sClientDisconnectionParams& params();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_APMANAGER* m_action_op = nullptr;
        sClientDisconnectionParams* m_params = nullptr;
};

class cACTION_APMANAGER_CLIENT_DISCONNECT_REQUEST : public BaseClass
{
    public:
        cACTION_APMANAGER_CLIENT_DISCONNECT_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_APMANAGER_CLIENT_DISCONNECT_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_APMANAGER_CLIENT_DISCONNECT_REQUEST();

        static eActionOp_APMANAGER get_action_op(){
            return (eActionOp_APMANAGER)(ACTION_APMANAGER_CLIENT_DISCONNECT_REQUEST);
        }
        beerocks::net::sMacAddr& mac();
        int8_t& vap_id();
        uint32_t& type();
        uint32_t& reason();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_APMANAGER* m_action_op = nullptr;
        beerocks::net::sMacAddr* m_mac = nullptr;
        int8_t* m_vap_id = nullptr;
        uint32_t* m_type = nullptr;
        uint32_t* m_reason = nullptr;
};

class cACTION_APMANAGER_CLIENT_DISCONNECT_RESPONSE : public BaseClass
{
    public:
        cACTION_APMANAGER_CLIENT_DISCONNECT_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_APMANAGER_CLIENT_DISCONNECT_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_APMANAGER_CLIENT_DISCONNECT_RESPONSE();

        static eActionOp_APMANAGER get_action_op(){
            return (eActionOp_APMANAGER)(ACTION_APMANAGER_CLIENT_DISCONNECT_RESPONSE);
        }
        sClientDisconnectResponse& params();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_APMANAGER* m_action_op = nullptr;
        sClientDisconnectResponse* m_params = nullptr;
};

class cACTION_APMANAGER_CLIENT_DISALLOW_REQUEST : public BaseClass
{
    public:
        cACTION_APMANAGER_CLIENT_DISALLOW_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_APMANAGER_CLIENT_DISALLOW_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_APMANAGER_CLIENT_DISALLOW_REQUEST();

        static eActionOp_APMANAGER get_action_op(){
            return (eActionOp_APMANAGER)(ACTION_APMANAGER_CLIENT_DISALLOW_REQUEST);
        }
        beerocks::net::sMacAddr& mac();
        uint8_t& reject_sta();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_APMANAGER* m_action_op = nullptr;
        beerocks::net::sMacAddr* m_mac = nullptr;
        uint8_t* m_reject_sta = nullptr;
};

class cACTION_APMANAGER_CLIENT_ALLOW_REQUEST : public BaseClass
{
    public:
        cACTION_APMANAGER_CLIENT_ALLOW_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_APMANAGER_CLIENT_ALLOW_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_APMANAGER_CLIENT_ALLOW_REQUEST();

        static eActionOp_APMANAGER get_action_op(){
            return (eActionOp_APMANAGER)(ACTION_APMANAGER_CLIENT_ALLOW_REQUEST);
        }
        beerocks::net::sMacAddr& mac();
        beerocks::net::sIpv4Addr& ipv4();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_APMANAGER* m_action_op = nullptr;
        beerocks::net::sMacAddr* m_mac = nullptr;
        beerocks::net::sIpv4Addr* m_ipv4 = nullptr;
};

class cACTION_APMANAGER_CLIENT_RX_RSSI_MEASUREMENT_REQUEST : public BaseClass
{
    public:
        cACTION_APMANAGER_CLIENT_RX_RSSI_MEASUREMENT_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_APMANAGER_CLIENT_RX_RSSI_MEASUREMENT_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_APMANAGER_CLIENT_RX_RSSI_MEASUREMENT_REQUEST();

        static eActionOp_APMANAGER get_action_op(){
            return (eActionOp_APMANAGER)(ACTION_APMANAGER_CLIENT_RX_RSSI_MEASUREMENT_REQUEST);
        }
        sNodeRssiMeasurementRequest& params();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_APMANAGER* m_action_op = nullptr;
        sNodeRssiMeasurementRequest* m_params = nullptr;
};

class cACTION_APMANAGER_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE : public BaseClass
{
    public:
        cACTION_APMANAGER_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_APMANAGER_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_APMANAGER_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE();

        static eActionOp_APMANAGER get_action_op(){
            return (eActionOp_APMANAGER)(ACTION_APMANAGER_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE);
        }
        sNodeRssiMeasurement& params();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_APMANAGER* m_action_op = nullptr;
        sNodeRssiMeasurement* m_params = nullptr;
};

class cACTION_APMANAGER_CLIENT_IRE_CONNECTED_NOTIFICATION : public BaseClass
{
    public:
        cACTION_APMANAGER_CLIENT_IRE_CONNECTED_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_APMANAGER_CLIENT_IRE_CONNECTED_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_APMANAGER_CLIENT_IRE_CONNECTED_NOTIFICATION();

        static eActionOp_APMANAGER get_action_op(){
            return (eActionOp_APMANAGER)(ACTION_APMANAGER_CLIENT_IRE_CONNECTED_NOTIFICATION);
        }
        beerocks::net::sMacAddr& mac();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_APMANAGER* m_action_op = nullptr;
        beerocks::net::sMacAddr* m_mac = nullptr;
};

class cACTION_APMANAGER_CLIENT_BSS_STEER_REQUEST : public BaseClass
{
    public:
        cACTION_APMANAGER_CLIENT_BSS_STEER_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_APMANAGER_CLIENT_BSS_STEER_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_APMANAGER_CLIENT_BSS_STEER_REQUEST();

        static eActionOp_APMANAGER get_action_op(){
            return (eActionOp_APMANAGER)(ACTION_APMANAGER_CLIENT_BSS_STEER_REQUEST);
        }
        sNodeBssSteerRequest& params();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_APMANAGER* m_action_op = nullptr;
        sNodeBssSteerRequest* m_params = nullptr;
};

class cACTION_APMANAGER_CLIENT_BSS_STEER_RESPONSE : public BaseClass
{
    public:
        cACTION_APMANAGER_CLIENT_BSS_STEER_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_APMANAGER_CLIENT_BSS_STEER_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_APMANAGER_CLIENT_BSS_STEER_RESPONSE();

        static eActionOp_APMANAGER get_action_op(){
            return (eActionOp_APMANAGER)(ACTION_APMANAGER_CLIENT_BSS_STEER_RESPONSE);
        }
        sNodeBssSteerResponse& params();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_APMANAGER* m_action_op = nullptr;
        sNodeBssSteerResponse* m_params = nullptr;
};

class cACTION_APMANAGER_CLIENT_RX_RSSI_MEASUREMENT_CMD_RESPONSE : public BaseClass
{
    public:
        cACTION_APMANAGER_CLIENT_RX_RSSI_MEASUREMENT_CMD_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_APMANAGER_CLIENT_RX_RSSI_MEASUREMENT_CMD_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_APMANAGER_CLIENT_RX_RSSI_MEASUREMENT_CMD_RESPONSE();

        static eActionOp_APMANAGER get_action_op(){
            return (eActionOp_APMANAGER)(ACTION_APMANAGER_CLIENT_RX_RSSI_MEASUREMENT_CMD_RESPONSE);
        }
        beerocks::net::sMacAddr& mac();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_APMANAGER* m_action_op = nullptr;
        beerocks::net::sMacAddr* m_mac = nullptr;
};

class cACTION_APMANAGER_STEERING_CLIENT_SET_REQUEST : public BaseClass
{
    public:
        cACTION_APMANAGER_STEERING_CLIENT_SET_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_APMANAGER_STEERING_CLIENT_SET_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_APMANAGER_STEERING_CLIENT_SET_REQUEST();

        static eActionOp_APMANAGER get_action_op(){
            return (eActionOp_APMANAGER)(ACTION_APMANAGER_STEERING_CLIENT_SET_REQUEST);
        }
        sSteeringClientSetRequest& params();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_APMANAGER* m_action_op = nullptr;
        sSteeringClientSetRequest* m_params = nullptr;
};

class cACTION_APMANAGER_STEERING_CLIENT_SET_RESPONSE : public BaseClass
{
    public:
        cACTION_APMANAGER_STEERING_CLIENT_SET_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_APMANAGER_STEERING_CLIENT_SET_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_APMANAGER_STEERING_CLIENT_SET_RESPONSE();

        static eActionOp_APMANAGER get_action_op(){
            return (eActionOp_APMANAGER)(ACTION_APMANAGER_STEERING_CLIENT_SET_RESPONSE);
        }
        sSteeringClientSetResponse& params();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_APMANAGER* m_action_op = nullptr;
        sSteeringClientSetResponse* m_params = nullptr;
};

class cACTION_APMANAGER_STEERING_EVENT_PROBE_REQ_NOTIFICATION : public BaseClass
{
    public:
        cACTION_APMANAGER_STEERING_EVENT_PROBE_REQ_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_APMANAGER_STEERING_EVENT_PROBE_REQ_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_APMANAGER_STEERING_EVENT_PROBE_REQ_NOTIFICATION();

        static eActionOp_APMANAGER get_action_op(){
            return (eActionOp_APMANAGER)(ACTION_APMANAGER_STEERING_EVENT_PROBE_REQ_NOTIFICATION);
        }
        sSteeringEvProbeReq& params();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_APMANAGER* m_action_op = nullptr;
        sSteeringEvProbeReq* m_params = nullptr;
};

class cACTION_APMANAGER_STEERING_EVENT_AUTH_FAIL_NOTIFICATION : public BaseClass
{
    public:
        cACTION_APMANAGER_STEERING_EVENT_AUTH_FAIL_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_APMANAGER_STEERING_EVENT_AUTH_FAIL_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_APMANAGER_STEERING_EVENT_AUTH_FAIL_NOTIFICATION();

        static eActionOp_APMANAGER get_action_op(){
            return (eActionOp_APMANAGER)(ACTION_APMANAGER_STEERING_EVENT_AUTH_FAIL_NOTIFICATION);
        }
        sSteeringEvAuthFail& params();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_APMANAGER* m_action_op = nullptr;
        sSteeringEvAuthFail* m_params = nullptr;
};

class cACTION_APMANAGER_HEARTBEAT_NOTIFICATION : public BaseClass
{
    public:
        cACTION_APMANAGER_HEARTBEAT_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_APMANAGER_HEARTBEAT_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_APMANAGER_HEARTBEAT_NOTIFICATION();

        static eActionOp_APMANAGER get_action_op(){
            return (eActionOp_APMANAGER)(ACTION_APMANAGER_HEARTBEAT_NOTIFICATION);
        }
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_APMANAGER* m_action_op = nullptr;
};

}; // close namespace: beerocks_message

#endif //_BEEROCKS/TLVF_BEEROCKS_MESSAGE_APMANAGER_H_
