/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _MULTIAP_COMMON_H_
#define _MULTIAP_COMMON_H_

#include <mapf/local_bus.h>
#include <mapf/transport/ieee1905_transport.h>

namespace mapf {

class MultiApBase {
public:
    // IEEE1905 CMDU header in packed format
    struct Ieee1905CmduHeader {
        uint8_t messageVersion;
        uint8_t _reservedField0;
        uint16_t messageType;
        uint16_t messageId;
        uint8_t fragmentId;
        uint8_t flags;
        void SetLastFragmentIndicator(bool value)
        {
            flags = (flags & ~0x80) | (value ? 0x80 : 0x00);
        };
        bool GetLastFragmentIndicator() { return (flags & 0x80); };
        void SetRelayIndicator(bool value) { flags = (flags & ~0x40) | (value ? 0x40 : 0x00); };
        bool GetRelayIndicator() { return (flags & 0x40); };
    };

    MultiApBase() : bus_(Context::Instance()) { bus_.Init(); }

    int Poll(long timeout)
    {
        int rc = poller_.Poll(timeout);
        if (rc > 0) {
            rc = poller_.CheckEvent(bus_.subscriber());
            mapf_assert(rc == MAPF_POLLIN);
        }
        return rc;
    }

    void Subscribe(uint16_t msg_type)
    {
        int rc =
            bus_.subscriber().Subscribe<CmduRxMessage>(CmduRxMessage::ieee1905_topic(msg_type));
        mapf_assert(rc == 0);
        rc = poller_.Add(bus_.subscriber());
        mapf_assert(rc == 0);
        bus_.Sync();
        MAPF_INFO("Subscriber: " << bus_.subscriber());
        MessageFactory::Instance().DumpMakers();
    }

protected:
    LocalBusInterface bus_;
    Poller poller_;
};
}; // namespace mapf
#endif
