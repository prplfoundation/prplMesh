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

#ifndef _TLVF_WFA_MAP_TLVCHANNELSELECTIONRESPONSE_H_
#define _TLVF_WFA_MAP_TLVCHANNELSELECTIONRESPONSE_H_

#include <cstddef>
#include <stdint.h>
#include <tlvf/swap.h>
#include <string.h>
#include <memory>
#include <tlvf/BaseClass.h>
#include <tlvf/ClassList.h>
#include "tlvf/wfa_map/eTlvTypeMap.h"
#include "tlvf/common/sMacAddr.h"

namespace wfa_map {


class tlvChannelSelectionResponse : public BaseClass
{
    public:
        tlvChannelSelectionResponse(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit tlvChannelSelectionResponse(std::shared_ptr<BaseClass> base, bool parse = false);
        ~tlvChannelSelectionResponse();

        enum eResponseCode: uint8_t {
            ACCEPT = 0x0,
            DECLINE_VIOLATES_CURRENT_PREFERENCES = 0x1,
            DECLINE_VIOLATES_MOST_RECENTLY_REPORTED_PREFERENCES = 0x2,
            DECLINE_PREVENT_OPERATION_OF_BACKHAUL_LINK = 0x3,
        };
        
        const eTlvTypeMap& type();
        const uint16_t& length();
        sMacAddr& radio_uid();
        eResponseCode& response_code();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eTlvTypeMap* m_type = nullptr;
        uint16_t* m_length = nullptr;
        sMacAddr* m_radio_uid = nullptr;
        eResponseCode* m_response_code = nullptr;
};

}; // close namespace: wfa_map

#endif //_TLVF/WFA_MAP_TLVCHANNELSELECTIONRESPONSE_H_
