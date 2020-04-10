/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2019-2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include <tlvf/WSC/m1.h>
#include <tlvf/tlvfdefines.h>
#include <tlvf/tlvflogging.h>

using namespace WSC;

std::shared_ptr<m1> m1::parse(ieee1905_1::tlvWsc &tlv)
{
    if (!tlv.payload_length()) {
        TLVF_LOG(ERROR) << "No room to add attribute list (payload length = 0)";
        return nullptr;
    }
    auto attributes = std::make_shared<m1>(tlv.payload(), tlv.payload_length(), true);
    if (!attributes) {
        TLVF_LOG(ERROR) << "Failed to initialize attributes";
        return nullptr;
    }
    attributes->init();
    if (attributes->msg_type() != WSC_MSG_TYPE_M1) {
        TLVF_LOG(INFO) << "Not M1, msg type is " << int(attributes->msg_type());
        return nullptr;
    }
    if (!attributes->valid()) {
        TLVF_LOG(ERROR) << "Not all attributes present";
        return nullptr;
    }
    return attributes;
}

std::shared_ptr<m1> m1::create(ieee1905_1::tlvWsc &tlv, const config &cfg)
{
    if (cfg.msg_type != eWscMessageType::WSC_MSG_TYPE_M1)
        return nullptr;
    auto attributes = std::make_shared<m1>(tlv.payload(), tlv.payload_length(), false);
    if (!attributes || !attributes->init(cfg)) {
        TLVF_LOG(ERROR) << "Failed to initialize attributes";
        return nullptr;
    }
    if (!attributes->finalize()) {
        TLVF_LOG(ERROR) << "Failed to finalize attributes";
        return nullptr;
    }
    tlv.addInnerClassList(attributes);
    return attributes;
}

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

bool m1::valid() const
{
    if (!getAttr<cWscAttrVersion>()) {
        TLVF_LOG(ERROR) << "getAttr<cWscAttrVersion> failed";
        return false;
    }
    if (!getAttr<cWscAttrMessageType>()) {
        TLVF_LOG(ERROR) << "getAttr<cWscAttrMessageType> failed";
        return false;
    }
    if (!getAttr<cWscAttrEnrolleeNonce>()) {
        TLVF_LOG(ERROR) << "getAttr<cWscAttrEnrolleeNonce> failed";
        return false;
    }
    if (!getAttr<cWscAttrPublicKey>()) {
        TLVF_LOG(ERROR) << "getAttr<cWscAttrPublicKey> failed";
        return false;
    }
    if (!getAttr<cWscAttrAuthenticationTypeFlags>()) {
        TLVF_LOG(ERROR) << "getAttr<cWscAttrAuthenticationTypeFlags> failed";
        return false;
    }
    if (!getAttr<cWscAttrEncryptionTypeFlags>()) {
        TLVF_LOG(ERROR) << "getAttr<cWscAttrEncryptionTypeFlags> failed";
        return false;
    }
    if (!getAttr<cWscAttrConnectionTypeFlags>()) {
        TLVF_LOG(ERROR) << "getAttr<cWscAttrConnectionTypeFlags> failed";
        return false;
    }
    if (!getAttr<cWscAttrConfigurationMethods>()) {
        TLVF_LOG(ERROR) << "getAttr<cWscAttrConfigurationMethods> failed";
        return false;
    }
    if (!getAttr<cWscAttrManufacturer>()) {
        TLVF_LOG(ERROR) << "getAttr<cWscAttrManufacturer> failed";
        return false;
    }
    if (!getAttr<cWscAttrModelName>()) {
        TLVF_LOG(ERROR) << "getAttr<cWscAttrModelName> failed";
        return false;
    }
    if (!getAttr<cWscAttrModelNumber>()) {
        TLVF_LOG(ERROR) << "getAttr<cWscAttrModelNumber> failed";
        return false;
    }
    if (!getAttr<cWscAttrSerialNumber>()) {
        TLVF_LOG(ERROR) << "getAttr<cWscAttrSerialNumber> failed";
        return false;
    }
    if (!getAttr<cWscAttrPrimaryDeviceType>()) {
        TLVF_LOG(ERROR) << "getAttr<cWscAttrPrimaryDeviceType> failed";
        return false;
    }
    if (!getAttr<cWscAttrDeviceName>()) {
        TLVF_LOG(ERROR) << "getAttr<cWscAttrDeviceName> failed";
        return false;
    }
    if (!getAttr<cWscAttrRfBands>()) {
        TLVF_LOG(ERROR) << "getAttr<cWscAttrRfBands> failed";
        return false;
    }
    if (!getAttr<cWscAttrAssociationState>()) {
        TLVF_LOG(ERROR) << "getAttr<cWscAttrAssociationState> failed";
        return false;
    }
    if (!getAttr<cWscAttrConfigurationError>()) {
        TLVF_LOG(ERROR) << "getAttr<cWscAttrConfigurationError> failed";
        return false;
    }
    if (!getAttr<cWscAttrDevicePasswordID>()) {
        TLVF_LOG(ERROR) << "getAttr<cWscAttrDevicePasswordID> failed";
        return false;
    }
    if (!getAttr<cWscAttrOsVersion>()) {
        TLVF_LOG(ERROR) << "getAttr<cWscAttrOsVersion> failed";
        return false;
    }
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
