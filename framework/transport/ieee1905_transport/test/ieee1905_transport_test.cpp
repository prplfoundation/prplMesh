/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include <mapf/broker/broker.h>
#include <mapf/common/poller.h>
#include <mapf/local_bus.h>
#include <mapf/transport/ieee1905_transport.h>

#include <mapf/common/logger.h>

#include <arpa/inet.h>
#include <ctype.h>
#include <exception>
#include <net/if.h>
#include <signal.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

//  Suggestions for unit testing (none of this is currently happening in this simple test program):
//  ----------------------------------------------------------------------------------------------
//  connect external Tester PC (with many interfaces) to DUT network interfaces:
//  - connect multiple Ethernet links
//  - connect Tester Wi-Fi to DUT AP
//  - connect DUT STA (EP) to Tester AP
//
//  /------------------------------------------------------------------------------\
//  |                                                                              |
//  |                                                                              |
//  |                                   Tester                                     |
//  |                                  Linux PC                                    |
//  |                          Running Test Application                            |
//  |                                                                              |
//  |                                                           STA        AP      |
//  | /--------\ /--------\ /--------\ /--------\ /--------\ /--------\ /--------\ |
//  | |  eth1  | |  eth2  | |  eth3  | |  eth4  | |  eth5  | | wlan0  | | wlan1  | |
//  \------------------------------------------------------------------------------/
//    []  |          |          |          |          |          :          :   []
//        |          |          |          |          |          :          :
//        |          |          |          |          |          :          :
//  /------------------------------------------------------------------------------\
//  | |  eth1  | | eth0_1 | | eth0_2 | | eth0_3 | | eth0_4 | | wlan0  | | wlan1  | |
//  | \--------/ \--------/ \--------/ \--------/ \--------/ \--------/ \--------/ |
//  |                                              Excluded     AP         STA     |
//  |                                                                              |
//  |                                     DUT                                      |
//  |                               EASY350 Platform                               |
//  |                          Running Multi-AP Framework                          |
//  |                              Running Dummy Agent                             |
//  |                 Multi-AP Agent and Multi-AP Controller disabled              |
//  |                                                                              |
//  |                                                                              |
//  \------------------------------------------------------------------------------/
//    []                                                                        []
//
//
//  The Test Application on the Tester PC should be able to generate arbitrary packets on any specific interface. And capture
//  all packets on all the interfaces.
//
//  Generate cooked packets from Tester on each one of the above connections (one at a time) and verify that:
//  1. IEEE1905 unicast packets with DUT dest address are not looped back to any of the other Tester's interfaces.
//  2. IEEE1905 unicast packets with non-DUT dest address are forwarded to all the Tester's interfaces (flooded). (this address
//     should not be the address of any interface in the setup). Verify each packet is forwarded exactly once per interface (e.g. not
//     duplicated by GSWIP-L).
//  3. IEEE1905 neighbor multicast packets are not looped back to any of the Tester's interfaces.
//  4. IEEE1905 relayed multicast packets are forwarded to all of the Tester's interfaces (and not to the originating interface).
//  5. Duplicate IEEE1905 multicast packets are not forwarded.
//  6. Generate non IEEE1905 packets (multicast, unicast, etc.) - verify that they are handled normally.
//  7. Send fragmented CMDUs (many valid and invalid varieties) with a predefined messageType to which the Dummy Agent will loop-back
//     the CMDUs.
//     - missing fragments
//     - huge CMDU (e.g. 256 fragments)
//     - send fragmented CMDU that when reassembled would contain a huge TLV - this should not be looped-back by the Dummy Agent
//       (the fragmentation code should drop this CMDU once the Dummy Agent tries to send it)
//     - send many many interleaved threads of fragmented CMDUs
//  8. Send duplicate CMDUs with a predefined messageType to which the Dummy Agent will loop-back the CMDUs.
//     - short delay between duplicate - verify duplicate is dropped
//     - long delay between duplicate - verify duplicate is accepted
//     - many duplicates - verify are dropped
//     - duplicates from multiple interfaces
//     - send many many concurrent different duplicate packets (many types of packets each duplicate many times)
//  9. Have one of the DUT ports excluded from the Transport
//     - verify zero IEEE1905 packets are transmitted by the excluded interface (e.g. in all the above tests)
//     - verify zero IEEE1905 packets are forwarded from this interface (when the Tester sends a IEEE1905 packet to it - e.g. in all
//       the above tests).
//  10. Send many many random / rogue packets and verify the transport does not crash.
//
//  Note 1: This setup could also be used to unit-test the Discovery Agent and other high level entities.
//
//  Note 2: The Tester PC could be replaced with a VM and a smart switch (using VLANs) like so:
//
//  /------------------------------------------------------------------------------\
//  |                                                                              |
//  |                                   Tester                                     |
//  |                                  Linux VM                                    |
//  |                          Running Test Application                            |
//  |                                                                              |
//  |                                                                              |
//  | /--------\ /--------\ /--------\ /--------\ /--------\ /--------\ /--------\ |  \
//  | | eth1.1 | | eth1.2 | | eth1.3 | | eth1.4 | | eth1.5 | | eth1.6 | | eth1.7 | |   } VLAN interfaces
//  | \--------/ \--------/ \--------/ \--------/ \--------/ \--------/ \--------/ |  /
//  |                                                                              |
//  |                                                                              |
//  |  T1,T2,T3,T4,T5,T6,T7 <- All VLANs are tagged on this physical interface     |
//  |       /--------\                                                             |
//  |       |  eth1  |                                                             |
//  \------------------------------------------------------------------------------/
//              |
//              |  All VLANs are tagged on this link
//              |
//  /------------------------------------------------------------------------------\
//  |       | port0  |                                                             |
//  |       \--------/                                                             |
//  |  T1,T2,T3,T4,T5,T6,T7 <- All VLANs are configured tagged on this port        |
//  |                                                                              |
//  |                                Managed Switch                                |
//  |                                                                              |
//  |     U1         U2         U3         U4         U5         U6         U7     | } each VLAN is configured un-tagged on a single specific port
//  | /--------\ /--------\ /--------\ /--------\ /--------\ /--------\ /--------\ |
//  | | port1  | | port2  | | port3  | | port4  | | port5  | | port6  | | port7  | |
//  \------------------------------------------------------------------------------/
//        |          |          |          |          |          |          |
//        |          |          |          |          |          |          |
//        |          |          |          |          |      /--------\ /--------\   \
//        |          |          |          |          |      |  STA   | |   AP   |    } Two APs one configured for WDS mode (acting as a STA)
//        |          |          |          |          |      \--------/ \--------/   /
//        |          |          |          |          |          :          :
//        |          |          |          |          |          :          :
//
//                                      To DUT
//

