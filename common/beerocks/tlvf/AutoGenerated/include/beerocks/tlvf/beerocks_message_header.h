///////////////////////////////////////
// AUTO GENERATED FILE - DO NOT EDIT //
///////////////////////////////////////

/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _BEEROCKS_TLVF_BEEROCKS_MESSAGE_HEADER_H_
#define _BEEROCKS_TLVF_BEEROCKS_MESSAGE_HEADER_H_

#include <cstddef>
#include <stdint.h>
#include <tlvf/swap.h>
#include <string.h>
#include <memory>
#include <tlvf/BaseClass.h>
#include <tlvf/ClassList.h>
#include "beerocks/tlvf/beerocks_message_common.h"
#include "beerocks/tlvf/beerocks_message_action.h"

namespace beerocks_message {


class cACTION_HEADER : public BaseClass
{
    public:
        cACTION_HEADER(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cACTION_HEADER(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cACTION_HEADER();

        const uint32_t& magic();
        const uint8_t& version();
        eAction& action();
        //need to cast eActionOp_XXXX to uint8_t
        uint8_t& action_op();
        uint8_t& direction();
        sMacAddr& radio_mac();
        uint8_t& last();
        uint16_t& id();
        uint16_t& length();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        uint32_t* m_magic = nullptr;
        uint8_t* m_version = nullptr;
        eAction* m_action = nullptr;
        uint8_t* m_action_op = nullptr;
        uint8_t* m_direction = nullptr;
        sMacAddr* m_radio_mac = nullptr;
        uint8_t* m_last = nullptr;
        uint16_t* m_id = nullptr;
        uint16_t* m_length = nullptr;
};

}; // close namespace: beerocks_message

#endif //_BEEROCKS/TLVF_BEEROCKS_MESSAGE_HEADER_H_
