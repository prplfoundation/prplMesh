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

#ifndef _TLVF_WFA_MAP_TLVASSOCIATEDSTATRAFFICSTATS_H_
#define _TLVF_WFA_MAP_TLVASSOCIATEDSTATRAFFICSTATS_H_

#include <cstddef>
#include <stdint.h>
#include <tlvf/swap.h>
#include <string.h>
#include <memory>
#include <tlvf/BaseClass.h>
#include <tlvf/ClassList.h>
#include "tlvf/wfa_map/eTlvTypeMap.h"
#include "tlvf/common/sMacAddr.h"

namespace wfa_map {


class tlvAssociatedStaTrafficStats : public BaseClass
{
    public:
        tlvAssociatedStaTrafficStats(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit tlvAssociatedStaTrafficStats(std::shared_ptr<BaseClass> base, bool parse = false);
        ~tlvAssociatedStaTrafficStats();

        const eTlvTypeMap& type();
        const uint16_t& length();
        sMacAddr& sta_mac();
        uint32_t& byte_sent();
        uint32_t& byte_recived();
        uint32_t& packets_sent();
        uint32_t& packets_recived();
        uint32_t& tx_packets_error();
        uint32_t& rx_packets_error();
        uint32_t& retransmission_count();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eTlvTypeMap* m_type = nullptr;
        uint16_t* m_length = nullptr;
        sMacAddr* m_sta_mac = nullptr;
        uint32_t* m_byte_sent = nullptr;
        uint32_t* m_byte_recived = nullptr;
        uint32_t* m_packets_sent = nullptr;
        uint32_t* m_packets_recived = nullptr;
        uint32_t* m_tx_packets_error = nullptr;
        uint32_t* m_rx_packets_error = nullptr;
        uint32_t* m_retransmission_count = nullptr;
};

}; // close namespace: wfa_map

#endif //_TLVF/WFA_MAP_TLVASSOCIATEDSTATRAFFICSTATS_H_
