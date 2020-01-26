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

#ifndef _TLVF_IEEE_1905_1_TLVDEVICEINFORMATION_H_
#define _TLVF_IEEE_1905_1_TLVDEVICEINFORMATION_H_

#include <cstddef>
#include <stdint.h>
#include <tlvf/swap.h>
#include <string.h>
#include <memory>
#include <tlvf/BaseClass.h>
#include <tlvf/ClassList.h>
#include "tlvf/ieee_1905_1/eTlvType.h"
#include "tlvf/common/sMacAddr.h"
#include <tuple>
#include "tlvf/ieee_1905_1/eMediaType.h"

namespace ieee1905_1 {


class tlvDeviceInformation : public BaseClass
{
    public:
        tlvDeviceInformation(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit tlvDeviceInformation(std::shared_ptr<BaseClass> base, bool parse = false);
        ~tlvDeviceInformation();

        typedef struct sInfo {
            sMacAddr mac;
            eMediaType media_type;
            uint8_t media_info_length;
            eMediaType media_info;
            void struct_swap(){
                mac.struct_swap();
            }
            void struct_init(){
                mac.struct_init();
            }
        } __attribute__((packed)) sInfo;
        
        const eTlvType& type();
        const uint16_t& length();
        sMacAddr& mac();
        uint8_t& info_length();
        std::tuple<bool, sInfo&> info(size_t idx);
        bool alloc_info(size_t count = 1);
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eTlvType* m_type = nullptr;
        uint16_t* m_length = nullptr;
        sMacAddr* m_mac = nullptr;
        uint8_t* m_info_length = nullptr;
        sInfo* m_info = nullptr;
        size_t m_info_idx__ = 0;
        int m_lock_order_counter__ = 0;
};

}; // close namespace: ieee1905_1

#endif //_TLVF/IEEE_1905_1_TLVDEVICEINFORMATION_H_
