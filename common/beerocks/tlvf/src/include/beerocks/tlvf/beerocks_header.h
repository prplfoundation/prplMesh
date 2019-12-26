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
#include <tlvf/ClassList.h>

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
class beerocks_header : public ClassList {
public:
    beerocks_header() = delete;
    beerocks_header(uint8_t *buff, size_t buff_len, bool parse) : ClassList(buff, buff_len, parse)
    {
    }
    std::shared_ptr<beerocks_message::cACTION_HEADER> actionhdr()
    {
        return getClass<beerocks_message::cACTION_HEADER>();
    }
    beerocks_message::eAction action()
    {
        auto action_hdr = actionhdr();
        if(!action_hdr){
            return beerocks_message::ACTION_NONE;
        }else{
            return action_hdr->action();
        } 
    }
    uint8_t action_op()
    {
        auto action_hdr = actionhdr();
        if(!action_hdr){
            return 0;
        }else{
            return action_hdr->action_op();
        }         
    };
    uint16_t id() 
    {
        auto action_hdr = actionhdr();
        if(!action_hdr){
            return 0;
        }else{
            return action_hdr->id();
        }         
    }
};

} // namespace beerocks

#endif // _TLVF_BEEROCKS_HEADER_H_
