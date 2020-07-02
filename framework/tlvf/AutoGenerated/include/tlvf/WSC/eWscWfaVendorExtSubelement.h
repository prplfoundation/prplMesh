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

#ifndef _TLVF_WSC_EWSCWFAVENDOREXTSUBELEMENT_H_
#define _TLVF_WSC_EWSCWFAVENDOREXTSUBELEMENT_H_

#include <cstddef>
#include <stdint.h>
#include <tlvf/swap.h>

namespace WSC {

enum eWscWfaVendorExtSubelement {
    VERSION2 = 0x0,
    AUTHORIZED_MACS = 0x1,
    NETWORK_KEY_SHAREABLE = 0x2,
    REQUEST_TO_ENROLL = 0x3,
    SETTINGS_DELAY_TIME = 0x4,
    REGISTRAR_CONFIGURATION_METHODS = 0x5,
    MULTI_AP_IDENTIFIER = 0x6,
    MULTI_AP_PROFILE = 0x7,
    MULTI_AP_DEFAULT_802_1Q_SETTING = 0x8,
};


}; // close namespace: WSC

#endif //_TLVF/WSC_EWSCWFAVENDOREXTSUBELEMENT_H_
