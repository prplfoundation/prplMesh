///////////////////////////////////////
// AUTO GENERATED FILE - DO NOT EDIT //
///////////////////////////////////////
/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Tomer Eliyahu (Intel Corporation)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _TLVF_WSC_WSC_ATTRIBUTES_HELPERS_H_
#define _TLVF_WSC_WSC_ATTRIBUTES_HELPERS_H_

#include <beerocks/bcl/beerocks_utils.h>
#include <tlvf/WSC/WSC_Attributes.h>

namespace WSC {
    enum eBssType : uint8_t {
        BACKHAUL_STA  = 0x80,
        BACKHAUL_BSS  = 0x40,
        FRONTHAUL_BSS = 0x20,
        TEARDOWN      = 0x10
    };

    static int vendor_extentions_bss_type(sWscAttrVendorExtension &attr)
    {
        // verify Multi-AP vendor Id
        if (attr.data[0] != WSC_VENDOR_ID_WFA_1 ||
            attr.data[1] != WSC_VENDOR_ID_WFA_2 ||
            attr.data[2] != WSC_VENDOR_ID_WFA_3)
            return -1;
        
        // verify subelement ID and length match Multi-AP Extension subelement
        if (attr.data[3] != 0x6 || attr.data[4] != 1)
            return -1;
        
        // set the Bss type
        return attr.data[5];
    }

    static void set_vendor_extentions_bss_type(sWscAttrVendorExtension &attr, uint8_t type)
    {
        attr.data[0] = WSC_VENDOR_ID_WFA_1;
        attr.data[1] = WSC_VENDOR_ID_WFA_2;
        attr.data[2] = WSC_VENDOR_ID_WFA_3;
        attr.data[3] = 0x6;
        attr.data[4] = 1;
        attr.data[5] = type;
    }

    static void set_primary_device_type(sWscAttrPrimaryDeviceType &attr, uint16_t type)
    {
        attr.category_id = WSC_DEV_NETWORK_INFRA;
        attr.sub_category_id = type; //eWscDev device type
        attr.oui = 0x0050F204; // WSC WiFi Alliance OUI
    }
}

#endif //_TLVF_WSC_WSC_ATTRIBUTES_HELPERS_H_
