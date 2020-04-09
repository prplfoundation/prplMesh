///////////////////////////////////////
// AUTO GENERATED FILE - DO NOT EDIT //
///////////////////////////////////////

/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 the prplMesh contributors (see AUTHORS.md)
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
#include <vector>
#include "tlvf/ieee_1905_1/eMediaType.h"

namespace ieee1905_1 {

class cLocalInterfaceInfo;

class tlvDeviceInformation : public BaseClass
{
    public:
        tlvDeviceInformation(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit tlvDeviceInformation(std::shared_ptr<BaseClass> base, bool parse = false);
        ~tlvDeviceInformation();

        const eTlvType& type();
        const uint16_t& length();
        sMacAddr& mac();
        uint8_t& local_interface_list_length();
        std::tuple<bool, cLocalInterfaceInfo&> local_interface_list(size_t idx);
        std::shared_ptr<cLocalInterfaceInfo> create_local_interface_list();
        bool add_local_interface_list(std::shared_ptr<cLocalInterfaceInfo> ptr);
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eTlvType* m_type = nullptr;
        uint16_t* m_length = nullptr;
        sMacAddr* m_mac = nullptr;
        uint8_t* m_local_interface_list_length = nullptr;
        cLocalInterfaceInfo* m_local_interface_list = nullptr;
        size_t m_local_interface_list_idx__ = 0;
        std::vector<std::shared_ptr<cLocalInterfaceInfo>> m_local_interface_list_vector;
        bool m_lock_allocation__ = false;
        int m_lock_order_counter__ = 0;
};

class cLocalInterfaceInfo : public BaseClass
{
    public:
        cLocalInterfaceInfo(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cLocalInterfaceInfo(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cLocalInterfaceInfo();

        sMacAddr& mac();
        eMediaType& media_type();
        uint8_t& media_info_length();
        uint8_t* media_info(size_t idx = 0);
        bool set_media_info(const void* buffer, size_t size);
        bool alloc_media_info(size_t count = 1);
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        sMacAddr* m_mac = nullptr;
        eMediaType* m_media_type = nullptr;
        uint8_t* m_media_info_length = nullptr;
        uint8_t* m_media_info = nullptr;
        size_t m_media_info_idx__ = 0;
        int m_lock_order_counter__ = 0;
};

}; // close namespace: ieee1905_1

#endif //_TLVF/IEEE_1905_1_TLVDEVICEINFORMATION_H_
