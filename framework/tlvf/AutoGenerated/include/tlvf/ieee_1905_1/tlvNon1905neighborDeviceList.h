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

#ifndef _TLVF_IEEE_1905_1_TLVNON1905NEIGHBORDEVICELIST_H_
#define _TLVF_IEEE_1905_1_TLVNON1905NEIGHBORDEVICELIST_H_

#include <cstddef>
#include <stdint.h>
#include <tlvf/swap.h>
#include <string.h>
#include <memory>
#include <tlvf/BaseClass.h>
#include <tlvf/ClassList.h>
#include "tlvf/ieee_1905_1/eTlvType.h"
#include "tlvf/common/sMacAddr.h"
#include <tuple>

namespace ieee1905_1 {


class tlvNon1905neighborDeviceList : public BaseClass
{
    public:
        tlvNon1905neighborDeviceList(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit tlvNon1905neighborDeviceList(std::shared_ptr<BaseClass> base, bool parse = false);
        ~tlvNon1905neighborDeviceList();

        const eTlvType& type();
        const uint16_t& length();
        sMacAddr& mac_local_iface();
        size_t mac_non_1905_device_length() { return m_mac_non_1905_device_idx__ * sizeof(sMacAddr); }
        std::tuple<bool, sMacAddr&> mac_non_1905_device(size_t idx);
        bool alloc_mac_non_1905_device(size_t count = 1);
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eTlvType* m_type = nullptr;
        uint16_t* m_length = nullptr;
        sMacAddr* m_mac_local_iface = nullptr;
        sMacAddr* m_mac_non_1905_device = nullptr;
        size_t m_mac_non_1905_device_idx__ = 0;
        int m_lock_order_counter__ = 0;
};

}; // close namespace: ieee1905_1

#endif //_TLVF/IEEE_1905_1_TLVNON1905NEIGHBORDEVICELIST_H_
