///////////////////////////////////////
// AUTO GENERATED FILE - DO NOT EDIT //
///////////////////////////////////////

/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _TLVF_BEEROCKS_MESSAGE_CONTROL_H_
#define _TLVF_BEEROCKS_MESSAGE_CONTROL_H_

#include <cstddef>
#include <stdint.h>
#include <tlvf/swap.h>
#include <string.h>
#include <memory>
#include <tlvf/BaseClass.h>
#include <tuple>
//~include_insert
//~namespace_insert
//~forward_declaration_insert
//~code_start_insert

class cACTION_CONTROL_AGENT_PING_RESPONSE : public BaseClass
{
    //~class_start_cACTION_CONTROL_AGENT_PING_RESPONSE
    public:
        cACTION_CONTROL_AGENT_PING_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_CONTROL_AGENT_PING_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        //~class_constractor_cACTION_CONTROL_AGENT_PING_RESPONSE
        ~cACTION_CONTROL_AGENT_PING_RESPONSE();
        //~class_distractor_cACTION_CONTROL_AGENT_PING_RESPONSE

        //~class_public_vars_insert_cACTION_CONTROL_AGENT_PING_RESPONSE
        //~class_public_vars_end_insert_cACTION_CONTROL_AGENT_PING_RESPONSE
        uint16_t& total();
        uint16_t& seq();
        uint16_t& size();
        std::tuple<bool, uint8_t&> data(size_t idx);
        bool alloc_data(size_t count = 1);
        //~class_public_func_insert_cACTION_CONTROL_AGENT_PING_RESPONSE
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        //~class_private_func_insert_cACTION_CONTROL_AGENT_PING_RESPONSE
        uint16_t* m_total = nullptr;
        uint16_t* m_seq = nullptr;
        uint16_t* m_size = nullptr;
        uint8_t* m_data = nullptr;
        size_t m_data_idx__ = 0;
        //~class_private_vars_insert_cACTION_CONTROL_AGENT_PING_RESPONSE
    //~class_end_cACTION_CONTROL_AGENT_PING_RESPONSE
};
//~code_end_insert

#endif //_TLVF_BEEROCKS_MESSAGE_CONTROL_H_
