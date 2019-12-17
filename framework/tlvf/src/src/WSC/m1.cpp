/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2019 Tomer Eliyahu (Intel)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include <tlvf/WSC/m1.h>
#include <tlvf/tlvfdefines.h>
#include <tlvf/tlvflogging.h>

using namespace WSC;

bool m1::init(const config &cfg)
{
    if (m_parse) {
        TLVF_LOG(ERROR) << "init(cfg) called but m_parse is set!";
        return false; // Used for create only
    }

    auto version = addAttr<cWscAttrVersion>();
    if (!version) {
        TLVF_LOG(ERROR) << "addAttr<cWscAttrVersion> failed";
        return false;
    }

    if (cfg.msg_type != eWscMessageType::WSC_MSG_TYPE_M1) {
        TLVF_LOG(ERROR) << "Invalid message type " << cfg.msg_type;
        return false;
    }
    auto msg_type_attr = addAttr<cWscAttrMessageType>();
    if (!msg_type_attr) {
        TLVF_LOG(ERROR) << "addAttr<cWscAttrMessageType> failed";
        return false;
    }
    msg_type_attr->msg_type() = cfg.msg_type;

    auto uuide_attr = addAttr<cWscAttrUuidE>();
    if (!uuide_attr) {
        TLVF_LOG(ERROR) << "addAttr<cWscAttrUuidE> failed";
        return false;
    }
    std::memset(uuide_attr->data(), 0xff, uuide_attr->length());

    auto mac_attr = addAttr<cWscAttrMac>();
    if (!mac_attr) {
        TLVF_LOG(ERROR) << "addAttr<cWscAttrMac> failed";
        return false;
    }
    mac_attr->data() = cfg.mac;

    auto enrolee_nonce_attr = addAttr<cWscAttrEnrolleeNonce>();
    if (!enrolee_nonce_attr) {
        TLVF_LOG(ERROR) << "addAttr<cWscAttrEnrolleeNonce> failed";
        return false;
    }
    std::copy_n(cfg.enrollee_nonce, WSC_NONCE_LENGTH, enrolee_nonce_attr->nonce());

    auto public_key_attr = addAttr<cWscAttrPublicKey>();
    if (!public_key_attr) {
        TLVF_LOG(ERROR) << "addAttr<cWscAttrPublicKey> failed";
        return false;
    }
    std::copy_n(cfg.pub_key, WSC_PUBLIC_KEY_LENGTH, public_key_attr->public_key());

    auto auth_type_attr = addAttr<cWscAttrAuthenticationTypeFlags>();
    if (!auth_type_attr) {
        TLVF_LOG(ERROR) << "addAttr<cWscAttrAuthenticationTypeFlags> failed";
        return false;
    }
    auth_type_attr->auth_type_flags() = cfg.auth_type_flags;

    auto enc_type_attr = addAttr<cWscAttrEncryptionTypeFlags>();
    if (!enc_type_attr) {
        TLVF_LOG(ERROR) << "addAttr<cWscAttrEncryptionTypeFlags> failed";
        return false;
    }
    enc_type_attr->encr_type_flags() = cfg.encr_type_flags;

    auto connection_type_attr = addAttr<cWscAttrConnectionTypeFlags>();
    if (!connection_type_attr) {
        TLVF_LOG(ERROR) << "addAttr<cWscAttrConnectionTypeFlags> failed";
        return false;
    }

    auto config_methods_attr = addAttr<cWscAttrConfigurationMethods>();
    if (!config_methods_attr) {
        TLVF_LOG(ERROR) << "addAttr<cWscAttrConfigurationMethods> failed";
        return false;
    }

    auto wsc_state_attr = addAttr<cWscAttrWscState>();
    if (!wsc_state_attr) {
        TLVF_LOG(ERROR) << "addAttr<cWscAttrWscState> failed";
        return false;
    }

    auto manufacturer_attr = addAttr<cWscAttrManufacturer>();
    if (!manufacturer_attr || !manufacturer_attr->set_manufacturer(cfg.manufacturer)) {
        TLVF_LOG(ERROR) << "addAttr<cWscAttrManufacturer> failed";
        return false;
    }

    auto model_attr = addAttr<cWscAttrModelName>();
    if (!model_attr || !model_attr->set_model(cfg.model_name)) {
        TLVF_LOG(ERROR) << "addAttr<cWscAttrModelName> failed";
        return false;
    }

    auto model_number_attr = addAttr<cWscAttrModelNumber>();
    if (!model_number_attr || !model_number_attr->set_model_number(cfg.model_number)) {
        TLVF_LOG(ERROR) << "addAttr<cWscAttrModelNumber> failed";
        return false;
    }

    auto serial_number_attr = addAttr<cWscAttrSerialNumber>();
    if (!serial_number_attr || !serial_number_attr->set_serial_number(cfg.serial_number)) {
        TLVF_LOG(ERROR) << "addAttr<cWscAttrSerialNumber> failed";
        return false;
    }

    auto primary_device_attr = addAttr<cWscAttrPrimaryDeviceType>();
    if (!primary_device_attr) {
        TLVF_LOG(ERROR) << "addAttr<cWscAttrPrimaryDeviceType> failed";
        return false;
    }
    primary_device_attr->sub_category_id() = cfg.primary_dev_type_id;

    auto devname_attr = addAttr<cWscAttrDeviceName>();
    if (!devname_attr || !devname_attr->set_device_name(cfg.device_name)) {
        TLVF_LOG(ERROR) << "addAttr<cWscAttrDeviceName> failed";
        return false;
    }

    auto rfbands_attr = addAttr<cWscAttrRfBands>();
    if (!rfbands_attr) {
        TLVF_LOG(ERROR) << "addAttr<cWscAttrRfBands> failed";
        return false;
    }
    rfbands_attr->bands() = cfg.bands;

    auto assoc_state_attr = addAttr<cWscAttrAssociationState>();
    if (!assoc_state_attr) {
        TLVF_LOG(ERROR) << "addAttr<cWscAttrAssociationState> failed";
        return false;
    }

    auto dev_pass_attr = addAttr<cWscAttrDevicePasswordID>();
    if (!dev_pass_attr) {
        TLVF_LOG(ERROR) << "addAttr<cWscAttrDevicePasswordID> failed";
        return false;
    }

    auto config_err_attr = addAttr<cWscAttrConfigurationError>();
    if (!config_err_attr) {
        TLVF_LOG(ERROR) << "addAttr<cWscAttrConfigurationError> failed";
        return false;
    }

    auto os_version_attr = addAttr<cWscAttrOsVersion>();
    if (!os_version_attr) {
        TLVF_LOG(ERROR) << "addAttr<cWscAttrOsVersion> failed";
        return false;
    }

    auto vendor_ext_attr = addAttr<cWscVendorExtWfa>();
    if (!vendor_ext_attr) {
        TLVF_LOG(ERROR) << "addAttr<cWscVendorExtWfa> failed";
        return false;
    }

    if (!vendor_ext_attr->alloc_vs_data(sizeof(sWscAttrVersion2))) {
        LOG(ERROR) << "buffer allocation failed for version2 attribute";
        return false;
    }

    sWscAttrVersion2 version2;
    version2.struct_swap();
    uint8_t *version2_buf = reinterpret_cast<uint8_t *>(&version2);
    std::copy(version2_buf, version2_buf + sizeof(version2), vendor_ext_attr->vs_data());

    return true;
}

bool m1::valid_custom() const
{
    if (!getAttr<cWscAttrUuidE>()) {
        TLVF_LOG(ERROR) << "getAttr<cWscAttrUuidE> failed";
        return false;
    }
    if (!getAttr<cWscAttrMac>()) {
        TLVF_LOG(ERROR) << "getAttr<cWscAttrMac> failed";
        return false;
    }
    if (!getAttr<cWscVendorExtWfa>()) {
        TLVF_LOG(ERROR) << "getAttr<cWscVendorExtWfa> failed";
        return false;
    }
    return true;
}
