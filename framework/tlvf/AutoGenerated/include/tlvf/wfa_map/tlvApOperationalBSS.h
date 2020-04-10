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

#ifndef _TLVF_WFA_MAP_TLVAPOPERATIONALBSS_H_
#define _TLVF_WFA_MAP_TLVAPOPERATIONALBSS_H_

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
#include <tlvf/tlvfutils.h>

namespace wfa_map {

class cRadioInfo;
class cRadioBssInfo;

class tlvApOperationalBSS : public BaseClass
{
    public:
        tlvApOperationalBSS(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit tlvApOperationalBSS(std::shared_ptr<BaseClass> base, bool parse = false);
        ~tlvApOperationalBSS();

        const eTlvTypeMap& type();
        const uint16_t& length();
        uint8_t& radio_list_length();
        std::tuple<bool, cRadioInfo&> radio_list(size_t idx);
        std::shared_ptr<cRadioInfo> create_radio_list();
        bool add_radio_list(std::shared_ptr<cRadioInfo> ptr);
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eTlvTypeMap* m_type = nullptr;
        uint16_t* m_length = nullptr;
        uint8_t* m_radio_list_length = nullptr;
        cRadioInfo* m_radio_list = nullptr;
        size_t m_radio_list_idx__ = 0;
        std::vector<std::shared_ptr<cRadioInfo>> m_radio_list_vector;
        bool m_lock_allocation__ = false;
        int m_lock_order_counter__ = 0;
};

class cRadioInfo : public BaseClass
{
    public:
        cRadioInfo(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cRadioInfo(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cRadioInfo();

        sMacAddr& radio_uid();
        uint8_t& radio_bss_list_length();
        std::tuple<bool, cRadioBssInfo&> radio_bss_list(size_t idx);
        std::shared_ptr<cRadioBssInfo> create_radio_bss_list();
        bool add_radio_bss_list(std::shared_ptr<cRadioBssInfo> ptr);
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        sMacAddr* m_radio_uid = nullptr;
        uint8_t* m_radio_bss_list_length = nullptr;
        cRadioBssInfo* m_radio_bss_list = nullptr;
        size_t m_radio_bss_list_idx__ = 0;
        std::vector<std::shared_ptr<cRadioBssInfo>> m_radio_bss_list_vector;
        bool m_lock_allocation__ = false;
        int m_lock_order_counter__ = 0;
};

class cRadioBssInfo : public BaseClass
{
    public:
        cRadioBssInfo(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cRadioBssInfo(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cRadioBssInfo();

        sMacAddr& radio_bssid();
        uint8_t& ssid_length();
        std::string ssid_str();
        char* ssid(size_t length = 0);
        bool set_ssid(const std::string& str);
        bool set_ssid(const char buffer[], size_t size);
        bool alloc_ssid(size_t count = 1);
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        sMacAddr* m_radio_bssid = nullptr;
        uint8_t* m_ssid_length = nullptr;
        char* m_ssid = nullptr;
        size_t m_ssid_idx__ = 0;
        int m_lock_order_counter__ = 0;
};

}; // close namespace: wfa_map

#endif //_TLVF/WFA_MAP_TLVAPOPERATIONALBSS_H_
