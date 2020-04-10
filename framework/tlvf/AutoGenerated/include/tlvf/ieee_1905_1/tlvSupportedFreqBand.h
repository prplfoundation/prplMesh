///////////////////////////////////////
// AUTO GENERATED FILE - DO NOT EDIT //
///////////////////////////////////////

/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2019 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _TLVF_IEEE_1905_1_TLVSUPPORTEDFREQBAND_H_
#define _TLVF_IEEE_1905_1_TLVSUPPORTEDFREQBAND_H_

#include <cstddef>
#include <stdint.h>
#include <tlvf/swap.h>
#include <string.h>
#include <memory>
#include <tlvf/BaseClass.h>
#include <tlvf/ClassList.h>
#include "tlvf/ieee_1905_1/eTlvType.h"

namespace ieee1905_1 {


class tlvSupportedFreqBand : public BaseClass
{
    public:
        tlvSupportedFreqBand(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit tlvSupportedFreqBand(std::shared_ptr<BaseClass> base, bool parse = false);
        ~tlvSupportedFreqBand();

        enum eValue: uint8_t {
            BAND_2_4G = 0x0,
            BAND_5G = 0x1,
            BAND_60G = 0x2,
        };
        
        const eTlvType& type();
        const uint16_t& length();
        eValue& value();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eTlvType* m_type = nullptr;
        uint16_t* m_length = nullptr;
        eValue* m_value = nullptr;
};

}; // close namespace: ieee1905_1

#endif //_TLVF/IEEE_1905_1_TLVSUPPORTEDFREQBAND_H_
