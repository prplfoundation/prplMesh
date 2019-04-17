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

#ifndef _CmduMessage_H_
#define _CmduMessage_H_

#include <vector>
#include <memory>
#include "ieee_1905_1/cCmduHeader.h"
#include "ieee_1905_1/eTlvType.h"

namespace ieee1905_1{

class CmduMessage {

public:

    CmduMessage();
    ~CmduMessage();

public:
    std::shared_ptr<cCmduHeader> getCmduHeader() const;

    template <class T> std::shared_ptr<T> addClass()
    {
        std::shared_ptr<T> ptr;
        if (m_cmdu_header) {
            if (m_class_vector.size() == 0) {
                ptr = std::make_shared<T>(m_cmdu_header, m_parse, m_swap);
            } else {
                ptr = std::make_shared<T>(m_class_vector.back(), m_parse, m_swap);
            }
            if (!ptr ||  ptr->isInitialized() == false) {
                return std::shared_ptr<T>();
            } else {
                m_class_vector.push_back(std::shared_ptr<BaseClass>(ptr));
            }
        } else {
            return std::shared_ptr<T>();
        }
        return ptr;
    }

    template <class T> std::shared_ptr<T> dynamicCast(std::shared_ptr<BaseClass> ptr) const {
        return std::dynamic_pointer_cast<T>(ptr);
    }

    static uint16_t getCmduHeaderLength() {
        return kCmduHeaderLength;
    }

    std::shared_ptr<BaseClass> getClass(size_t idx) const;

    template <class T> std::shared_ptr<T> getClass(size_t idx) const {
        return std::dynamic_pointer_cast<T>(getClass(idx));
    }
    
    size_t getClassCount() const;
    const std::vector<std::shared_ptr<BaseClass>>& getClassVector() const;
    size_t getMessageLength() const;
    size_t getMessageBuffLength() const;
    uint8_t* getMessageBuff() const;
    bool getNextTlvType(eTlvType& tlvType) const;
    uint16_t getNextTlvLength() const;
    void swap();
    bool is_finalized() const { return m_finalized; };
    bool is_swapped() const { return m_swapped; };
    eMessageType getMessageType();

protected:

    void reset();
    
    size_t m_buff_len = 0;
    bool m_finalized = false;
    bool m_swapped = false;
    bool m_parse = false;
    bool m_swap = false;
    bool m_dynamically_allocated = false;
    uint8_t* m_buff = nullptr;
    std::shared_ptr<cCmduHeader> m_cmdu_header;
    std::vector<std::shared_ptr<BaseClass>> m_class_vector;
    static const uint16_t kCmduHeaderLength = 8;
    static const uint16_t kTlvHeaderLength = 4;
};

}; // close namespace: ieee1905_1

#endif //_CmduMessage_H_
