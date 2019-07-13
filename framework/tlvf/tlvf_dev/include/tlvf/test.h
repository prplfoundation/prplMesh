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
//~include_insert

namespace ieee1905_1 {

//~namespace_insert
//~forward_declaration_insert
//~code_start_insert

class tlvVendorSpecific : public BaseClass
{
    //~class_start_tlvVendorSpecific
    public:
        tlvVendorSpecific(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        tlvVendorSpecific(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        //~class_constractor_tlvVendorSpecific
        ~tlvVendorSpecific();
        //~class_distractor_tlvVendorSpecific

        //~class_public_vars_insert_tlvVendorSpecific
        //~class_public_vars_end_insert_tlvVendorSpecific
        const uint8_t& type();
        uint16_t& length();
        uint16_t& test_length();
        std::tuple<bool, uint8_t&> test_list(size_t idx);
        bool alloc_test_list(size_t count = 1);
        uint16_t& test_uint16();
        uint16_t& test2_length();
        std::tuple<bool, uint8_t&> test2_list(size_t idx);
        bool alloc_test2_list(size_t count = 1);
        uint32_t& test_uint32();
        //Use eVendorOUI
        std::tuple<bool, uint8_t&> vendor_oui(size_t idx);
        //~class_public_func_insert_tlvVendorSpecific
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        //~class_private_func_insert_tlvVendorSpecific
        uint8_t* m_type = nullptr;
        uint16_t* m_length = nullptr;
        uint16_t* m_test_length = nullptr;
        uint8_t* m_test_list = nullptr;
        size_t m_test_list_idx__ = 0;
        uint16_t* m_test_uint16 = nullptr;
        uint16_t* m_test2_length = nullptr;
        uint8_t* m_test2_list = nullptr;
        size_t m_test2_list_idx__ = 0;
        uint32_t* m_test_uint32 = nullptr;
        uint8_t* m_vendor_oui = nullptr;
        size_t m_vendor_oui_idx__ = 0;
        //~class_private_vars_insert_tlvVendorSpecific
    //~class_end_tlvVendorSpecific
};
//~code_end_insert

}; // close namespace: ieee1905_1

#endif //_TLVF_TEST_H_
