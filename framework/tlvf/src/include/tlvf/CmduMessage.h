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

typedef struct sTlvHeader {
    uint8_t type;
    uint16_t length;
} __attribute__((packed)) sTlvHeader;

class CmduMessage {

public:
    CmduMessage() = delete;
    CmduMessage(uint8_t *buff, size_t buff_len) : msg(buff, buff_len){};
    ~CmduMessage(){};

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
    template <class T> std::shared_ptr<T> getClass() const { return msg.getClass<T>(); };
    template <class T> std::list<std::shared_ptr<T>> getClassList() const
    {
        return msg.getClassList<T>();
    };
    size_t getMessageLength() const { return msg.getMessageLength(); };
    size_t getMessageBuffLength() const { return msg.getMessageBuffLength(); };
    uint8_t *getMessageBuff() const { return msg.getMessageBuff(); };
    template <class T> std::shared_ptr<T> getClass() { return msg.getClass<T>(); }
    void swap() { msg.swap(); };
    bool is_finalized() const { return msg.is_finalized(); };
    bool is_swapped() const { return msg.is_swapped(); };

    static const size_t kMaxCmduLength      = 1500;
    static const uint16_t kCmduHeaderLength = 8;
    static const uint16_t kTlvHeaderLength  = 3;

protected:
    ClassList msg;
    bool parse();

private:
    std::shared_ptr<BaseClass> parseNextTlv();
};

}; // namespace ieee1905_1

#endif //_CmduMessage_H_
