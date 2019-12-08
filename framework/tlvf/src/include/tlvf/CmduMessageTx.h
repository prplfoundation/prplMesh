/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _CmduMessageTX_H_
#define _CmduMessageTX_H_

#include <tlvf/CmduMessage.h>
#include <tlvf/ieee_1905_1/tlvVendorSpecific.h>

namespace ieee1905_1 {

class CmduMessageTx : public CmduMessage {

public:
    CmduMessageTx(uint8_t *buff, size_t buff_len);
    ~CmduMessageTx();

    std::shared_ptr<cCmduHeader> create(uint16_t id, eMessageType message_type);
    std::shared_ptr<cCmduHeader> load();
    std::shared_ptr<tlvVendorSpecific> add_vs_tlv(tlvVendorSpecific::eVendorOUI voui);

    bool finalize(bool swap_needed);
};

}; // close namespace: ieee1905_1

#endif //_CmduMessageTX_H_
