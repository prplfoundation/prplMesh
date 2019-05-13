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

#ifndef _TLVF_IEEE_1905_1_TLVPUSHBUTTONJOINNOTIFICATION_H_
#define _TLVF_IEEE_1905_1_TLVPUSHBUTTONJOINNOTIFICATION_H_

#include "tlvf/common/sMacAddress.h"
#include "tlvf/ieee_1905_1/eTlvType.h"
#include <cstddef>
#include <memory>
#include <stdint.h>
#include <string.h>
#include <tlvf/BaseClass.h>
#include <tlvf/swap.h>

namespace ieee1905_1 {

class tlvPushButtonJoinNotification : public BaseClass {
public:
    tlvPushButtonJoinNotification(uint8_t *buff, size_t buff_len, bool parse = false,
                                  bool swap_needed = false);
    tlvPushButtonJoinNotification(std::shared_ptr<BaseClass> base, bool parse = false,
                                  bool swap_needed = false);
    ~tlvPushButtonJoinNotification();

    const eTlvType &type();
    const uint16_t &length();
    sMacAddress &al_mac_notification_src();
    uint16_t &mid_of_the_notification();
    //Interface-specific MAC address of the interface of the transmitting device
    //belonging to the medium on which a new device joined.
    sMacAddress &transmitter_iface_mac_of_new_device_joined();
    //Interface-specific MAC address of the interface of the new device that was
    //joined to the network as a result of the push button configuration sequence.
    sMacAddress &iface_mac_of_new_device_joined();
    void class_swap();
    static size_t get_initial_size();

private:
    bool init();
    eTlvType *m_type                                          = nullptr;
    uint16_t *m_length                                        = nullptr;
    sMacAddress *m_al_mac_notification_src                    = nullptr;
    uint16_t *m_mid_of_the_notification                       = nullptr;
    sMacAddress *m_transmitter_iface_mac_of_new_device_joined = nullptr;
    sMacAddress *m_iface_mac_of_new_device_joined             = nullptr;
};

}; // close namespace: ieee1905_1

#endif //_TLVF/IEEE_1905_1_TLVPUSHBUTTONJOINNOTIFICATION_H_
