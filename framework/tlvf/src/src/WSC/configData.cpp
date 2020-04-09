/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include <tlvf/WSC/configData.h>
#include <tlvf/tlvfdefines.h>
#include <tlvf/tlvflogging.h>

using namespace WSC;

std::shared_ptr<configData> configData::parse(uint8_t *buff, size_t buff_len)
{
    auto attributes = std::make_shared<configData>(buff, buff_len, true);
    if (!attributes) {
        TLVF_LOG(ERROR) << "Failed to initialize attributes";
        return nullptr;
    }
    attributes->init();
    if (!attributes->valid()) {
        TLVF_LOG(ERROR) << "Not all attributes present";
        return nullptr;
    }
    return attributes;
}

std::shared_ptr<configData> configData::create(const config &cfg, uint8_t *buff, size_t buff_len)
{
    auto attributes = std::make_shared<configData>(buff, buff_len, false);
    if (!attributes || !attributes->init(cfg)) {
        TLVF_LOG(ERROR) << "Failed to initialize attributes";
        return nullptr;
    }
    if (!attributes->finalize()) {
        TLVF_LOG(ERROR) << "Failed to finalize attributes";
        return nullptr;
    }
    return attributes;
}

bool configData::init(const config &cfg)
{
    if (m_parse) {
        TLVF_LOG(ERROR) << "init(cfg) called but m_parse is set!";
        return false; // Used for create only
    }

    auto ssid = addAttr<cWscAttrSsid>();
    if (!ssid || !ssid->set_ssid(cfg.ssid)) {
        TLVF_LOG(ERROR) << "addAttr<cWscAttrSsid> failed";
        return false;
    }

    auto auth_type_attr = addAttr<cWscAttrAuthenticationType>();
    if (!auth_type_attr) {
        TLVF_LOG(ERROR) << "addAttr<cWscAttrAuthenticationType> failed";
        return false;
    }
    auth_type_attr->data() = cfg.auth_type;

    auto enc_type_attr = addAttr<cWscAttrEncryptionType>();
    if (!enc_type_attr) {
        TLVF_LOG(ERROR) << "addAttr<cWscAttrEncryptionType> failed";
        return false;
    }
    enc_type_attr->data() = cfg.encr_type;

    auto network_key_attr = addAttr<cWscAttrNetworkKey>();
    if (!network_key_attr || !network_key_attr->set_key(cfg.network_key)) {
        TLVF_LOG(ERROR) << "addAttr<cWscAttrNetworkKey> failed";
        return false;
    }

    auto bssid_attr = addAttr<cWscAttrMac>();
    if (!bssid_attr) {
        TLVF_LOG(ERROR) << "addAttr<cWscAttrMac> failed";
        return false;
    }
    bssid_attr->data() = cfg.bssid;

    auto vendor_ext_attr = addAttr<cWscVendorExtWfa>();
    if (!vendor_ext_attr) {
        TLVF_LOG(ERROR) << "addAttr<cWscVendorExtWfa> failed";
        return false;
    }
    vendor_ext_attr->subelement_value() = cfg.bss_type;

    return true;
}

bool configData::valid() const
{
    bool valid = true;
    if (!getAttr<cWscVendorExtWfa>()) {
        TLVF_LOG(ERROR) << "getAttr<cWscVendorExtWfa> failed";
        valid = false;
    }
    if (!getAttr<cWscAttrSsid>()) {
        TLVF_LOG(ERROR) << "getAttr<cWscAttrSsid> failed";
        valid = false;
    }
    if (!getAttr<cWscAttrAuthenticationType>()) {
        TLVF_LOG(ERROR) << "getAttr<cWscAttrAuthenticationType> failed";
        valid = false;
    }
    if (!getAttr<cWscAttrEncryptionType>()) {
        TLVF_LOG(ERROR) << "getAttr<cWscAttrEncryptionType> failed";
        valid = false;
    }
    if (!getAttr<cWscAttrNetworkKey>()) {
        TLVF_LOG(ERROR) << "getAttr<cWscAttrNetworkKey> failed";
        valid = false;
    }
    if (!getAttr<cWscAttrMac>()) {
        TLVF_LOG(ERROR) << "getAttr<cWscAttrMac> failed";
        valid = false;
    }
    return valid;
}
