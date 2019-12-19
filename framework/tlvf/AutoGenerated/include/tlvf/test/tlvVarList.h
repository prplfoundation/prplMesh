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
#include <tlvf/tlvfutils.h>
#include <vector>
class cInner;

class tlvTestVarList : public BaseClass
{
    public:
        tlvTestVarList(uint8_t* buff, size_t buff_len, bool parse = false);
        tlvTestVarList(std::shared_ptr<BaseClass> base, bool parse = false);
        ~tlvTestVarList();

        const uint8_t& type();
        const uint16_t& length();
        uint16_t& var0();
        uint8_t& simple_list_length();
        uint16_t* simple_list(size_t idx = 0);
        bool alloc_simple_list(size_t count = 1);
        uint8_t& test_string_length();
        std::string test_string_str();
        char* test_string(size_t length = 0);
        bool set_test_string(const std::string& str);
        bool set_test_string(const char buffer[], size_t size);
        bool alloc_test_string(size_t count = 1);
        uint8_t& complex_list_length();
        std::tuple<bool, cInner&> complex_list(size_t idx);
        std::shared_ptr<cInner> create_complex_list();
        bool add_complex_list(std::shared_ptr<cInner> ptr);
        bool isPostInitSucceeded();
        std::shared_ptr<cInner> create_var1();
        bool add_var1(std::shared_ptr<cInner> ptr);
        std::shared_ptr<cInner> var1() { return m_var1_ptr; }
        std::shared_ptr<cInner> create_var3();
        bool add_var3(std::shared_ptr<cInner> ptr);
        std::shared_ptr<cInner> var3() { return m_var3_ptr; }
        uint32_t& var2();
        size_t unknown_length_list_length();
        std::tuple<bool, cInner&> unknown_length_list(size_t idx);
        std::shared_ptr<cInner> create_unknown_length_list();
        bool add_unknown_length_list(std::shared_ptr<cInner> ptr);
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        uint8_t* m_type = nullptr;
        uint16_t* m_length = nullptr;
        uint16_t* m_var0 = nullptr;
        uint8_t* m_simple_list_length = nullptr;
        uint16_t* m_simple_list = nullptr;
        size_t m_simple_list_idx__ = 0;
        int m_lock_order_counter__ = 0;
        uint8_t* m_test_string_length = nullptr;
        char* m_test_string = nullptr;
        size_t m_test_string_idx__ = 0;
        uint8_t* m_complex_list_length = nullptr;
        cInner* m_complex_list = nullptr;
        size_t m_complex_list_idx__ = 0;
        std::vector<std::shared_ptr<cInner>> m_complex_list_vector;
        bool m_lock_allocation__ = false;
        cInner *m_var1 = nullptr;
        std::shared_ptr<cInner> m_var1_ptr = nullptr;
        bool m_var1_init = false;
        cInner *m_var3 = nullptr;
        std::shared_ptr<cInner> m_var3_ptr = nullptr;
        bool m_var3_init = false;
        uint32_t* m_var2 = nullptr;
        cInner* m_unknown_length_list = nullptr;
        size_t m_unknown_length_list_idx__ = 0;
        std::vector<std::shared_ptr<cInner>> m_unknown_length_list_vector;
};

class cInner : public BaseClass
{
    public:
        cInner(uint8_t* buff, size_t buff_len, bool parse = false);
        cInner(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cInner();

        const uint16_t& type();
        const uint16_t& length();
        uint8_t& list_length();
        uint8_t* list(size_t idx = 0);
        bool alloc_list(size_t count = 1);
        uint32_t& var1();
        size_t unknown_length_list_inner_length() { return m_unknown_length_list_inner_idx__ * sizeof(char); }
        std::string unknown_length_list_inner_str();
        char* unknown_length_list_inner(size_t length = 0);
        bool set_unknown_length_list_inner(const std::string& str);
        bool set_unknown_length_list_inner(const char buffer[], size_t size);
        bool alloc_unknown_length_list_inner(size_t count = 1);
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        uint16_t* m_type = nullptr;
        uint16_t* m_length = nullptr;
        uint8_t* m_list_length = nullptr;
        uint8_t* m_list = nullptr;
        size_t m_list_idx__ = 0;
        int m_lock_order_counter__ = 0;
        uint32_t* m_var1 = nullptr;
        char* m_unknown_length_list_inner = nullptr;
        size_t m_unknown_length_list_inner_idx__ = 0;
};

#endif //_TLVF/TEST_TLVVARLIST_H_