#define MAX_IFS 16
static unsigned int ieee1905_bridge_if_index     = 0;
static unsigned int ieee1905_if_indexes[MAX_IFS] = {0};
static bool only_configure_interfaces            = false;
static bool use_unicast_address                  = false;
static uint8_t unicast_address[6];
using namespace mapf;

int fork_local_bus()
{
    pid_t pid = fork();
    mapf_assert(pid >= 0);

    if (pid != 0)
        return pid;

    MAPF_INFO("staring local bus broker");
    Broker localbus(kLocalBusConf);
    localbus.PrintConfig();
    localbus.Run();

    exit(0);
}

int fork_ieee1905_transport()
{
    pid_t pid = fork();
    mapf_assert(pid >= 0);

    if (pid != 0)
        return pid;

    MAPF_INFO("staring ieee1905 transport");

    Ieee1905Transport ieee1905_transport;

    MAPF_INFO("starting ieee1905 transport main loop...");
    ieee1905_transport.run();

    exit(0);
}

int isValidMacAddress(const char *mac)
{
    int digits     = 0;
    int seperators = 0;

    while (*mac) {
        if (isxdigit(*mac)) {
            digits++;
        } else if (*mac == ':') {
            if (digits == 0 || digits / 2 - 1 != seperators)
                break;

            seperators++;
        } else {
            seperators = -1;
            break;
        }
        mac++;
    }

    return (digits == 12 && (seperators == 5 || seperators == 0));
}

