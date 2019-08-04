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

        const uint16_t& type();
        const uint16_t& length();
        uint16_t& var0();
        uint8_t& simple_list_length();
        std::tuple<bool, uint16_t&> simple_list(size_t idx);
        bool alloc_simple_list(size_t count = 1);
        uint8_t& complex_list_length();
        std::tuple<bool, cInner&> complex_list(size_t idx);
        std::shared_ptr<cInner> create_complex_list();
        bool add_complex_list(std::shared_ptr<cInner> ptr);
        std::shared_ptr<cInner> create_var1();
        bool add_var1(std::shared_ptr<cInner> ptr);
        std::shared_ptr<cInner> var1() { return m_var1_ptr; }
        size_t unknown_length_list_length();
        std::tuple<bool, cInner&> unknown_length_list(size_t idx);
        std::shared_ptr<cInner> create_unknown_length_list();
        bool add_unknown_length_list(std::shared_ptr<cInner> ptr);
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        uint16_t* m_type = nullptr;
        uint16_t* m_length = nullptr;
        uint16_t* m_var0 = nullptr;
        uint8_t* m_simple_list_length = nullptr;
        uint16_t* m_simple_list = nullptr;
        size_t m_simple_list_idx__ = 0;
        int m_lock_order_counter__ = 0;
        uint8_t* m_complex_list_length = nullptr;
        cInner* m_complex_list = nullptr;
        size_t m_complex_list_idx__ = 0;
        std::vector<std::shared_ptr<cInner>> m_complex_list_vector;
        bool m_lock_allocation__ = false;
        cInner *m_var1 = nullptr;
        std::shared_ptr<cInner> m_var1_ptr = nullptr;
        cInner* m_unknown_length_list = nullptr;
        size_t m_unknown_length_list_idx__ = 0;
        std::vector<std::shared_ptr<cInner>> m_unknown_length_list_vector;
};

class cInner : public BaseClass
{
    public:
        cInner(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cInner(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cInner();

        uint8_t& list_length();
        std::tuple<bool, uint8_t&> list(size_t idx);
        bool alloc_list(size_t count = 1);
        uint32_t& var1();
        size_t unknown_length_list_length() { return m_unknown_length_list_idx__ * sizeof(uint8_t); }
        std::tuple<bool, uint8_t&> unknown_length_list(size_t idx);
        bool alloc_unknown_length_list(size_t count = 1);
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        uint8_t* m_list_length = nullptr;
        uint8_t* m_list = nullptr;
        size_t m_list_idx__ = 0;
        int m_lock_order_counter__ = 0;
        uint32_t* m_var1 = nullptr;
        uint8_t* m_unknown_length_list = nullptr;
        size_t m_unknown_length_list_idx__ = 0;
};

#endif //_TLVF/TEST_TLVVARLIST_H_
