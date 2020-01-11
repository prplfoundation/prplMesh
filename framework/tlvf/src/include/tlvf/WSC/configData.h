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
    std::string ssid() const { return getAttr<cWscAttrSsid>()->ssid_str(); };
    eWscAuth auth_type() const { return getAttr<cWscAttrAuthenticationType>()->data(); };
    eWscEncr encr_type() const { return getAttr<cWscAttrEncryptionType>()->data(); };
    std::string network_key() const { return getAttr<cWscAttrNetworkKey>()->key_str(); };
    sMacAddr bssid() const { return getAttr<cWscAttrMac>()->data(); };
    uint8_t bss_type() const { return getAttr<cWscVendorExtWfa>()->subelement_value(); };
};

} // namespace WSC

#endif // _TLVF_WSC_CONFIGDATA_H_
