///////////////////////////////////////
// AUTO GENERATED FILE - DO NOT EDIT //
///////////////////////////////////////

/*
#############################################################################
# INTEL CONFIDENTIAL
# Copyright 2018 Intel Corporation All Rights Reserved.
#
# The source code contained or described herein and all documents related to
# the source code ("Material") are owned by Intel Corporation or its
# suppliers or licensors.  Title to the Material remains with Intel
# Corporation or its suppliers and licensors.  The Material contains trade
# secrets and proprietary and confidential information of Intel or its
# suppliers and licensors.  The Material is protected by worldwide copyright
# and trade secret laws and treaty provisions. No part of the Material may
# be used, copied, reproduced, modified, published, uploaded, posted,
# transmitted, distributed, or disclosed in any way without Intel's prior
# express written permission.
#
# No license under any patent, copyright, trade secret or other intellectual
# property right is granted to or conferred upon you by disclosure or
# delivery of the Materials,  either expressly, by implication, inducement,
# estoppel or otherwise.  Any license under such intellectual property
# rights must be express and approved by Intel in writing.
#############################################################################
*/

#ifndef _BEEROCKS_TLVF_BEEROCKS_MESSAGE_HEADER_H_
#define _BEEROCKS_TLVF_BEEROCKS_MESSAGE_HEADER_H_

#include <cstddef>
#include <stdint.h>
#include <tlvf/swap.h>
#include <string.h>
#include <memory>
#include <tlvf/BaseClass.h>
#include "beerocks/tlvf/beerocks_message_common.h"
#include "beerocks/tlvf/beerocks_message_action.h"

namespace beerocks_message {


class cACTION_HEADER : public BaseClass
{
    public:
        cACTION_HEADER(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_HEADER(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_HEADER();

        const uint32_t& magic();
        const uint8_t& version();
        eAction& action();
        //need to cast eActionOp_XXXX to uint8_t
        uint8_t& action_op();
        uint8_t& direction();
        beerocks::net::sMacAddr& radio_mac();
        uint8_t& last();
        uint16_t& id();
        uint16_t& length();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        uint32_t* m_magic = nullptr;
        uint8_t* m_version = nullptr;
        eAction* m_action = nullptr;
        uint8_t* m_action_op = nullptr;
        uint8_t* m_direction = nullptr;
        beerocks::net::sMacAddr* m_radio_mac = nullptr;
        uint8_t* m_last = nullptr;
        uint16_t* m_id = nullptr;
        uint16_t* m_length = nullptr;
};

}; // close namespace: beerocks_message

#endif //_BEEROCKS/TLVF_BEEROCKS_MESSAGE_HEADER_H_
