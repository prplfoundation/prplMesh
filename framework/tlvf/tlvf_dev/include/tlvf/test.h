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

#ifndef _TLVF_TEST_H_
#define _TLVF_TEST_H_

#include <cstddef>
#include <stdint.h>
#include <tlvf/swap.h>
#include <string.h>
#include <memory>
#include <tlvf/BaseClass.h>
#include <tuple>
#include <vector>
#include <tlvf/tlvfutils.h>
//~include_insert

namespace ieee1905_1 {

//~namespace_insert
class cOuterClass;
//~forward_declaration_insert
//~code_start_insert

class cInnerClass : public BaseClass
{
    //~class_start_cInnerClass
    public:
        cInnerClass(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cInnerClass(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        //~class_constractor_cInnerClass
        ~cInnerClass();
        //~class_distractor_cInnerClass

        //~class_public_vars_insert_cInnerClass
        //~class_public_vars_end_insert_cInnerClass
        const uint8_t& var1();
        uint16_t& list1_length();
        std::tuple<bool, uint8_t&> list1(size_t idx);
        bool alloc_list1(size_t count = 1);
        uint16_t& var2();
        uint16_t& list2_length();
        std::tuple<bool, uint8_t&> list2(size_t idx);
        bool alloc_list2(size_t count = 1);
        uint32_t& var3();
        std::tuple<bool, uint8_t&> var4(size_t idx);
        //~class_public_func_insert_cInnerClass
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        //~class_private_func_insert_cInnerClass
        uint8_t* m_var1 = nullptr;
        uint16_t* m_list1_length = nullptr;
        uint8_t* m_list1 = nullptr;
        size_t m_list1_idx__ = 0;
        uint16_t* m_var2 = nullptr;
        uint16_t* m_list2_length = nullptr;
        uint8_t* m_list2 = nullptr;
        size_t m_list2_idx__ = 0;
        uint32_t* m_var3 = nullptr;
        uint8_t* m_var4 = nullptr;
        size_t m_var4_idx__ = 0;
        //~class_private_vars_insert_cInnerClass
    //~class_end_cInnerClass
};

class cOuterClass : public BaseClass
{
    //~class_start_cOuterClass
    public:
        cOuterClass(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cOuterClass(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        //~class_constractor_cOuterClass
        ~cOuterClass();
        //~class_distractor_cOuterClass

        //~class_public_vars_insert_cOuterClass
        //~class_public_vars_end_insert_cOuterClass
        const uint32_t& var1();
        uint16_t& var2_length();
        std::tuple<bool, cInnerClass&> var2_list(size_t idx);
        std::shared_ptr<cInnerClass> create_var2_list();
        bool add_var2_list(std::shared_ptr<cInnerClass> ptr);
        uint16_t& var3_length();
        std::tuple<bool, uint8_t&> var3_list(size_t idx);
        bool alloc_var3_list(size_t count = 1);
        char* list_dynamic(size_t length = 0);
        bool set_list_dynamic(std::string& str);
        bool set_list_dynamic(const std::string& str);
        bool set_list_dynamic(char buffer[], size_t size);
        bool alloc_list_dynamic(size_t count = 1);
        //~class_public_func_insert_cOuterClass
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        //~class_private_func_insert_cOuterClass
        uint32_t* m_var1 = nullptr;
        uint16_t* m_var2_length = nullptr;
        cInnerClass* m_var2_list = nullptr;
        size_t m_var2_list_idx__ = 0;
        std::vector<std::shared_ptr<cInnerClass>> m_var2_list_vector;
        bool m_lock_allocation__ = false;
        uint16_t* m_var3_length = nullptr;
        uint8_t* m_var3_list = nullptr;
        size_t m_var3_list_idx__ = 0;
        char* m_list_dynamic = nullptr;
        size_t m_list_dynamic_idx__ = 0;
        //~class_private_vars_insert_cOuterClass
    //~class_end_cOuterClass
};
//~code_end_insert

}; // close namespace: ieee1905_1

#endif //_TLVF_TEST_H_
