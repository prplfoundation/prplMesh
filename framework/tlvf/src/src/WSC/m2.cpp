/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2019 Tomer Eliyahu (Intel)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include <tlvf/WSC/m2.h>
#include <tlvf/tlvfdefines.h>
#include <tlvf/tlvflogging.h>

using namespace WSC;

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

bool m2::valid_custom() const
{
    if (!getAttr<cWscAttrUuidR>()) {
        TLVF_LOG(ERROR) << "getAttr<cWscAttrUuidR> failed";
        return false;
    }
    if (!getAttr<cWscAttrRegistrarNonce>()) {
        TLVF_LOG(ERROR) << "getAttr<cWscAttrRegistrarNonce> failed";
        return false;
    }
    if (!getAttr<cWscAttrVersion2>()) {
        TLVF_LOG(ERROR) << "getAttr<cWscAttrVersion2> failed";
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
    return false;
}
