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

#ifndef _TLVF_WSC_SM2_H_
#define _TLVF_WSC_SM2_H_

#include <cstddef>
#include <stdint.h>
#include <tlvf/swap.h>
#include "tlvf/WSC/WSC_Attributes.h"

namespace WSC {

typedef struct sM2 {
    WSC::sWscAttrVersion version_attr;
    WSC::sWscAttrMessageType message_type_attr;
    WSC::sWscAttrEnroleeNonce enrolee_nonce_attr;
    WSC::sWscAttrRegistrarNonce registrar_nonce_attr;
    WSC::sWscAttrUuidR uuid_r_attr;
    WSC::sWscAttrPublicKey public_key_attr;
    WSC::sWscAttrAuthenticationTypeFlags authentication_type_flags_attr;
    WSC::sWscAttrEncryptionTypeFlags encryption_type_flags_attr;
    WSC::sWscAttrConnectionTypeFlags connection_type_flags_attr;
    WSC::sWscAttrConfigurationMethods configuration_methods_attr;
    WSC::sWscAttrManufacturer manufacturer_attr;
    WSC::sWscAttrModelName model_name_attr;
    WSC::sWscAttrModelNumber model_number_attr;
    WSC::sWscAttrSerialNumber serial_number_attr;
    WSC::sWscAttrPrimaryDeviceType primary_device_type_attr;
    WSC::sWscAttrRfBands rf_bands_attr;
    WSC::sWscAttrAssociationState association_state_attr;
    WSC::sWscAttrConfigurationError configuration_error_attr;
    WSC::sWscAttrDevicePasswordID device_password_id_attr;
    WSC::sWscAttrOsVersion os_version_attr;
    WSC::sWscAttrVersionExtension vendor_extensions_attr;
    sWscAttrEncryptedSettings encrypted_settings_attr;
    void struct_swap(){
        version_attr.struct_swap();
        message_type_attr.struct_swap();
        enrolee_nonce_attr.struct_swap();
        registrar_nonce_attr.struct_swap();
        uuid_r_attr.struct_swap();
        public_key_attr.struct_swap();
        authentication_type_flags_attr.struct_swap();
        encryption_type_flags_attr.struct_swap();
        connection_type_flags_attr.struct_swap();
        configuration_methods_attr.struct_swap();
        manufacturer_attr.struct_swap();
        model_name_attr.struct_swap();
        model_number_attr.struct_swap();
        serial_number_attr.struct_swap();
        primary_device_type_attr.struct_swap();
        rf_bands_attr.struct_swap();
        association_state_attr.struct_swap();
        configuration_error_attr.struct_swap();
        device_password_id_attr.struct_swap();
        os_version_attr.struct_swap();
        vendor_extensions_attr.struct_swap();
        encrypted_settings_attr.struct_swap();
    }
    void struct_init(){
        version_attr.struct_init();
        message_type_attr.struct_init();
        enrolee_nonce_attr.struct_init();
        registrar_nonce_attr.struct_init();
        uuid_r_attr.struct_init();
        public_key_attr.struct_init();
        authentication_type_flags_attr.struct_init();
        encryption_type_flags_attr.struct_init();
        connection_type_flags_attr.struct_init();
        configuration_methods_attr.struct_init();
        manufacturer_attr.struct_init();
        model_name_attr.struct_init();
        model_number_attr.struct_init();
        serial_number_attr.struct_init();
        primary_device_type_attr.struct_init();
        rf_bands_attr.struct_init();
        association_state_attr.struct_init();
        configuration_error_attr.struct_init();
        device_password_id_attr.struct_init();
        os_version_attr.struct_init();
        vendor_extensions_attr.struct_init();
        encrypted_settings_attr.struct_init();
    }
} sM2;


}; // close namespace: WSC

#endif //_TLVF/WSC_SM2_H_
