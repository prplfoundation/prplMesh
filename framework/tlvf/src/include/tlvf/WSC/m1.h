/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2019 Tomer Eliyahu (Intel Corporation)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _TLVF_WSC_M1_H_
#define _TLVF_WSC_M1_H_

#include <tlvf/WSC/AttrList.h>

namespace WSC {

class m1 : public AttrList {

public:
    m1(uint8_t *buff, size_t buff_len, bool parse) : AttrList(buff, buff_len, parse) {}
    virtual ~m1() = default;

    bool init(const config &cfg) override;
    bool valid_custom() const override;

    // getters
    sMacAddr mac_addr() const { return getAttr<cWscAttrMac>()->data(); };
};

} // namespace WSC

#endif // _TLVF_WSC_M1_H_
