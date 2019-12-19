/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _CmduMessage_H_
#define _CmduMessage_H_

#include <memory>
#include <tlvf/ClassList.h>
#include <tlvf/ieee_1905_1/cCmduHeader.h>
#include <tlvf/ieee_1905_1/eTlvType.h>
#include <tlvf/tlvflogging.h>
#include <vector>

namespace ieee1905_1 {

class CmduMessage {

public:
    CmduMessage() = delete;
    CmduMessage(uint8_t *buff, size_t buff_len) : msg(buff, buff_len){};
    ~CmduMessage(){};

    ClassList msg;

    std::shared_ptr<cCmduHeader> getCmduHeader() const { return msg.getClass<cCmduHeader>(); };
    static uint16_t getCmduHeaderLength() { return kCmduHeaderLength; }
    eMessageType getMessageType();
    uint16_t getMessageId();
    bool getNextTlvType(eTlvType &tlvType) const;
    int getNextTlvType() const;
    uint16_t getNextTlvLength() const;
    uint8_t *getNextTlvData() const;

    // Forward wrapper functions
    // TODO check which of them can be removed
    template <class T> std::shared_ptr<T> addClass() { return msg.addClass<T>(); }
    std::shared_ptr<BaseClass> getClass(size_t idx) const { return msg.getClass(idx); };
    template <class T> std::shared_ptr<T> getClass() const { return msg.template getClass<T>(); };
    template <class T> std::shared_ptr<T> getClass(size_t idx) const
    {
        return msg.template getClass<T>(idx);
    };
    template <class T> size_t getClassCount() const { return msg.template getClassCount<T>(); };
    size_t getMessageLength() const { return msg.getMessageLength(); };
    size_t getMessageBuffLength() const { return msg.getMessageBuffLength(); };
    uint8_t *getMessageBuff() const { return msg.getMessageBuff(); };
    void swap() { msg.swap(); };
    bool is_finalized() const { return msg.is_finalized(); };
    bool is_swapped() const { return msg.is_swapped(); };

    static const size_t kMaxCmduLength      = 1500;
    static const uint16_t kCmduHeaderLength = 8;
    static const uint16_t kTlvHeaderLength  = 3;
};

}; // namespace ieee1905_1

#endif //_CmduMessage_H_
