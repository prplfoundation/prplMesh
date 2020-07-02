/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _TLVF_WSC_CONFIGDATA_H_
#define _TLVF_WSC_CONFIGDATA_H_

#include <tlvf/WSC/AttrList.h>
#include <tlvf/tlvflogging.h>

#include <iomanip>

namespace WSC {

class configData : public AttrList {

public:
    struct config {
        std::string ssid = "";
        eWscAuth auth_type;
        eWscEncr encr_type;
        std::string network_key = "";
        sMacAddr bssid          = {};
        uint8_t bss_type;
    };

    configData(uint8_t *buff, size_t buff_len, bool parse) : AttrList(buff, buff_len, parse) {}
    virtual ~configData() = default;

    bool init(const config &cfg);
    bool init() { return AttrList::init(); };
    bool valid() const override;
    static std::shared_ptr<configData> create(const config &cfg, uint8_t *buff, size_t buff_len);
    static std::shared_ptr<configData> parse(uint8_t *buff, size_t buff_len);

    // getters
    std::string ssid() const
    {
        auto attr = getAttr<cWscAttrSsid>();
        return attr ? attr->ssid_str() : std::string();
    };
    eWscAuth auth_type() const
    {
        auto attr = getAttr<cWscAttrAuthenticationType>();
        return attr ? attr->data() : eWscAuth::WSC_AUTH_INVALID;
    };
    eWscEncr encr_type() const
    {
        auto attr = getAttr<cWscAttrEncryptionType>();
        return attr ? attr->data() : eWscEncr::WSC_ENCR_INVALID;
    };
    std::string network_key() const
    {
        auto attr = getAttr<cWscAttrNetworkKey>();
        return attr ? attr->key_str() : std::string();
    };
    sMacAddr bssid() const
    {
        auto attr = getAttr<cWscAttrMac>();
        return attr ? attr->data() : sMacAddr();
    };
    eWscVendorExtSubelementBssType bss_type() const
    {
        constexpr eWscVendorExtSubelementBssType default_value =
            eWscVendorExtSubelementBssType::TEARDOWN;

        // Iterate over all Vendor Extension attributes until we find one that matches the WFA ID.
        for (auto &vendor_ext_attr : getAttrList<cWscAttrVendorExtension>()) {

            // Skip Vendor Extension attribute if it is not a WFA Vendor Extension attribute
            if ((eWscVendorId::WSC_VENDOR_ID_WFA_1 != vendor_ext_attr->vendor_id_0()) ||
                (eWscVendorId::WSC_VENDOR_ID_WFA_2 != vendor_ext_attr->vendor_id_1()) ||
                (eWscVendorId::WSC_VENDOR_ID_WFA_3 != vendor_ext_attr->vendor_id_2())) {
                continue;
            }

            size_t index     = 0;
            auto vendor_data = vendor_ext_attr->vendor_data();
            while ((index + sizeof(sWscWfaVendorExtSubelementMultiApIdentifier)) <=
                   vendor_ext_attr->vendor_data_length()) {
                auto subelement_type = static_cast<eWscWfaVendorExtSubelement>(vendor_data[index]);
                auto subelement_length = vendor_data[index + 1];
                if ((eWscWfaVendorExtSubelement::MULTI_AP_IDENTIFIER == subelement_type) &&
                    (1 == subelement_length)) {
                    return static_cast<WSC::eWscVendorExtSubelementBssType>(vendor_data[index + 2]);
                }

                // Skip this subelement
                index += 2 + subelement_length;
            }
        }

        return default_value;
    };
};

} // namespace WSC

#endif // _TLVF_WSC_CONFIGDATA_H_