bool convertStringToMacAddress(const char *address, uint8_t *mac)
{
    if (!address) {
        return false;
    }
    int digits     = 0;
    int seperators = 0;
    std::string cleanAddress;

    //validate mac address
    while (*address) {
        if (isxdigit(*address)) {
            digits++;
            cleanAddress += *address;
        } else if (*address == ':') {
            if (digits == 0 || digits / 2 - 1 != seperators)
                break;

            seperators++;
        } else {
            seperators = -1;
            break;
        }
        address++;
    }

    if (digits == 12 && (seperators == 5 || seperators == 0)) {
        unsigned long tempaddr = strtoul(cleanAddress.c_str(), NULL, 16);

        for (int i = 5; i >= 0; i--) {
            mac[i] = tempaddr % 0x100;
            tempaddr /= 0x100;
        }
    } else {
        return false;
    }

    return true;
}

int main(int argc, char *argv[])
{

    mapf::Logger::Instance().LoggerInit("transport_test");

    int c;
    int interfaces = 0;
    while ((c = getopt(argc, argv, "b:i:u:x")) != -1) {
        switch (c) {
        case 'b':
            ieee1905_bridge_if_index = if_nametoindex(optarg);
            if (ieee1905_bridge_if_index == 0) {
                fprintf(stderr, "unknown interface %s\n", optarg);
                return false;
            }
            break;
        case 'i':
            ieee1905_if_indexes[interfaces] = if_nametoindex(optarg);
            if (ieee1905_if_indexes[interfaces] == 0) {
                fprintf(stderr, "unknown interface %s\n", optarg);
                return false;
            }
            interfaces++;
            break;
        case 'x':
            only_configure_interfaces = true;
            break;
        case 'u':
            if (convertStringToMacAddress(optarg, unicast_address)) {
                use_unicast_address = true;
            } else {
                fprintf(stderr, "Invalid MAC address\n");
                return false;
            }
            break;
        case '?':
            if (optopt == 'i') {
                fprintf(stderr, "Option -%c requires a network interface name as an argument.\n",
                        optopt);
            } else if (isprint(optopt)) {
                fprintf(stderr, "Unknown option `-%c'.\n", optopt);
            } else {
                fprintf(stderr, "Unknown option character `\\x%x'.\n", optopt);
            }
        // intentional fallthrough
        default:
            fprintf(stderr, "usage: %s [-b <interface name>] [-i <interface name>] [-x]...\n",
                    argv[0]);
            return false;
        }
    }

    pid_t local_bus_pid          = 0;
    pid_t ieee1905_transport_pid = 0;
    if (!only_configure_interfaces) {
        local_bus_pid          = fork_local_bus();
        ieee1905_transport_pid = fork_ieee1905_transport();
    }

    bool success = true;

    do {
        MAPF_INFO("initializing local bus interface");
        LocalBusInterface bus(Context::Instance());
        bus.Init();

        int rc;

        Poller poller;
        rc = poller.Add(bus.subscriber());
        if (rc != 0)
            break;

        // messageType 0x8000 is used exclusively to make sure the Ieee1905Transport proccess is up and running
        MAPF_INFO("subscribing to CmduRxMessage with topic "
                  << CmduRxMessage::ieee1905_topic(0x8000));
        rc = bus.subscriber().Subscribe<CmduRxMessage>(CmduRxMessage::ieee1905_topic(0x8000));
        if (rc != 0)
            break;
        bus.Sync();

        // this loop is not part of the test it is just to make sure
        // the Ieee1905Transport has started (it is running in a forked process)
        for (int i = 100; i > 0; i--) {
            // Send a multicast CmduTxMessage on the local bus - expect a CmduRxMessage (due to multicast)
            CmduTxMessage cmdu_tx_msg;
            std::copy_n("\x01\x80\xc2\x00\x00\x13", 6, cmdu_tx_msg.metadata()->dst);
            cmdu_tx_msg.metadata()->ether_type = ETH_P_1905_1;
            cmdu_tx_msg.metadata()->msg_type   = 0x8000;
            cmdu_tx_msg.metadata()->length     = 32;
            std::fill_n(cmdu_tx_msg.data(), cmdu_tx_msg.metadata()->length, 0x00);
            uint8_t ieee1905_header[] = {0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0xc0};
            std::copy_n(ieee1905_header, sizeof(ieee1905_header), cmdu_tx_msg.data());

            MAPF_INFO("sending CmduTxMessage: " << std::endl << cmdu_tx_msg);
            bool ret = bus.publisher().Send(cmdu_tx_msg);
            if (!ret) {
                success = false;
                break;
            }

            MAPF_INFO("polling...");
            rc = poller.Poll(100);
            if (rc == 0) {
                continue; // timeout
            }
            rc = poller.CheckEvent(bus.subscriber());
            if (rc != MAPF_POLLIN) {
                success = false;
                break;
            }

            //receive the message
            auto msg = bus.subscriber().Receive();

            if (auto cmdu_rx_msg = dynamic_cast<CmduRxMessage *>(msg.get())) {
                MAPF_INFO("received CmduRxMessage (initial synchronization): " << std::endl
                                                                               << *cmdu_rx_msg);
                break;
            } else {
                MAPF_INFO("received other message: " << std::endl << *msg);
                success = false;
                break;
            }
        }
        if (!success) {
            MAPF_INFO("failed to synchronize with Ieee1905Transport proccess");
            break;
        }

        MAPF_INFO("subscribing to CmduRxMessage with topic "
                  << CmduRxMessage::ieee1905_topic(0x8016));
        rc = bus.subscriber().Subscribe<CmduRxMessage>(CmduRxMessage::ieee1905_topic(0x8016));
        if (rc != 0) {
            success = false;
            break;
        }
        bus.Sync();

        MAPF_INFO("subscribing to CmduTxConfirmationMessage with topic "
                  << CmduTxConfirmationMessage::ieee1905_topic(0x8016));
        rc = bus.subscriber().Subscribe<CmduTxConfirmationMessage>(
            CmduTxConfirmationMessage::ieee1905_topic(0x8016));
        if (rc != 0) {
            success = false;
            break;
        }
        bus.Sync();

        // inform the 1905 Transport on the required interface configuration (as specified on the command line)
        InterfaceConfigurationRequestMessage interface_configuration_request_msg;
        uint32_t n = 0;
        if (ieee1905_bridge_if_index) {
            using Flags = InterfaceConfigurationRequestMessage::Flags;
            interface_configuration_request_msg.metadata()->interfaces[n].if_index =
                ieee1905_bridge_if_index;
            interface_configuration_request_msg.metadata()->interfaces[n].flags |= Flags::IS_BRIDGE;
            n++;
        }
        for (int i = 0; i < MAX_IFS; i++) {
            if (ieee1905_if_indexes[i]) {
                using Flags = InterfaceConfigurationRequestMessage::Flags;
                interface_configuration_request_msg.metadata()->interfaces[n].if_index =
                    ieee1905_if_indexes[i];
                interface_configuration_request_msg.metadata()->interfaces[n].flags |=
                    Flags::ENABLE_IEEE1905_TRANSPORT;
                n++;
            }
        }
        interface_configuration_request_msg.metadata()->numInterfaces = n;

        MAPF_INFO("sending InterfaceConfigurationRequestMessage: "
                  << std::endl
                  << interface_configuration_request_msg);
        bool ret = bus.publisher().Send(interface_configuration_request_msg);
        if (!ret) {
            MAPF_INFO("failed to send");
            success = false;
            break;
        }

        if (only_configure_interfaces) {
            break;
        }

        // Send a multicast CmduTxMessage on the local bus - expect a CmduRxMessage (due to multicast)
        // and a CmduTxConfirmationMessage.
        do {
            bool got_cmdu_rx_msg              = false;
            bool got_cmdu_tx_confirmation_msg = false;

            CmduTxMessage cmdu_tx_msg;
            std::copy_n("\x01\x80\xc2\x00\x00\x13", 6, cmdu_tx_msg.metadata()->dst);
            cmdu_tx_msg.metadata()->ether_type = ETH_P_1905_1;
            cmdu_tx_msg.metadata()->msg_type   = 0x8016;
            cmdu_tx_msg.metadata()->length     = 32;
            cmdu_tx_msg.metadata()->cookie     = 1;
            std::fill_n(cmdu_tx_msg.data(), cmdu_tx_msg.metadata()->length, 0x00);
            uint8_t ieee1905_header[] = {0x00, 0x00, 0x80, 0x16, 0x00, 0x00, 0x00, 0xc0};
            std::copy_n(ieee1905_header, sizeof(ieee1905_header), cmdu_tx_msg.data());

            MAPF_INFO("sending CmduTxMessage: " << std::endl << cmdu_tx_msg);
            ret = bus.publisher().Send(cmdu_tx_msg);
            if (!ret) {
                MAPF_INFO("failed to send");
                success = false;
                break;
            }

            for (int i = 10; i > 0; i--) {
                MAPF_INFO("polling...");
                rc = poller.Poll(100);
                if (rc == 0) {
                    continue;
                }

                rc = poller.CheckEvent(bus.subscriber());
                if (rc != MAPF_POLLIN) {
                    MAPF_INFO("unexpected event");
                    success = false;
                    break;
                }

                auto msg = bus.subscriber().Receive();

                if (auto cmdu_rx_msg = dynamic_cast<CmduRxMessage *>(msg.get())) {
                    MAPF_INFO("received CmduRxMessage: " << std::endl << *cmdu_rx_msg);
                    if (cmdu_rx_msg->metadata()->length != cmdu_tx_msg.metadata()->length) {
                        MAPF_INFO("message length mismatch");
                    } else if (cmdu_rx_msg->metadata()->ether_type !=
                               cmdu_tx_msg.metadata()->ether_type) {
                        MAPF_INFO("message metadata mismatch");
                    } else if (cmdu_rx_msg->metadata()->msg_type !=
                               cmdu_tx_msg.metadata()->msg_type) {
                        MAPF_INFO("message metadata mismatch");
                    } else {
                        MAPF_INFO("this is the expected message");
                        got_cmdu_rx_msg = true;
                    }
                } else if (auto cmdu_tx_confirmation_msg =
                               dynamic_cast<CmduTxConfirmationMessage *>(msg.get())) {
                    MAPF_INFO("received CmduTxConfirmationMessage: " << std::endl
                                                                     << *cmdu_tx_confirmation_msg);
                    got_cmdu_tx_confirmation_msg = true;
                } else {
                    MAPF_INFO("received other message: " << std::endl << *msg);
                }

                if (got_cmdu_rx_msg && got_cmdu_tx_confirmation_msg) {
                    break;
                }
            }
            if (!(got_cmdu_rx_msg && got_cmdu_tx_confirmation_msg)) {
                MAPF_INFO("did not receive all expected messages");
                success = false;
                break;
            }
        } while (0);

        // Send a non-relayed multicast CmduTxMessage on the local bus (helps to manually verify relayed multicast mechanism)
        {
            CmduTxMessage cmdu_tx_msg;
            std::copy_n("\x01\x80\xc2\x00\x00\x13", 6, cmdu_tx_msg.metadata()->dst);
            cmdu_tx_msg.metadata()->ether_type = ETH_P_1905_1;
            cmdu_tx_msg.metadata()->msg_type   = 0x8017;
            cmdu_tx_msg.metadata()->length     = 32;
            std::fill_n(cmdu_tx_msg.data(), cmdu_tx_msg.metadata()->length, 0x00);
            uint8_t ieee1905_header[] = {0x00, 0x00, 0x80, 0x17, 0x00, 0x00, 0x00, 0x80};
            std::copy_n(ieee1905_header, sizeof(ieee1905_header), cmdu_tx_msg.data());

            MAPF_INFO("sending CmduTxMessage: " << std::endl << cmdu_tx_msg);
            ret = bus.publisher().Send(cmdu_tx_msg);
            if (!ret)
                break;
        }

        // Send a relayed multicast CmduTxMessage on the local bus (helps to manually verify relayed multicast mechanism)
        {
            CmduTxMessage cmdu_tx_msg;
            std::copy_n("\x01\x80\xc2\x00\x00\x13", 6, cmdu_tx_msg.metadata()->dst);
            cmdu_tx_msg.metadata()->ether_type = ETH_P_1905_1;
            cmdu_tx_msg.metadata()->msg_type   = 0x8018;
            cmdu_tx_msg.metadata()->length     = (8) + (3 + 0);
            std::fill_n(cmdu_tx_msg.data(), cmdu_tx_msg.metadata()->length, 0x00);

            uint8_t ieee1905_header[] = {0x00, 0x00, 0x80, 0x18, 0x00, 0x00, 0x00, 0xc0};
            std::copy_n(ieee1905_header, sizeof(ieee1905_header), cmdu_tx_msg.data());

            MAPF_INFO("sending CmduTxMessage: " << std::endl << cmdu_tx_msg);
            ret = bus.publisher().Send(cmdu_tx_msg);
            if (!ret)
                break;
        }

        // Send a large CmduTxMessage on the local bus (helps to manually verify fragmentation code)
        {
            CmduTxMessage cmdu_tx_msg;
            std::copy_n("\x01\x80\xc2\x00\x00\x13", 6, cmdu_tx_msg.metadata()->dst);
            cmdu_tx_msg.metadata()->ether_type = ETH_P_1905_1;
            cmdu_tx_msg.metadata()->msg_type   = 0x8019;
            cmdu_tx_msg.metadata()->length     = 8;
            std::fill_n(cmdu_tx_msg.data(), cmdu_tx_msg.metadata()->length, 0x00);

            uint8_t ieee1905_header[] = {0x00, 0x00, 0x80, 0x19, 0x00, 0x00, 0x00, 0xc0};
            std::copy_n(ieee1905_header, sizeof(ieee1905_header), cmdu_tx_msg.data());

            uint16_t tlvlen = 1024;
            uint8_t *tlv;
            cmdu_tx_msg.metadata()->length += 3 + tlvlen;
            tlv  = cmdu_tx_msg.data() + cmdu_tx_msg.metadata()->length - (3 + tlvlen);
            *tlv = 1;                               // tlvType
            *(uint16_t *)(tlv + 1) = htons(tlvlen); // tlvLength
            std::fill_n(tlv + 3, tlvlen, *tlv);
            tlv = tlv + 3 + tlvlen;

            tlvlen = 0;
            cmdu_tx_msg.metadata()->length += 3 + tlvlen;
            tlv  = cmdu_tx_msg.data() + cmdu_tx_msg.metadata()->length - (3 + tlvlen);
            *tlv = 2;                               // tlvType
            *(uint16_t *)(tlv + 1) = htons(tlvlen); // tlvLength
            std::fill_n(tlv + 3, tlvlen, *tlv);
            tlv = tlv + 3 + tlvlen;

            tlvlen = 1;
            cmdu_tx_msg.metadata()->length += 3 + tlvlen;
            tlv  = cmdu_tx_msg.data() + cmdu_tx_msg.metadata()->length - (3 + tlvlen);
            *tlv = 3;                          // tlvType
            *(uint16_t *)(tlv + 1) = htons(1); // tlvLength
            std::fill_n(tlv + 3, tlvlen, *tlv);
            tlv = tlv + 3 + 1;

            tlvlen = 2;
            cmdu_tx_msg.metadata()->length += 3 + tlvlen;
            tlv  = cmdu_tx_msg.data() + cmdu_tx_msg.metadata()->length - (3 + tlvlen);
            *tlv = 4;                               // tlvType
            *(uint16_t *)(tlv + 1) = htons(tlvlen); // tlvLength
            std::fill_n(tlv + 3, tlvlen, *tlv);
            tlv = tlv + 3 + tlvlen;

            tlvlen = 3;
            cmdu_tx_msg.metadata()->length += 3 + tlvlen;
            tlv  = cmdu_tx_msg.data() + cmdu_tx_msg.metadata()->length - (3 + tlvlen);
            *tlv = 5;                               // tlvType
            *(uint16_t *)(tlv + 1) = htons(tlvlen); // tlvLength
            std::fill_n(tlv + 3, tlvlen, *tlv);
            tlv = tlv + 3 + tlvlen;

            tlvlen = 4;
            cmdu_tx_msg.metadata()->length += 3 + tlvlen;
            tlv  = cmdu_tx_msg.data() + cmdu_tx_msg.metadata()->length - (3 + tlvlen);
            *tlv = 6;                               // tlvType
            *(uint16_t *)(tlv + 1) = htons(tlvlen); // tlvLength
            std::fill_n(tlv + 3, tlvlen, *tlv);
            tlv = tlv + 3 + tlvlen;

            tlvlen = 1000;
            cmdu_tx_msg.metadata()->length += 3 + tlvlen;
            tlv  = cmdu_tx_msg.data() + cmdu_tx_msg.metadata()->length - (3 + tlvlen);
            *tlv = 7;                               // tlvType
            *(uint16_t *)(tlv + 1) = htons(tlvlen); // tlvLength
            std::fill_n(tlv + 3, tlvlen, *tlv);
            tlv = tlv + 3 + tlvlen;

            tlvlen = 0;
            cmdu_tx_msg.metadata()->length += 3 + tlvlen;
            tlv  = cmdu_tx_msg.data() + cmdu_tx_msg.metadata()->length - (3 + tlvlen);
            *tlv = 0;                               // tlvType
            *(uint16_t *)(tlv + 1) = htons(tlvlen); // tlvLength
            std::fill_n(tlv + 3, tlvlen, *tlv);

            MAPF_INFO("sending CmduTxMessage: " << std::endl << cmdu_tx_msg);
            ret = bus.publisher().Send(cmdu_tx_msg);
            if (!ret)
                break;
        }

        // Send a unicast CmduTxMessage on the local bus
        if (use_unicast_address) {
            CmduTxMessage cmdu_tx_msg;
            std::copy_n(unicast_address, 6, cmdu_tx_msg.metadata()->dst);
            cmdu_tx_msg.metadata()->ether_type = ETH_P_1905_1;
            cmdu_tx_msg.metadata()->msg_type   = 0x8020;
            cmdu_tx_msg.metadata()->length     = (8) + (3 + 0);
            std::fill_n(cmdu_tx_msg.data(), cmdu_tx_msg.metadata()->length, 0x00);

            uint8_t ieee1905_header[] = {0x00, 0x00, 0x80, 0x20, 0x00, 0x00, 0x00, 0x80};
            std::copy_n(ieee1905_header, sizeof(ieee1905_header), cmdu_tx_msg.data());

            MAPF_INFO("sending CmduTxMessage: " << std::endl << cmdu_tx_msg);
            ret = bus.publisher().Send(cmdu_tx_msg);
            if (!ret)
                break;
        }

        // Send a multicast CmduTxMessage on the local bus - expect a CmduRxMessage (due to multicast)
        // and a CmduTxConfirmationMessage.
        //Done to verify that all the messages were sent on the local bus before terminating
        do {
            bool got_cmdu_rx_msg              = false;
            bool got_cmdu_tx_confirmation_msg = false;

            CmduTxMessage cmdu_tx_msg;
            std::copy_n("\x01\x80\xc2\x00\x00\x13", 6, cmdu_tx_msg.metadata()->dst);
            cmdu_tx_msg.metadata()->ether_type = ETH_P_1905_1;
            cmdu_tx_msg.metadata()->msg_type   = 0x8016;
            cmdu_tx_msg.metadata()->length     = 32;
            cmdu_tx_msg.metadata()->cookie     = 1;
            std::fill_n(cmdu_tx_msg.data(), cmdu_tx_msg.metadata()->length, 0x00);
            uint8_t ieee1905_header[] = {0x00, 0x00, 0x80, 0x16, 0x00, 0x00, 0x00, 0xc0};
            std::copy_n(ieee1905_header, sizeof(ieee1905_header), cmdu_tx_msg.data());

            MAPF_INFO("sending CmduTxMessage: " << std::endl << cmdu_tx_msg);
            ret = bus.publisher().Send(cmdu_tx_msg);
            if (!ret) {
                MAPF_INFO("failed to send");
                success = false;
                break;
            }

            for (int i = 10; i > 0; i--) {
                MAPF_INFO("polling...");
                rc = poller.Poll(100);
                if (rc == 0) {
                    continue;
                }

                rc = poller.CheckEvent(bus.subscriber());
                if (rc != MAPF_POLLIN) {
                    MAPF_INFO("unexpected event");
                    success = false;
                    break;
                }

                auto msg = bus.subscriber().Receive();

                if (auto cmdu_rx_msg = dynamic_cast<CmduRxMessage *>(msg.get())) {
                    MAPF_INFO("received CmduRxMessage: " << std::endl << *cmdu_rx_msg);
                    if (cmdu_rx_msg->metadata()->length != cmdu_tx_msg.metadata()->length) {
                        MAPF_INFO("message length mismatch");
                    } else if (cmdu_rx_msg->metadata()->ether_type !=
                               cmdu_tx_msg.metadata()->ether_type) {
                        MAPF_INFO("message metadata mismatch");
                    } else if (cmdu_rx_msg->metadata()->msg_type !=
                               cmdu_tx_msg.metadata()->msg_type) {
                        MAPF_INFO("message metadata mismatch");
                    } else {
                        MAPF_INFO("this is the expected message");
                        got_cmdu_rx_msg = true;
                    }
                } else if (auto cmdu_tx_confirmation_msg =
                               dynamic_cast<CmduTxConfirmationMessage *>(msg.get())) {
                    MAPF_INFO("received CmduTxConfirmationMessage: " << std::endl
                                                                     << *cmdu_tx_confirmation_msg);
                    got_cmdu_tx_confirmation_msg = true;
                } else {
                    MAPF_INFO("received other message: " << std::endl << *msg);
                }

                if (got_cmdu_rx_msg && got_cmdu_tx_confirmation_msg) {
                    break;
                }
            }
            if (!(got_cmdu_rx_msg && got_cmdu_tx_confirmation_msg)) {
                MAPF_INFO("did not receive all expected messages");
                success = false;
                break;
            }
        } while (0);
    } while (0);

    usleep(300000);

    MAPF_INFO("terminating forked proccesses...");
    if (ieee1905_transport_pid)
        kill(ieee1905_transport_pid, SIGTERM);
    if (local_bus_pid)
        kill(local_bus_pid, SIGTERM);

    MAPF_INFO("DONE - TEST " << (success ? "PASSED" : "FAILED"));
    mapf_assert(success);

    return success ? 0 : 1;
}
