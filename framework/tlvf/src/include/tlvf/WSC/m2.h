/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2019 Tomer Eliyahu (Intel Corporation)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _TLVF_WSC_M2_H_
#define _TLVF_WSC_M2_H_

#include <tlvf/WSC/AttrList.h>

namespace WSC {

class m2 : public AttrList {
public:
    m2(uint8_t *buff, size_t buff_len, bool parse) : AttrList(buff, buff_len, parse) {}
    virtual ~m2() = default;

    bool init(const config &cfg) override;
    bool valid_custom() const override;

    // getters
    eWscMessageType msg_type() const { return getAttr<cWscAttrMessageType>()->msg_type(); };
    std::string manufacturer() const
    {
        return getAttr<cWscAttrManufacturer>()->manufacturer_str();
    };
    std::string model_name() const { return getAttr<cWscAttrModelName>()->model_str(); };
    std::string device_name() const { return getAttr<cWscAttrDeviceName>()->device_name_str(); };
    std::string serial_number() const
    {
        return getAttr<cWscAttrSerialNumber>()->serial_number_str();
    };
    uint16_t encr_type_flags() const
    {
        return getAttr<cWscAttrEncryptionTypeFlags>()->encr_type_flags();
    };
    uint16_t auth_type_flags() const
    {
        return getAttr<cWscAttrAuthenticationTypeFlags>()->auth_type_flags();
    };
    uint8_t *enrollee_nonce() { return getAttr<cWscAttrEnrolleeNonce>()->nonce(); };
    uint8_t *public_key() { return getAttr<cWscAttrPublicKey>()->public_key(); };
    uint16_t rf_bands() const { return getAttr<cWscAttrRfBands>()->bands(); };

    uint8_t *authenticator() { return getAttr<cWscAttrAuthenticator>()->data(); };
    uint8_t *registrar_nonce() { return getAttr<cWscAttrRegistrarNonce>()->nonce(); };
    cWscAttrEncryptedSettings &encrypted_settings()
    {
        return *getAttr<cWscAttrEncryptedSettings>();
    };
};

} // namespace WSC

#endif // _TLVF_WSC_M2_H_
