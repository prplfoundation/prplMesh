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

#ifndef _TLVF_WFA_MAP_TLVTRANSMITPOWERLIMIT_H_
#define _TLVF_WFA_MAP_TLVTRANSMITPOWERLIMIT_H_

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


class tlvTransmitPowerLimit : public BaseClass
{
    public:
        tlvTransmitPowerLimit(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit tlvTransmitPowerLimit(std::shared_ptr<BaseClass> base, bool parse = false);
        ~tlvTransmitPowerLimit();

        const eTlvTypeMap& type();
        const uint16_t& length();
        sMacAddr& radio_uid();
        //Transmit Power Limit EIRP per 20 MHz bandwidth representing the nominal transmit power limit for this radio.
        //The field is coded as a 2's complement signed integer in units of decibels relative to 1 mW (dBm).
        int8_t& transmit_power_limit_dbm();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eTlvTypeMap* m_type = nullptr;
        uint16_t* m_length = nullptr;
        sMacAddr* m_radio_uid = nullptr;
        int8_t* m_transmit_power_limit_dbm = nullptr;
};

}; // close namespace: wfa_map

#endif //_TLVF/WFA_MAP_TLVTRANSMITPOWERLIMIT_H_
