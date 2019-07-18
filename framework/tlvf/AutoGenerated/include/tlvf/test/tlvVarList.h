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

#ifndef _TLVF_TEST_TLVVARLIST_H_
#define _TLVF_TEST_TLVVARLIST_H_

#include <cstddef>
#include <stdint.h>
#include <tlvf/swap.h>
#include <string.h>
#include <memory>
#include <tlvf/BaseClass.h>

class tlvTestVarList : public BaseClass
{
    public:
        tlvTestVarList(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        tlvTestVarList(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~tlvTestVarList();

        const uint8_t& type();
        uint16_t& var0();
        const uint16_t& length();
        uint16_t& var1();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        uint8_t* m_type = nullptr;
        uint16_t* m_var0 = nullptr;
        uint16_t* m_length = nullptr;
        uint16_t* m_var1 = nullptr;
};

#endif //_TLVF/TEST_TLVVARLIST_H_
