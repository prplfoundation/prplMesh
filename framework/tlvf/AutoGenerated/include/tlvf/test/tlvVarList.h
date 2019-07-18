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
#include <tuple>
#include <vector>
class cInner;

class tlvTestVarList : public BaseClass
{
    public:
        tlvTestVarList(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        tlvTestVarList(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~tlvTestVarList();

        const uint8_t& type();
        uint16_t& var0();
        const uint16_t& length();
        uint16_t& simple_list_length();
        std::tuple<bool, uint16_t&> simple_list(size_t idx);
        bool alloc_simple_list(size_t count = 1);
        uint16_t& complex_list_length();
        std::tuple<bool, cInner&> complex_list(size_t idx);
        std::shared_ptr<cInner> create_complex_list();
        bool add_complex_list(std::shared_ptr<cInner> ptr);
        uint16_t& var1();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        uint8_t* m_type = nullptr;
        uint16_t* m_var0 = nullptr;
        uint16_t* m_length = nullptr;
        uint16_t* m_simple_list_length = nullptr;
        uint16_t* m_simple_list = nullptr;
        size_t m_simple_list_idx__ = 0;
        uint16_t* m_complex_list_length = nullptr;
        cInner* m_complex_list = nullptr;
        size_t m_complex_list_idx__ = 0;
        std::vector<std::shared_ptr<cInner>> m_complex_list_vector;
        bool m_lock_allocation__ = false;
        uint16_t* m_var1 = nullptr;
};

class cInner : public BaseClass
{
    public:
        cInner(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cInner(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cInner();

        uint16_t& list_length();
        std::tuple<bool, uint8_t&> list(size_t idx);
        bool alloc_list(size_t count = 1);
        uint32_t& var1();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        uint16_t* m_list_length = nullptr;
        uint8_t* m_list = nullptr;
        size_t m_list_idx__ = 0;
        uint32_t* m_var1 = nullptr;
};

#endif //_TLVF/TEST_TLVVARLIST_H_
