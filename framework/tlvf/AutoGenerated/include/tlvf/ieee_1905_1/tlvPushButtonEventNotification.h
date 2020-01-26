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

#ifndef _TLVF_IEEE_1905_1_TLVPUSHBUTTONEVENTNOTIFICATION_H_
#define _TLVF_IEEE_1905_1_TLVPUSHBUTTONEVENTNOTIFICATION_H_

#include <cstddef>
#include <stdint.h>
#include <tlvf/swap.h>
#include <string.h>
#include <memory>
#include <tlvf/BaseClass.h>
#include <tlvf/ClassList.h>
#include "tlvf/ieee_1905_1/eTlvType.h"
#include <tuple>
#include "tlvf/ieee_1905_1/eMediaType.h"
#include "tlvf/ieee_1905_1/s802_11SpecificInformation.h"

namespace ieee1905_1 {


class tlvPushButtonEventNotification : public BaseClass
{
    public:
        tlvPushButtonEventNotification(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit tlvPushButtonEventNotification(std::shared_ptr<BaseClass> base, bool parse = false);
        ~tlvPushButtonEventNotification();

        typedef struct sMediaType {
            eMediaType media_type;
            uint8_t k_octets_of_media_specific_information;
            s802_11SpecificInformation media_specific_information;
            void struct_swap(){
                media_specific_information.struct_swap();
            }
            void struct_init(){
                media_specific_information.struct_init();
            }
        } __attribute__((packed)) sMediaType;
        
        const eTlvType& type();
        const uint16_t& length();
        uint8_t& media_type_list_length();
        std::tuple<bool, sMediaType&> media_type_list(size_t idx);
        bool alloc_media_type_list(size_t count = 1);
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eTlvType* m_type = nullptr;
        uint16_t* m_length = nullptr;
        uint8_t* m_media_type_list_length = nullptr;
        sMediaType* m_media_type_list = nullptr;
        size_t m_media_type_list_idx__ = 0;
        int m_lock_order_counter__ = 0;
};

}; // close namespace: ieee1905_1

#endif //_TLVF/IEEE_1905_1_TLVPUSHBUTTONEVENTNOTIFICATION_H_
