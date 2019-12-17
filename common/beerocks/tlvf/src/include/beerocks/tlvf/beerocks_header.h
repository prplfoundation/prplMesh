/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2019 Tomer Eliyahu (Intel Corporation)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _TLVF_BEEROCKS_HEADER_H_
#define _TLVF_BEEROCKS_HEADER_H_

#include <beerocks/tlvf/beerocks_message_header.h>
#include <tlvf/TlvList.h>
#include <tlvf/tlvflogging.h>

namespace beerocks {

/**
 * @brief beerocks_header
 *
 * Each vendor specific message used for internal and external communication
 * between the different components in prplMesh, via UDS or the BTL, includes
 * an action header and the actual message data.
 * These stored in the vendor specific TLV payload as a separate TLVlist.
 *
 * Note that this is not really a "header", a more appropriate name would be
 * beerocks_actions - but since the name beerocks_header is widely used, we keep
 * it for backward comparability. Same goes to all the getter functions.
 *
 */
class beerocks_header : public ieee1905_1::TlvList {
public:
    beerocks_header() = delete;
    beerocks_header(uint8_t *buff, size_t buff_len, bool parse, bool swap)
        : TlvList(buff, buff_len, parse, swap)
    {
    }
    std::shared_ptr<beerocks_message::cACTION_HEADER> actionhdr()
    {
        return getClass<beerocks_message::cACTION_HEADER>();
    }
    beerocks_message::eAction action()
    {
        return actionhdr() ? actionhdr()->action() : beerocks_message::ACTION_NONE;
    }
    uint8_t action_op() { return actionhdr() ? actionhdr()->action_op() : 0; };
    uint16_t id() { return actionhdr() ? actionhdr()->id() : 0; }
    virtual bool finalize(bool swap_needed) override
    {
        if (m_finalized)
            return true;

        for (auto &it : m_class_vector) {
            if (!(it->isPostInitSucceeded())) {
                TLVF_LOG(ERROR) << "TLV post init failed";
                return false;
            }
        }

        if (swap_needed)
            swap();

        m_finalized = true;
        return true;
    }
};

} // namespace beerocks

#endif // _TLVF_BEEROCKS_HEADER_H_
