/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef MAP_TRANSPORT_IEEE1905_TRANSPORT_H_
#define MAP_TRANSPORT_IEEE1905_TRANSPORT_H_

#include "ieee1905_transport_messages.h"
#include <mapf/common/poller.h>
#include <mapf/local_bus.h>

#include <arpa/inet.h>
#include <chrono>
#include <linux/netlink.h>
#include <map>

//
// Notes:
//
// Setting ebtables rules:
// -----------------------
// IEEE1905 multicast packets should not be bridge-fludded by default (only "relayed multicast" packets should be fludded)
// In order to prevent bridge fludding of IEEE1905 multicast packets the following ebtables configuration should be applied:
//
//      ebtables -t filter -I FORWARD 1 -p 0x893a --destination 01:80:c2:00:00:13 -j DROP
//
// Relayed multicast will be implemented by the IEEE1905 protocol handler (in the Ieee1905Transport class)
//
// TODO: need to set ebtables rule at the platform level
//
//
// Modifying GRX350 GSWIP-L configuration:
// --------------------------------------
// The GSWIP-L ethernet switch hardware of the GRX350 is configured by default to always forward/flood multicast packets. Flooding all multicast
// packets between the four Ethernet ports of the GRX350 will break the non-relayed multicast behaviour expected
// from IEEE1905 devices. In order to avoid this the following command should be issued:
//
//      switch_cli dev=0 GSW_REGISTER_SET nRegAddr=0x454 nData=1
//
// TODO: this could be considered a platform bug that should be solved at the platform level - see https://jira-chd.intel.com/browse/UGW_SW-25961
//

namespace mapf {

class Ieee1905Transport {
public:
    void run();

private:
    // network interface status table
    // this table holds all the network interfaces that are to be used by
    // the transport.
    //
    // interface index (if_index) is used as Key to the table
    struct NetworkInterface {
        int fd =
            -1; // the file descriptor of the socket bound to this interface (or -1 if inactive)
        uint8_t addr[ETH_ALEN] = {0};
        unsigned int bridge_if_index =
            0;                  // the bridge's interface index (if this interface is in a bridge)
        bool is_bridge = false; // is this interface a bridge interface
    };
    std::map<unsigned int, NetworkInterface> network_interfaces_;

    // netlink socket file descriptor (used to track network interface status)
    int netlink_fd_ = -1;

    mapf::LocalBusInterface *local_bus_;
    mapf::Poller poller_;

    uint16_t message_id_           = 0;
    uint8_t al_mac_addr_[ETH_ALEN] = {0};

// IEEE1905 CMDU header in packed format
#pragma pack(push, 1)
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
#pragma pack(pop)

    // packed TLV struct to help in the parsing of the packet
#pragma pack(push, 1)
    struct Tlv {
    protected:
        uint8_t tlvType;
        uint16_t tlvLength;

    public:
        uint8_t &type() { return tlvType; }

        Tlv *next() { return (Tlv *)((uint8_t *)this + size()); }

        size_t size() { return sizeof(Tlv) + ntohs(tlvLength); }
    };
#pragma pack(pop)

    // internal statistics counters
    enum CounterId {
        MESSAGE_ID_WRAPAROUND,
        INCOMMING_NETWORK_PACKETS,
        OUTGOING_NETWORK_PACKETS,
        INCOMMING_LOCAL_BUS_PACKETS,
        OUTGOING_LOCAL_BUS_PACKETS,
        DUPLICATE_PACKETS,
        DEFRAGMENTATION_FAILURE,
    };
    std::map<CounterId, unsigned long> counters_;

    // an internal data structure used for manipulating packets (CMDUs, LLDP, etc.)
    class Packet {
    public:
        uint8_t dst_if_type       = CmduRxMessage::IF_TYPE_NONE;
        unsigned int dst_if_index = 0;
        uint8_t src_if_type       = CmduRxMessage::IF_TYPE_NONE;
        unsigned int src_if_index = 0;
        uint8_t dst[ETH_ALEN]     = {0}; // destination mac address
        uint8_t src[ETH_ALEN]     = {0}; // source mac address
        uint16_t ether_type       = 0x0000;
        struct iovec header       = {.iov_base = NULL, .iov_len = 0};
        struct iovec payload      = {.iov_base = NULL, .iov_len = 0};

        virtual std::ostream &print(std::ostream &os) const;
    };
    friend std::ostream &operator<<(std::ostream &os, const Packet &m);

    // de-duplication internal data structures

    // two IEE1905 packets received from the same src address and with the same messageId
    // will be considered as duplicate iff they are received within a kMaximumDeDuplicationAge duration.
    // This duration should be long enough to allow for any buffered/in-transit duplicate packets to be flushed; and
    // should be short enough to handle the case when a device reboots (and reuses the same messageId).
    const std::chrono::milliseconds kMaximumDeDuplicationAge = std::chrono::milliseconds(1000);

    // limit the size of the de-duplication map (to prevent memory exhaustion attack)
    const int kMaximumDeDuplicationThreads = 1024;

