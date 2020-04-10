/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2019 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include <tlvf/WSC/m2.h>
#include <tlvf/tlvfdefines.h>
#include <tlvf/tlvflogging.h>

using namespace WSC;

std::shared_ptr<m2> m2::parse(ieee1905_1::tlvWsc &tlv)
{
    if (!tlv.payload_length()) {
        TLVF_LOG(ERROR) << "No room to add attribute list (payload length = 0)";
        return nullptr;
    }
    auto attributes = std::make_shared<m2>(tlv.payload(), tlv.payload_length(), true);
    if (!attributes) {
        TLVF_LOG(ERROR) << "Failed to initialize attributes";
        return nullptr;
    }
    attributes->init();
    if (attributes->msg_type() != WSC_MSG_TYPE_M2) {
        TLVF_LOG(INFO) << "Not M2, msg type is " << int(attributes->msg_type());
        return nullptr;
    }
    if (!attributes->valid()) {
        TLVF_LOG(ERROR) << "Not all attributes present";
        return nullptr;
    }
    return attributes;
}

std::shared_ptr<m2> m2::create(ieee1905_1::tlvWsc &tlv, const config &cfg)
{
    if (cfg.msg_type != eWscMessageType::WSC_MSG_TYPE_M2)
        return nullptr;
    auto attributes = std::make_shared<m2>(tlv.payload(), tlv.payload_length(), false);
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

bool m2::init(const config &cfg)
{
    if (m_parse)
        return false; // Used for create only

    auto version = addAttr<cWscAttrVersion>();
    if (!version) {
        TLVF_LOG(ERROR) << "addAttr<cWscAttrVersion> failed";
        return false;
    }

    if (cfg.msg_type != eWscMessageType::WSC_MSG_TYPE_M2) {
        TLVF_LOG(ERROR) << "Invalid message type " << cfg.msg_type;
        return false;
    }
    auto msg_type_attr = addAttr<cWscAttrMessageType>();
    if (!msg_type_attr) {
        TLVF_LOG(ERROR) << "addAttr<cWscAttrMessageType> failed";
        return false;
    }
    msg_type_attr->msg_type() = cfg.msg_type;

    auto enrollee_nonce_attr = addAttr<cWscAttrEnrolleeNonce>();
    if (!enrollee_nonce_attr) {
        TLVF_LOG(ERROR) << "addAttr<cWscAttrEnrolleeNonce> failed";
        return false;
    }
    std::copy_n(cfg.enrollee_nonce, WSC_NONCE_LENGTH, enrollee_nonce_attr->nonce());

    auto registrar_nonce_attr = addAttr<cWscAttrRegistrarNonce>();
    if (!registrar_nonce_attr)
        return false;
    std::copy_n(cfg.registrar_nonce, WSC_NONCE_LENGTH, registrar_nonce_attr->nonce());

    auto uuidr_attr = addAttr<cWscAttrUuidR>();
    if (!uuidr_attr) {
        TLVF_LOG(ERROR) << "addAttr<cWscAttrUuidR> failed";
        return false;
    }
    std::memset(uuidr_attr->data(), 0xee, uuidr_attr->length());

    auto public_key_attr = addAttr<cWscAttrPublicKey>();
    if (!public_key_attr) {
        TLVF_LOG(ERROR) << "addAttr<cWscAttrPublicKey> failed";
        return false;
    }
    std::copy(cfg.pub_key, cfg.pub_key + WSC_PUBLIC_KEY_LENGTH, public_key_attr->public_key());

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

    auto manufacturer_attr = addAttr<cWscAttrManufacturer>();
    if (!manufacturer_attr || !manufacturer_attr->set_manufacturer(cfg.manufacturer)) {
        TLVF_LOG(ERROR) << "Failed setting cWscAttrManufacturer";
        return false;
    }
    auto model_attr = addAttr<cWscAttrModelName>();
    if (!model_attr || !model_attr->set_model(cfg.model_name)) {
        TLVF_LOG(ERROR) << "Failed setting cWscAttrModelName";
        return false;
    }

    auto model_number_attr = addAttr<cWscAttrModelNumber>();
    if (!model_number_attr || !model_number_attr->set_model_number(cfg.model_number)) {
        TLVF_LOG(ERROR) << "Failed setting cWscAttrModelNumber";
        return false;
    }

    auto serial_number_attr = addAttr<cWscAttrSerialNumber>();
    if (!serial_number_attr || !serial_number_attr->set_serial_number(cfg.serial_number)) {
        TLVF_LOG(ERROR) << "Failed setting cWscAttrSerialNumber";
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
        TLVF_LOG(ERROR) << "Failed setting cWscAttrDeviceName";
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

    auto config_err_attr = addAttr<cWscAttrConfigurationError>();
    if (!config_err_attr) {
        TLVF_LOG(ERROR) << "addAttr<cWscAttrConfigurationError> failed";
        return false;
    }

    auto dev_pass_attr = addAttr<cWscAttrDevicePasswordID>();
    if (!dev_pass_attr) {
        TLVF_LOG(ERROR) << "addAttr<cWscAttrDevicePasswordID> failed";
        return false;
    }

    auto os_version_attr = addAttr<cWscAttrOsVersion>();
    if (!os_version_attr) {
        TLVF_LOG(ERROR) << "addAttr<cWscAttrOsVersion> failed";
        return false;
    }

    auto version2_attr = addAttr<cWscAttrVersion2>();
    if (!version2_attr) {
        TLVF_LOG(ERROR) << "addAttr<cWscAttrVersion2> failed";
        return false;
    }

    auto encrypted_settings = addAttr<cWscAttrEncryptedSettings>();
    if (!encrypted_settings) {
        TLVF_LOG(ERROR) << "addAttr<cWscAttrEncryptedSettings> failed";
        return false;
    }
    std::copy_n(cfg.iv, WSC_ENCRYPTED_SETTINGS_IV_LENGTH, encrypted_settings->iv());
    encrypted_settings->alloc_encrypted_settings(cfg.encrypted_settings.size());
    std::copy_n(cfg.encrypted_settings.data(), encrypted_settings->encrypted_settings_length(),
                encrypted_settings->encrypted_settings());

    auto authenticator = addAttr<cWscAttrAuthenticator>();
    if (!authenticator) {
        TLVF_LOG(ERROR) << "addAttr<cWscAttrAuthenticator> failed";
        return false;
    }

    return true;
}

bool m2::valid() const
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
    if (!getAttr<cWscAttrUuidR>()) {
        TLVF_LOG(ERROR) << "getAttr<cWscAttrUuidR> failed";
        return false;
    }
    if (!getAttr<cWscAttrRegistrarNonce>()) {
        TLVF_LOG(ERROR) << "getAttr<cWscAttrRegistrarNonce> failed";
        return false;
    }
    if (!getAttr<cWscAttrEncryptedSettings>()) {
        TLVF_LOG(ERROR) << "getAttr<cWscAttrEncryptedSettings> failed";
        return false;
    }
    if (!getAttr<cWscAttrAuthenticator>()) {
        TLVF_LOG(ERROR) << "getAttr<cWscAttrAuthenticator> failed";
        return false;
    }
    return true;
}
