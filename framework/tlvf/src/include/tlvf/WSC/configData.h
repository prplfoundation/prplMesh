/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2020 Tomer Eliyahu (Intel Corporation)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _TLVF_WSC_CONFIGDATA_H_
#define _TLVF_WSC_CONFIGDATA_H_

#include <tlvf/WSC/AttrList.h>

namespace WSC {

class configData : public AttrList {

public:
    struct config {
        std::string ssid = "NA";
        eWscAuth auth_type;
        eWscEncr encr_type;
        std::string network_key = "NA";
        sMacAddr bssid          = {};
        eWscVendorExtSubelementBssType bss_type;
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
    uint8_t bss_type() const { return getAttr<cWscVendorExtWfa>()->subelement_value(); };
};

} // namespace WSC

#endif // _TLVF_WSC_CONFIGDATA_H_
