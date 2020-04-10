///////////////////////////////////////
// AUTO GENERATED FILE - DO NOT EDIT //
///////////////////////////////////////

/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2019 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _TLVF_WFA_MAP_TLVASSOCIATEDCLIENTS_H_
#define _TLVF_WFA_MAP_TLVASSOCIATEDCLIENTS_H_

#include <cstddef>
#include <stdint.h>
#include <tlvf/swap.h>
#include <string.h>
#include <memory>
#include <tlvf/BaseClass.h>
#include <tlvf/ClassList.h>
#include "tlvf/wfa_map/eTlvTypeMap.h"
#include <tuple>
#include <vector>
#include "tlvf/common/sMacAddr.h"

namespace wfa_map {

class cBssInfo;
class cClientInfo;

class tlvAssociatedClients : public BaseClass
{
    public:
        tlvAssociatedClients(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit tlvAssociatedClients(std::shared_ptr<BaseClass> base, bool parse = false);
        ~tlvAssociatedClients();

        const eTlvTypeMap& type();
        const uint16_t& length();
        uint8_t& bss_list_length();
        std::tuple<bool, cBssInfo&> bss_list(size_t idx);
        std::shared_ptr<cBssInfo> create_bss_list();
        bool add_bss_list(std::shared_ptr<cBssInfo> ptr);
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eTlvTypeMap* m_type = nullptr;
        uint16_t* m_length = nullptr;
        uint8_t* m_bss_list_length = nullptr;
        cBssInfo* m_bss_list = nullptr;
        size_t m_bss_list_idx__ = 0;
        std::vector<std::shared_ptr<cBssInfo>> m_bss_list_vector;
        bool m_lock_allocation__ = false;
        int m_lock_order_counter__ = 0;
};

class cBssInfo : public BaseClass
{
    public:
        cBssInfo(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cBssInfo(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cBssInfo();

        sMacAddr& bssid();
        uint16_t& clients_associated_list_length();
        std::tuple<bool, cClientInfo&> clients_associated_list(size_t idx);
        std::shared_ptr<cClientInfo> create_clients_associated_list();
        bool add_clients_associated_list(std::shared_ptr<cClientInfo> ptr);
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        sMacAddr* m_bssid = nullptr;
        uint16_t* m_clients_associated_list_length = nullptr;
        cClientInfo* m_clients_associated_list = nullptr;
        size_t m_clients_associated_list_idx__ = 0;
        std::vector<std::shared_ptr<cClientInfo>> m_clients_associated_list_vector;
        bool m_lock_allocation__ = false;
        int m_lock_order_counter__ = 0;
};

class cClientInfo : public BaseClass
{
    public:
        cClientInfo(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cClientInfo(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cClientInfo();

        sMacAddr& mac();
        uint16_t& time_since_last_association_sec();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        sMacAddr* m_mac = nullptr;
        uint16_t* m_time_since_last_association_sec = nullptr;
};

}; // close namespace: wfa_map

#endif //_TLVF/WFA_MAP_TLVASSOCIATEDCLIENTS_H_
