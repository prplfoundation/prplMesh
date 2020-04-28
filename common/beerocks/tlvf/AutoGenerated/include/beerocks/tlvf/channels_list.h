///////////////////////////////////////
// AUTO GENERATED FILE - DO NOT EDIT //
///////////////////////////////////////

/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _BEEROCKS_TLVF_CHANNELS_LIST_H_
#define _BEEROCKS_TLVF_CHANNELS_LIST_H_

#include <cstddef>
#include <stdint.h>
#include <tlvf/swap.h>
#include <string.h>
#include <memory>
#include <tlvf/BaseClass.h>
#include <tlvf/ClassList.h>
#include <tuple>
#include "beerocks/tlvf/beerocks_message_action.h"
#include "bcl/beerocks_message_structs.h"

class cChannelsList : public BaseClass
{
    public:
        cChannelsList(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit cChannelsList(std::shared_ptr<BaseClass> base, bool parse = false);
        ~cChannelsList();

        uint8_t& supported_channels_length();
        std::tuple<bool, beerocks::message::sWifiChannel&> supported_channels(size_t idx);
        bool alloc_supported_channels(size_t count = 1);
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        uint8_t* m_supported_channels_length = nullptr;
        beerocks::message::sWifiChannel* m_supported_channels = nullptr;
        size_t m_supported_channels_idx__ = 0;
        int m_lock_order_counter__ = 0;
};

#endif //_BEEROCKS/TLVF_CHANNELS_LIST_H_
