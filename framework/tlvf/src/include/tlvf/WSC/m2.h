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
    uint8_t *authenticator() { return getAttr<cWscAttrAuthenticator>()->data(); };
    uint8_t *registrar_nonce() { return getAttr<cWscAttrRegistrarNonce>()->nonce(); };
    cWscAttrEncryptedSettings &encrypted_settings()
    {
        return *getAttr<cWscAttrEncryptedSettings>();
    };
};

} // namespace WSC

#endif // _TLVF_WSC_M2_H_
