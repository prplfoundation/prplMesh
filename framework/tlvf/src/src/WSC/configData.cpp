/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include <tlvf/WSC/configData.h>
#include <tlvf/tlvfdefines.h>

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

    auto vendor_ext_attr = addAttr<cWscAttrVendorExtension>();
    if (!vendor_ext_attr) {
        TLVF_LOG(ERROR) << "addAttr<cWscAttrVendorExtension> failed";
        return false;
    }

    // WFA Vendor Data
    const size_t vendor_data_size = sizeof(sWscWfaVendorExtSubelementVersion2) +
                                    sizeof(sWscWfaVendorExtSubelementMultiApIdentifier);
    if (!vendor_ext_attr->alloc_vendor_data(vendor_data_size)) {
        LOG(ERROR) << "Failed to allocate vendor data [" << vendor_data_size << "]!";
        return false;
    }
    auto vendor_data = vendor_ext_attr->vendor_data();

    // WFA Vendor Extension Subelement at #0: Version2
    size_t index = 0;
    sWscWfaVendorExtSubelementVersion2 version2{eWscWfaVendorExtSubelement::VERSION2, 0x01,
                                                eWscVendorExtVersionIE::WSC_VERSION2};
    std::copy_n(reinterpret_cast<uint8_t *>(&version2), sizeof(version2), &vendor_data[index]);

    // WFA Vendor Extension Subelement at #1: Multi-AP Identifier
    index += sizeof(version2);
    sWscWfaVendorExtSubelementMultiApIdentifier multi_ap_identifier{
        eWscWfaVendorExtSubelement::MULTI_AP_IDENTIFIER, 0x01, cfg.bss_type};
    std::copy_n(reinterpret_cast<uint8_t *>(&multi_ap_identifier), sizeof(multi_ap_identifier),
                &vendor_data[index]);

    return true;
}

bool configData::valid() const
{
    bool valid = true;
    if (!getAttr<cWscAttrVendorExtension>()) {
        TLVF_LOG(ERROR) << "getAttr<cWscAttrVendorExtension> failed";
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
