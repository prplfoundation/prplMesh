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

#ifndef _BaseClass_H_
#define _BaseClass_H_

#include <stdint.h>
#include <stddef.h>
#include <cstddef>
#include <string>

class BaseClass {
    protected:
        BaseClass(uint8_t* buff_, const size_t buff_len_, const bool parse_=false, const bool swap_needed = false);
        ~BaseClass();

    public:
        uint8_t* getBuffPtr();
		uint8_t* getStartBuffPtr();
        size_t getBuffRemainingBytes();
        size_t getLen();
        bool isInitialized();
        virtual void class_swap() = 0;

    protected:
        uint8_t* m_buff__;
        uint8_t* m_buff_ptr__;
        const size_t m_buff_len__;
        const bool m_parse__;
        const bool m_swap__;
        bool m_init_succeeded;
        static const size_t kMinimumLength = 4; // tlv header length
};

#endif //_BaseClass_H_
