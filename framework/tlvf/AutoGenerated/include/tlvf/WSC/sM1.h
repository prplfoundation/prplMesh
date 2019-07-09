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

#ifndef _TLVF_WSC_SM1_H_
#define _TLVF_WSC_SM1_H_

#include <cstddef>
#include <stdint.h>
#include <tlvf/swap.h>
#include "tlvf/WSC/WSC_Attributes.h"

namespace WSC {

typedef struct sM1 {
    sWscAttrVersion version_attr;
    sWscAttrMessageType message_type_attr;
    sWscAttrUuidE uuid_e_attr;
    sWscAttrMac mac_attr;
    sWscAttrEnroleeNonce enrolee_nonce_attr;
    sWscAttrPublicKey public_key_attr;
    sWscAttrAuthenticationTypeFlags authentication_type_flags_attr;
    sWscAttrEncryptionTypeFlags encryption_type_flags_attr;
    sWscAttrConnectionTypeFlags connection_type_flags_attr;
    sWscAttrConfigurationMethods configuration_methods_attr;
    sWscAttrWscState wsc_state_attr;
    sWscAttrManufacturer manufacturer_attr;
    sWscAttrModelName model_name_attr;
    sWscAttrModelNumber model_number_attr;
    sWscAttrSerialNumber serial_number_attr;
    sWscAttrPrimaryDeviceType primary_device_type_attr;
    sWscAttrDeviceName device_name_attr;
    sWscAttrRfBands rf_bands_attr;
    sWscAttrAssociationState association_state_attr;
    sWscAttrDevicePasswordID device_password_id_attr;
    sWscAttrConfigurationError configuration_error_attr;
    sWscAttrOsVersion os_version_attr;
    sWscAttrVendorExtension vendor_extensions_attr;
    void struct_swap(){
        version_attr.struct_swap();
        message_type_attr.struct_swap();
        uuid_e_attr.struct_swap();
        mac_attr.struct_swap();
        enrolee_nonce_attr.struct_swap();
        public_key_attr.struct_swap();
        authentication_type_flags_attr.struct_swap();
        encryption_type_flags_attr.struct_swap();
        connection_type_flags_attr.struct_swap();
        configuration_methods_attr.struct_swap();
        wsc_state_attr.struct_swap();
        manufacturer_attr.struct_swap();
        model_name_attr.struct_swap();
        model_number_attr.struct_swap();
        serial_number_attr.struct_swap();
        primary_device_type_attr.struct_swap();
        device_name_attr.struct_swap();
        rf_bands_attr.struct_swap();
        association_state_attr.struct_swap();
        device_password_id_attr.struct_swap();
        configuration_error_attr.struct_swap();
        os_version_attr.struct_swap();
        vendor_extensions_attr.struct_swap();
    }
    void struct_init(){
        version_attr.struct_init();
        message_type_attr.struct_init();
        uuid_e_attr.struct_init();
        mac_attr.struct_init();
        enrolee_nonce_attr.struct_init();
        public_key_attr.struct_init();
        authentication_type_flags_attr.struct_init();
        encryption_type_flags_attr.struct_init();
        connection_type_flags_attr.struct_init();
        configuration_methods_attr.struct_init();
        wsc_state_attr.struct_init();
        manufacturer_attr.struct_init();
        model_name_attr.struct_init();
        model_number_attr.struct_init();
        serial_number_attr.struct_init();
        primary_device_type_attr.struct_init();
        device_name_attr.struct_init();
        rf_bands_attr.struct_init();
        association_state_attr.struct_init();
        device_password_id_attr.struct_init();
        configuration_error_attr.struct_init();
        os_version_attr.struct_init();
        vendor_extensions_attr.struct_init();
    }
} __attribute__((packed)) sM1;


}; // close namespace: WSC

#endif //_TLVF/WSC_SM1_H_
