/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _CmduMessageRX_H_
#define _CmduMessageRX_H_

#include <tlvf/CmduMessage.h>

namespace ieee1905_1 {

class CmduMessageRx : public CmduMessage {

public:
    CmduMessageRx() = delete;
    CmduMessageRx(uint8_t *buff, size_t buff_len) : CmduMessage(buff, buff_len){};
    ~CmduMessageRx(){};

    // Forward wrapper functions
    template <class T> std::shared_ptr<T> getClass() const { return msg.getClass<T>(); };
    template <class T> std::list<std::shared_ptr<T>> getClassList() const
    {
        return msg.getClassList<T>();
    };

    bool parse();
    CmduMessageRx &operator=(const CmduMessageRx &) = delete;

private:
    int getNextTlvType() const;
    uint16_t getNextTlvLength() const;
    std::shared_ptr<BaseClass> parseNextTlv();
};

}; // namespace ieee1905_1

#endif //_CmduMessageRX_H_