    struct DeDuplicationKey {
        uint8_t src[ETH_ALEN];
        uint8_t dst[ETH_ALEN];
        uint16_t
            messageType; // required to distinguish the case of Request / Reply (when the reply carries an out of sync MID)
        uint16_t messageId;
        uint8_t fragmentId;
    };
    struct DeDuplicationKeyCompare {
        // implement Compare for the std::map template
        bool operator()(const DeDuplicationKey &lhs, const DeDuplicationKey &rhs) const
        {
            // compare based on src and dst address first then messageId, then fragmentId
            int srccmp = memcmp(lhs.src, rhs.src, ETH_ALEN);
            if (srccmp)
                return srccmp < 0;
            int dstcmp = memcmp(lhs.dst, rhs.dst, ETH_ALEN);
            if (dstcmp)
                return dstcmp < 0;
            else if (lhs.messageType != rhs.messageType)
                return lhs.messageType < rhs.messageType;
            else if (lhs.messageId != rhs.messageId)
                return lhs.messageId < rhs.messageId;
            else
                return lhs.fragmentId < rhs.fragmentId;
        }
    };
    struct DeDuplicationValue {
        std::chrono::steady_clock::time_point time;
    };
    std::map<DeDuplicationKey, DeDuplicationValue, DeDuplicationKeyCompare> de_duplication_map_;

    // de-fragmentation internal data structures

    // two IEE1905 packet fragments received from the same src address and with the same messageId
    // will be considered as belonging to the same PDU iff they are received within a kMaximumDeFragmentionAge duration.
    // This duration should be long enough to allow for any buffered/in-transit fragments to arrive; and
    // should be short enough to handle the case when a device reboots (and reuses the same messageId).
    const std::chrono::milliseconds kMaximumDeFragmentationAge = std::chrono::milliseconds(1000);

    // limit the size of the de-fragmentation map (to prevent memory exhaustion attack)
    const int kMaximumDeFragmentationThreads = 16;

    static const size_t kMaximumDeFragmentionSize = (64 * 1024);

    struct DeFragmentationKey {
        uint8_t src[ETH_ALEN];
        uint16_t
            messageType; // required to distinguish the case of Request / Reply (when the reply carries an out of sync MID)
        uint16_t messageId;
    };
    struct DeFragmentationKeyCompare {
        // implement Compare for the std::map template
        bool operator()(const DeFragmentationKey &lhs, const DeFragmentationKey &rhs) const
        {
            // compare based on src address first then messageId
            int srccmp = memcmp(lhs.src, rhs.src, ETH_ALEN);
            if (srccmp)
                return srccmp < 0;
            else if (lhs.messageType != rhs.messageType)
                return lhs.messageType < rhs.messageType;
            else
                return lhs.messageId < rhs.messageId;
        }
    };
    struct DeFragmentationValue {
        std::chrono::steady_clock::time_point time;
        uint8_t numFragments                   = 0;
        uint8_t buf[kMaximumDeFragmentionSize] = {0};
        int bufIndex                           = 0;
        bool complete                          = false;
    };
    std::map<DeFragmentationKey, DeFragmentationValue, DeFragmentationKeyCompare>
        de_fragmentation_map_;

    static const int kIeee1905FragmentationThreashold =
        1500 -
        sizeof(Tlv); // IEEE1905 packets (CMDU) should be fragmented if larger than this threashold

    //
    // NETWORK INTERFACE STUFF
    //
    void
    update_network_interfaces(std::map<unsigned int, NetworkInterface> updated_network_interfaces);
    bool open_interface_socket(unsigned int if_index);
    bool attach_interface_socket_filter(unsigned int if_index);
    void handle_interface_status_change(unsigned int if_index, bool is_active);
    void handle_interface_pollin_event(int fd);
    bool get_interface_mac_addr(unsigned int if_index, uint8_t *addr);
    bool send_packet_to_network_interface(unsigned int if_index, Packet &packet);
    void set_al_mac_addr(const uint8_t *addr);

    //
    // NETLINK STUFF
    //
    bool open_netlink_socket();
    int handle_netlink_message(struct nlmsghdr *msg);
    void handle_netlink_pollin_event();

    //
    // LOCAL BUS STUFF
    //
    void handle_local_bus_pollin_event();
    void handle_local_bus_cmdu_tx_message(CmduTxMessage &msg);
    void handle_local_bus_interface_configuration_request_message(
        InterfaceConfigurationRequestMessage &msg);
    bool send_packet_to_local_bus(Packet &packet);
    void publish_interface_configuration_indication();
    uint16_t get_next_message_id();

    //
    // PACKET PROCESSING STUFF
    //
    void handle_packet(Packet &packet);
    bool verify_packet(Packet &packet);
    bool de_duplicate_packet(Packet &packet);
    bool de_fragment_packet(Packet &packet);
    bool fragment_and_send_packet_to_network_interface(unsigned int if_index, Packet &packet);
    bool forward_packet(Packet &packet);
};

inline std::ostream &operator<<(std::ostream &os, const Ieee1905Transport::Packet &m)
{
    return m.print(os);
}

}; // namespace mapf

#endif // MAP_TRANSPORT_IEEE1905_TRANSPORT_H_
