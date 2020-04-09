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

#ifndef _TLVF_IEEE_1905_1_TLVPUSHBUTTONJOINNOTIFICATION_H_
#define _TLVF_IEEE_1905_1_TLVPUSHBUTTONJOINNOTIFICATION_H_

#include <cstddef>
#include <stdint.h>
#include <tlvf/swap.h>
#include <string.h>
#include <memory>
#include <tlvf/BaseClass.h>
#include <tlvf/ClassList.h>
#include "tlvf/ieee_1905_1/eTlvType.h"
#include "tlvf/common/sMacAddr.h"

namespace ieee1905_1 {


class tlvPushButtonJoinNotification : public BaseClass
{
    public:
        tlvPushButtonJoinNotification(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit tlvPushButtonJoinNotification(std::shared_ptr<BaseClass> base, bool parse = false);
        ~tlvPushButtonJoinNotification();

        const eTlvType& type();
        const uint16_t& length();
        sMacAddr& al_mac_notification_src();
        uint16_t& mid_of_the_notification();
        //Interface-specific MAC address of the interface of the transmitting device
        //belonging to the medium on which a new device joined.
        sMacAddr& transmitter_iface_mac_of_new_device_joined();
        //Interface-specific MAC address of the interface of the new device that was
        //joined to the network as a result of the push button configuration sequence.
        sMacAddr& iface_mac_of_new_device_joined();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eTlvType* m_type = nullptr;
        uint16_t* m_length = nullptr;
        sMacAddr* m_al_mac_notification_src = nullptr;
        uint16_t* m_mid_of_the_notification = nullptr;
        sMacAddr* m_transmitter_iface_mac_of_new_device_joined = nullptr;
        sMacAddr* m_iface_mac_of_new_device_joined = nullptr;
};

}; // close namespace: ieee1905_1

#endif //_TLVF/IEEE_1905_1_TLVPUSHBUTTONJOINNOTIFICATION_H_
