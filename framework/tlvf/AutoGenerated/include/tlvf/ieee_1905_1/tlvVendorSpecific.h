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

#ifndef _TLVF_IEEE_1905_1_TLVVENDORSPECIFIC_H_
#define _TLVF_IEEE_1905_1_TLVVENDORSPECIFIC_H_

#include <cstddef>
#include <stdint.h>
#include <tlvf/swap.h>
#include <string.h>
#include <memory>
#include <tlvf/BaseClass.h>
#include <tlvf/ClassList.h>
#include "tlvf/ieee_1905_1/eTlvType.h"
#include "tlvf/ieee_1905_1/sVendorOUI.h"
#include <tuple>

namespace ieee1905_1 {


class tlvVendorSpecific : public BaseClass
{
    public:
        tlvVendorSpecific(uint8_t* buff, size_t buff_len, bool parse = false);
        tlvVendorSpecific(std::shared_ptr<BaseClass> base, bool parse = false);
        ~tlvVendorSpecific();

        enum eVendorOUI: uint32_t {
            OUI_BYTES = 0x3,
            OUI_INTEL = 0x470300,
        };
        
        const eTlvType& type();
        const uint16_t& length();
        sVendorOUI& vendor_oui();
        size_t payload_length() { return m_payload_idx__ * sizeof(uint8_t); }
        uint8_t* payload(size_t idx = 0);
        bool alloc_payload(size_t count = 1);
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eTlvType* m_type = nullptr;
        uint16_t* m_length = nullptr;
        sVendorOUI* m_vendor_oui = nullptr;
        uint8_t* m_payload = nullptr;
        size_t m_payload_idx__ = 0;
        int m_lock_order_counter__ = 0;
};

}; // close namespace: ieee1905_1

#endif //_TLVF/IEEE_1905_1_TLVVENDORSPECIFIC_H_
