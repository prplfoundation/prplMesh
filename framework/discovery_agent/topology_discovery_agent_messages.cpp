/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include <mapf/topology_discovery_agent/topology_discovery_agent.h>

namespace mapf {

bool DiscAgent::get_interface_mac_addr(unsigned int if_index, uint8_t *addr)
{
    int sockfd;
    if ((sockfd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL))) == -1) {
        MAPF_ERR("cannot open raw socket \"" << strerror(errno) << "\" (" << errno << ").");
        return false;
    }

    struct ifreq ifr;
    memset(&ifr, 0, sizeof(ifr));

    if (!if_indextoname(if_index, ifr.ifr_name)) {
        MAPF_ERR("cannot find name of interface  " << if_index << ".");
        close(sockfd);
        return false;
    }

    if ((ioctl(sockfd, SIOCGIFHWADDR, &ifr)) < 0) {
        MAPF_ERR("raw socket SIOCGIFHWADDR ioctl failed \"" << strerror(errno) << "\" (" << errno
                                                            << ").");
        close(sockfd);
        return false;
    }
    std::copy_n(ifr.ifr_hwaddr.sa_data, ETH_ALEN, addr);

    MAPF_DBG("address of interface "
             << if_index << " is " << std::hex << std::setfill('0') << std::setw(2)
             << (unsigned)addr[0] << ":" << std::hex << std::setfill('0') << std::setw(2)
             << (unsigned)addr[1] << ":" << std::hex << std::setfill('0') << std::setw(2)
             << (unsigned)addr[2] << ":" << std::hex << std::setfill('0') << std::setw(2)
             << (unsigned)addr[3] << ":" << std::hex << std::setfill('0') << std::setw(2)
             << (unsigned)addr[4] << ":" << std::hex << std::setfill('0') << std::setw(2)
             << (unsigned)addr[5] << "." << std::dec);

    close(sockfd);
    return true;
}

int DiscAgent::TimerSetup()
{
    struct itimerspec timerValue;

    // set timerfd
    timerfd_ = timerfd_create(CLOCK_REALTIME, 0);
    if (timerfd_ < 0) {
        MAPF_ERR("Failed to create timer fd");
        return -1;
    }
    bzero(&timerValue, sizeof(timerValue));
    timerValue.it_value.tv_sec     = DA_DISC_TIMER_INTVL;
    timerValue.it_value.tv_nsec    = 0;
    timerValue.it_interval.tv_sec  = DA_DISC_TIMER_INTVL;
    timerValue.it_interval.tv_nsec = 0;

    /* start timer */
    if (timerfd_settime(timerfd_, 0, &timerValue, NULL) < 0) {
        MAPF_ERR("could not start timer");
        return -1;
    }

    poller_.Add(timerfd_);
    return 0;
}

/*
 *	Main procedure to subscribe and listen on local bus for relevant topic(s)
 *	and act accordingly.
 */
void DiscAgent::Run()
{
    // int	return_value = 0;

    // keep polling local bus forever
    while (1) {
        poller_.Poll();
        // return_value = poller_.Poll();
        //mapf_assert(return_value > 0); // TODO uncomment causes core dump
        auto revents = poller_.CheckEvent(bus_.subscriber());
        if (revents & MAPF_POLLIN) {
            // mapf_assert(return_value == MAPF_POLLIN); // TODO uncomment causes core dump

            //receive the message
            auto message1 = bus_.subscriber().Receive();
            if (auto message2 = dynamic_cast<CmduRxMessage *>(message1.get())) {
                MAPF_DBG("Received cmdu rx message, type : " << message2->metadata()->msg_type);
                // message with ether_type as LLDP and all other subscribed messages with ether_type as ETH_P_1905_1
                if (message2->metadata()->ether_type == ETH_P_LLDP) {
                    HandleLldpDiscovery(message2);
                } else if (message2->metadata()->msg_type == TOPOLOGY_QUERY_MESSAGE) {
                    HandleTopologyQuery(message2);
                } else if (message2->metadata()->msg_type == TOPOLOGY_DISCOVERY_MESSAGE) {
                    HandleTopologyDiscovery(message2);
                } else if (message2->metadata()->msg_type == HIGHER_LAYER_QUERY_MESSAGE) {
                    HandleHigherLayerQuery(message2);
                } else if (message2->metadata()->msg_type == TOPOLOGY_NOTIFICATION_MESSAGE) {
                    struct topology_notification_format *topology_notification =
                        (struct topology_notification_format *)(message2->data() + 8);
                    HandleTopologyNotify(topology_notification);
                }
            } else if (auto message3 = dynamic_cast<InterfaceConfigurationIndicationMessage *>(
                           message1.get())) {
                HandleLocalBusInterfaceIndication(message3->metadata());
            } else {
                auto message = dynamic_cast<Message *>(message1.get());
                MAPF_DBG("Received multi frame message:\n\tMessage: "
                         << message << "\n\tFrame: " << message->frame().str()
                         << "\n\ttopic: " << message->topic());
                /* a. mAP agent sends two types of local bus messages: 1) Topology Interface change 2) Topology WLAN client connect/disconnect 
				 * b. Transport agent receives message 1) and send  local bus message: 3) Transport Interface change
				 * TODO message structure, origin for 2) and 3) yet not known
				 */
                if (message->topic().compare(da_topics[0]) == 0) {
                    HandleTopologyNotify(
                        (struct topology_notification_format *)message->frame().data());
                } else if (message->topic().compare(da_topics[1]) == 0) {
                    HandleWlanApEvent((struct ap_operational_bss *)message->frame().data());
                } else if (message->topic().compare(da_topics[2]) == 0) {
                    HandleClientAssociationEvent(
                        (struct associated_client_list *)message->frame().data());
                } else {
                    MAPF_INFO("Has not subscribed to: " << message->topic());
                }
            }
        } else {
            revents = poller_.CheckEvent(timerfd_);
            if (revents & MAPF_POLLIN) {
                timerselapsed_ = 0;
                (void)read(timerfd_, &timerselapsed_, 8);
                MAPF_DBG("Timer expired from timerfd poll " << timerselapsed_);
                // prepare topology discovery and lldp messages
                HandleTimerEvent();
            }
        }
    }
}

int DiscAgent::HandleLocalBusInterfaceIndication(
    mapf::InterfaceConfigurationIndicationMessage::Metadata *interface_metadata)
{
    int i, return_value = 0;
    struct interface interface_instance = {};
    for (i = 0; i < int(interface_metadata->numInterfaces); i++) {
        interface_instance.index               = interface_metadata->interfaces[i].if_index;
        interface_instance.parent_bridge_index = interface_metadata->interfaces[i].bridge_if_index;
        interface_instance.transport =
            interface_metadata->interfaces[i].flags & Flags::ENABLE_IEEE1905_TRANSPORT;
        interface_instance.ieee802_1bridge =
            interface_metadata->interfaces[i].flags & Flags::IS_BRIDGE;
        get_interface_mac_addr(interface_instance.index, interface_instance.mac);
        return_value = HandleInterfaceEvent(interface_instance);
        if (return_value & (1 << 1)) {
            /* latest added bridge interface MAC is AL MAC
			 * TODO : what in case of multiple bridge interfaces
			 */
            std::copy_n(interface_instance.mac, ETH_ALEN, almac_);
        }
        if (return_value & (1 << 2)) {
            // trigger topology discovery, ifinfo passed by caller should have proper mac,index value
            struct interface_list_item interface_list_instance;
            std::copy_n((uint8_t *)&interface_instance, sizeof(interface_instance),
                        (uint8_t *)&interface_list_instance.instance);
            SendDiscoveryMessage(interface_list_instance);
        }
    }

    return return_value;
}

int DiscAgent::HandleHigherLayerQuery(CmduRxMessage *rxmsg)
{
    uint8_t higher_layer_response_tlv_buffer[MAX_DATA_SIZE] = {0},
            ipv4_address[MAX_IPv4_ADDR_LEN]                 = {0};
    uint8_t ipv6_local_address[MAX_IPv6_ADDR_LEN];
    struct Ieee1905CmduHeader ieee1905_header(HIGHER_LAYER_RESPONSE_MESSAGE,
                                              CMDU_NO_FRAGMENT_INDICATOR);
    int higher_layer_response_tlv_size = 0, return_value, copied_bytes = 0;
    struct higher_layer_response_format higher_layer_response = {};
    CmduTxMessage higher_layer_response_message;

    higher_layer_response.al_mac_tlv.type                  = 1;
    higher_layer_response.al_mac_tlv.length                = 6;
    higher_layer_response.profile_version_tlv.type         = 26;
    higher_layer_response.profile_version_tlv.length       = 1;
    higher_layer_response.profile_version_tlv.profile_ver  = PROFILE_1905_VER;
    higher_layer_response.device_identification_tlv.type   = 21;
    higher_layer_response.device_identification_tlv.length = 0; // TODO calculate
    higher_layer_response.control_url_tlv.type             = 22;

    std::copy_n(almac_, ETH_ALEN, higher_layer_response.al_mac_tlv.almac);
#if 0
	higher_layer_response.device_identification_tlv.friendly_name = kDeviceFriendlyName;
	copied_bytes += higher_layer_response.device_identification_tlv.friendly_name.size();
	higher_layer_response.device_identification_tlv.manufacturer_name = kDeviceManufacturerName;
	copied_bytes += higher_layer_response.device_identification_tlv.manufacturer_name.size();
	higher_layer_response.device_identification_tlv.manufacturer_model = kDeviceManufacturerModel;
	copied_bytes += higher_layer_response.device_identification_tlv.manufacturer_model.size();
#endif

    // TODO assume last 4 octets of AL MAC as IP, until mAP provides this in separate message
    std::copy_n(almac_ + 2, MAX_IPv4_ADDR_LEN, ipv4_address);
    auto url = "http://" + std::to_string(ipv4_address[0]) + '.' + std::to_string(ipv4_address[1]) +
               '.' + std::to_string(ipv4_address[2]) + '.' + std::to_string(ipv4_address[3]) + '/';

    higher_layer_response.control_url_tlv.length = url.size();
    std::copy_n(url.c_str(), url.size(), higher_layer_response.control_url_tlv.url);

    copied_bytes = sizeof(struct higher_layer_response_format) - MAX_URL_LEN +
                   higher_layer_response.control_url_tlv.length;
    std::copy_n((uint8_t *)&higher_layer_response, copied_bytes,
                (uint8_t *)&higher_layer_response_tlv_buffer[higher_layer_response_tlv_size]);
    higher_layer_response_tlv_size += copied_bytes;

    higher_layer_response_tlv_buffer[higher_layer_response_tlv_size++] = 1;
    std::copy_n(almac_, ETH_ALEN,
                (uint8_t *)&higher_layer_response_tlv_buffer[higher_layer_response_tlv_size]);
    higher_layer_response_tlv_size += ETH_ALEN;
    higher_layer_response_tlv_buffer[higher_layer_response_tlv_size++] = 1; // assume 1 IP address
    higher_layer_response_tlv_buffer[higher_layer_response_tlv_size++] = 0; // address type:unknown
    std::copy_n(ipv4_address, MAX_IPv4_ADDR_LEN,
                (uint8_t *)&higher_layer_response_tlv_buffer
                    [higher_layer_response_tlv_size]); // IP address in octet format
    higher_layer_response_tlv_size += MAX_IPv4_ADDR_LEN;
    memset((uint8_t *)&higher_layer_response_tlv_buffer[higher_layer_response_tlv_size], 0,
           MAX_IPv4_ADDR_LEN); // DHCP server unknown:all 0's
    higher_layer_response_tlv_size += MAX_IPv4_ADDR_LEN;

    // TODO assume last 3 octets of AL MAC as local IPv6 IP, until mAP provides this in separate message
    higher_layer_response_tlv_buffer[higher_layer_response_tlv_size++] = 1;

    std::copy_n(std::array<uint8_t, 16>({0xfe, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x9a,
                                         0x96, 0xff, 0xfe, 0xf3, 0x58, 0x30})
                    .data(),
                13, ipv6_local_address); // array contain 16, but we copy only 13

    std::copy_n(almac_ + 3, 3, ipv6_local_address + 13);
    std::copy_n(almac_, ETH_ALEN,
                (uint8_t *)&higher_layer_response_tlv_buffer[higher_layer_response_tlv_size]);
    higher_layer_response_tlv_size += ETH_ALEN;

    //		metadata	+		cmdu		+ 		TLVs				+ EndofMsg TLV
    // dst : 1905 multicast MAC		last fragment : 1		AL MAC address, MAC address type TLV
    // src : interface MAC			relay bit     : 0
    // msg type : 0000

    std::copy_n(rxmsg->metadata()->src, ETH_ALEN, higher_layer_response_message.metadata()->dst);
    std::copy_n(rxmsg->metadata()->dst, ETH_ALEN, higher_layer_response_message.metadata()->src);

    higher_layer_response_message.metadata()->ether_type = ETH_P_1905_1;
    higher_layer_response_message.metadata()->msg_type   = HIGHER_LAYER_RESPONSE_MESSAGE;
    higher_layer_response_message.metadata()->if_type    = rxmsg->metadata()->if_type;
    higher_layer_response_message.metadata()->if_index   = rxmsg->metadata()->if_index;

    higher_layer_response_message.metadata()->length =
        sizeof(ieee1905_header) + higher_layer_response_tlv_size + 3; // end of message tlv : 3

    std::copy_n((uint8_t *)&ieee1905_header, sizeof(ieee1905_header),
                higher_layer_response_message.data());
    std::copy_n(higher_layer_response_tlv_buffer, higher_layer_response_tlv_size,
                higher_layer_response_message.data() + sizeof(ieee1905_header));

    memset(higher_layer_response_message.data() + sizeof(ieee1905_header) +
               higher_layer_response_tlv_size,
           0, 3);

    return_value = bus_.publisher().Send(higher_layer_response_message);

    return return_value;
}

/* check if exec is in between timer expiry, if so then
 * check if this host is already known, if not record the same with bridge exist by default
 */
int DiscAgent::HandleTopologyDiscovery(CmduRxMessage *rxmsg)
{
    int return_value = 0;
    struct topology_discovery_format *topology_discovery =
        (struct topology_discovery_format *)(rxmsg->data() + 8);

    return_value =
        UpdateNeighborEntry(rxmsg->metadata()->if_index, NULL, topology_discovery->al_mac_tlv.almac,
                            NEIGHBOR_1905, 1); // oper is add

    return return_value;
}

/* check if exec is in between timer expiry, if so then
 * check if this host is known, if so then mark no bridge exist otherwise as yes
 * bridge device will not forward lldp packet and 1905 device will forward. so,
 * upon receiving lldp discovery set bridge exist flag as false
 */
int DiscAgent::HandleLldpDiscovery(CmduRxMessage *rxmsg)
{
    int return_value                             = 0;
    struct lldp_discovery_format *lldp_discovery = (struct lldp_discovery_format *)rxmsg->data();

    return_value =
        UpdateNeighborEntry(rxmsg->metadata()->if_index, NULL, lldp_discovery->port_id.id,
                            NEIGHBOR_1905, 2); // oper is update

    return return_value;
}

int DiscAgent::HandleTimerEvent()
{
    uint8_t testmac[ETH_ALEN] = {0};

    if (memcmp(almac_, testmac, ETH_ALEN) == 0) {
        // no bridge details available
        MAPF_ERR("AL MAC details not available. Unable to send topology discovery message.");
        return -1;
    }

    // for each known interface from intf_node
    for (auto it : interface_list) {
        SendDiscoveryMessage(it);
    }

    return 0;
}

int DiscAgent::SendDiscoveryMessage(struct interface_list_item interfacelistitem)
{
    int return_value = 0;
    struct Ieee1905CmduHeader ieee1905_header(TOPOLOGY_DISCOVERY_MESSAGE,
                                              CMDU_NO_FRAGMENT_INDICATOR);
    struct topology_discovery_format topology_discovery = {};
    struct lldp_discovery_format lldp_discovery         = {};
    CmduTxMessage lldp_discovery_message, topology_discovery_message;
    // TODO to be sent only for transport interfaces
    std::copy_n(std::array<uint8_t, 16>({0x01, 0x80, 0xc2, 0x00, 0x00, 0x0e}).data(), ETH_ALEN,
                lldp_discovery_message.metadata()->dst);
    std::copy_n(interfacelistitem.instance.mac, ETH_ALEN, lldp_discovery_message.metadata()->src);
    lldp_discovery_message.metadata()->ether_type = ETH_P_LLDP;
    lldp_discovery_message.metadata()->if_type    = CmduXxMessage::IF_TYPE_NET;
    lldp_discovery_message.metadata()->if_index   = interfacelistitem.instance.index;
    lldp_discovery_message.metadata()->length     = sizeof(lldp_discovery);
    memset(lldp_discovery_message.data(), 0, lldp_discovery_message.metadata()->length);

    lldp_discovery.chassis_id.type    = 2;
    lldp_discovery.chassis_id.length  = 7;
    lldp_discovery.chassis_id.subtype = 4;
    lldp_discovery.port_id.type       = 4;
    lldp_discovery.port_id.length     = 7;
    lldp_discovery.port_id.subtype    = 3;
    lldp_discovery.ttl.type           = 6;
    lldp_discovery.ttl.length         = 2;
    lldp_discovery.ttl.value          = 1800;
    std::copy_n(almac_, ETH_ALEN, lldp_discovery.chassis_id.id);
    std::copy_n(interfacelistitem.instance.mac, ETH_ALEN, lldp_discovery.port_id.id);
    std::copy_n((uint8_t *)&lldp_discovery, sizeof(lldp_discovery), lldp_discovery_message.data());
    return_value = bus_.publisher().Send(lldp_discovery_message);

    //		metadata	+		cmdu		+ 		TLVs				+ EndofMsg TLV
    // dst : 1905 multicast MAC		last fragment : 1		AL MAC address, MAC address type TLV
    // src : interface MAC			relay bit     : 0
    // msg type : 0000

    std::copy_n(std::array<uint8_t, 16>({0x01, 0x80, 0xc2, 0x00, 0x00, 0x13}).data(), ETH_ALEN,
                topology_discovery_message.metadata()->dst);
    std::copy_n(interfacelistitem.instance.mac, ETH_ALEN,
                topology_discovery_message.metadata()->src);
    topology_discovery_message.metadata()->ether_type = ETH_P_1905_1;
    // topology_discovery_message.metadata()->msg_type = TOPOLOGY_DISCOVERY_MESSAGE;
    topology_discovery_message.metadata()->if_type  = CmduXxMessage::IF_TYPE_NET;
    topology_discovery_message.metadata()->if_index = interfacelistitem.instance.index;

    topology_discovery_message.metadata()->length =
        sizeof(ieee1905_header) + sizeof(topology_discovery) + 3; // end of message tlv : 3

    topology_discovery.al_mac_tlv.type        = 1;
    topology_discovery.al_mac_tlv.length      = 6;
    topology_discovery.mac_address_tlv.type   = 2;
    topology_discovery.mac_address_tlv.length = 6;
    std::copy_n(almac_, ETH_ALEN, topology_discovery.al_mac_tlv.almac);
    std::copy_n(interfacelistitem.instance.mac, ETH_ALEN, topology_discovery.mac_address_tlv.ifmac);
    std::copy_n((uint8_t *)&ieee1905_header, 8, topology_discovery_message.data());
    std::copy_n((uint8_t *)&topology_discovery, sizeof(topology_discovery),
                topology_discovery_message.data() + 8);
    memset(topology_discovery_message.data() + sizeof(ieee1905_header) + sizeof(topology_discovery),
           0, 3);

    return_value = bus_.publisher().Send(topology_discovery_message);

    return return_value;
}

// TODO implement section 8.1

int DiscAgent::HandleTopologyQuery(CmduRxMessage *rxmsg)
{
    int i, j, topology_response_tlv_size = 0, bridge_index_list[MAX_BR_COUNT] = {-1};
    struct Ieee1905CmduHeader ieee1905_header(TOPOLOGY_RESPONSE_MESSAGE,
                                              CMDU_NO_FRAGMENT_INDICATOR);
    // TODO optimize stack size
    char neighbor_1905_tlv[MAX_DATA_SIZE] = {0}, neighbor_non_1905_tlv[MAX_DATA_SIZE] = {0};
    uint8_t topology_response_tlv[MAX_DATA_SIZE] = {0}, ap_operational_bss_tlv[MAX_DATA_SIZE] = {0},
            associated_clients_tlv[MAX_DATA_SIZE] = {0};
    uint8_t type, interface_count, mac_buffer[MAX_DATA_SIZE] = {0}, mac_buffer_size = 0;
    char ssid_length;
    uint16_t neighbor_1905_tlv_size, neighbor_non_1905_tlv_size, ap_operational_bss_tlv_size,
        associated_clients_tlv_size;
    uint16_t number_of_map_services, tlv_length;
    // struct device_info_tlv device_info;
    CmduTxMessage topology_response_message;

    // assumed that dst MAC matches this device's AL MAC since query is unicast packet

    // prepare topology response TLVs
    // device information type
    type            = 3;
    tlv_length      = 10;
    interface_count = (uint8_t)interface_list.size();
    std::copy_n((uint8_t *)&type, sizeof(type), topology_response_tlv);
    topology_response_tlv_size += 1;
    std::copy_n((uint8_t *)&tlv_length, sizeof(tlv_length),
                topology_response_tlv + topology_response_tlv_size);
    topology_response_tlv_size += 2;
    std::copy_n(&almac_, ETH_ALEN, topology_response_tlv + topology_response_tlv_size);
    topology_response_tlv_size += ETH_ALEN;
    std::copy_n((uint8_t *)&interface_count, sizeof(interface_count),
                topology_response_tlv + topology_response_tlv_size);
    topology_response_tlv_size += 1;
    i = 0;
    for (auto it : interface_list) {
        std::copy_n(it.instance.mac, ETH_ALEN, topology_response_tlv + topology_response_tlv_size);
        topology_response_tlv_size += ETH_ALEN;
        std::copy_n((uint8_t *)&it.instance.media_type, 2,
                    topology_response_tlv + topology_response_tlv_size);
        topology_response_tlv_size += 2;
        memset(topology_response_tlv + topology_response_tlv_size, 0, 2);
        topology_response_tlv_size += 2;
        // TODO identify media specific information based on information name or take this input as part of interface status message
        i++;
    }

    // device bridging capability
    /* 1. if fwk sends bridge interface update as part of normal interface notification then
	 *	fwk has to set ieee802_1bridge flag and here we need to maintain this bridge name in local array.
	 * 2. else maintain and store bridge interface linked list with slave interfaces' details copy.
	 * Option 1 assumed and followed. TODO clarify.
	 */
    interface_count                                     = 0;
    topology_response_tlv[topology_response_tlv_size++] = 4; // device bridging capability tlv type
    for (auto it : interface_list) {
        if (it.instance.parent_bridge_index != -1) {
            // TODO assume as single bridge
            std::copy_n(it.instance.mac, ETH_ALEN, mac_buffer + mac_buffer_size);
            mac_buffer_size += ETH_ALEN;
            interface_count++;
        } else {
            /* TODO assume as single bridge
			 * for multiple bridges, use C++ constructs to handle dynamic list of
			 * bridge index, bridge mac and their members mac
			 * list brmaclist
			 * bridx     brmac         membermac
			 */
            bridge_index_list[i++] = it.instance.index;

            // TODO: Remove "unused variable" warning
            (void)bridge_index_list;
        }
    }
    tlv_length = (interface_count * ETH_ALEN) + 2;
    std::copy_n((uint8_t *)&tlv_length, sizeof(tlv_length),
                topology_response_tlv + topology_response_tlv_size);
    topology_response_tlv_size += 2; //skip 2 octets for length, to be filled later
    topology_response_tlv[topology_response_tlv_size] = 1;
    topology_response_tlv_size += 1;
    topology_response_tlv[topology_response_tlv_size] =
        interface_count; // skip 1 octet for number of MAC addresses in bridging tuple
    topology_response_tlv_size += 1;
    std::copy_n(mac_buffer, mac_buffer_size, topology_response_tlv + topology_response_tlv_size);
    topology_response_tlv_size += mac_buffer_size;

    // non-1905 neighbor device list
    // 1905.1 neighbor device list
    neighbor_1905_tlv_size = neighbor_non_1905_tlv_size = 0;
    for (auto it1 : interface_list) {
        std::copy_n(it1.instance.mac, ETH_ALEN, neighbor_1905_tlv + neighbor_1905_tlv_size);
        neighbor_1905_tlv_size += ETH_ALEN;
        std::copy_n(it1.instance.mac, ETH_ALEN, neighbor_non_1905_tlv + neighbor_non_1905_tlv_size);
        neighbor_non_1905_tlv_size += ETH_ALEN;
        for (auto it2 : it1.neighbor_list) {
            if (it2.type == NEIGHBOR_1905) {
                //neighbor_1905_tlv = (char *)realloc(neighbor_1905_tlv_size + 2 * ETH_ALEN + 1);
                std::copy_n(it2.mac, ETH_ALEN, neighbor_1905_tlv + neighbor_1905_tlv_size);
                neighbor_1905_tlv_size += ETH_ALEN;
                if (it2.bridge_exists) {
                    neighbor_1905_tlv[neighbor_1905_tlv_size] = 0x10;
                    neighbor_1905_tlv_size += 1;
                }
            }
            if (it2.type == NEIGHBOR_NON_1905) {
                //neighbor_non_1905_tlv = (char *)realloc(neighbor_non_1905_tlv_size + 2 * ETH_ALEN);
                std::copy_n(it2.mac, ETH_ALEN, neighbor_non_1905_tlv + neighbor_non_1905_tlv_size);
                neighbor_non_1905_tlv_size += ETH_ALEN;
            }
        }
    }

    topology_response_tlv[topology_response_tlv_size++] =
        6; // non-1905 neighbor device list tlv type
    std::copy_n((uint8_t *)&neighbor_non_1905_tlv_size, sizeof(neighbor_non_1905_tlv_size),
                &topology_response_tlv
                    [topology_response_tlv_size]); // non-1905 neighbor device list tlv data length
    topology_response_tlv_size += 2;
    std::copy_n(neighbor_non_1905_tlv, neighbor_non_1905_tlv_size,
                &topology_response_tlv[topology_response_tlv_size]); // tlv data
    topology_response_tlv_size += neighbor_non_1905_tlv_size;

    topology_response_tlv[topology_response_tlv_size++] = 7; // 1905.1 neighbor device list tlv type
    std::copy_n((uint8_t *)&neighbor_1905_tlv_size, sizeof(neighbor_1905_tlv_size),
                &topology_response_tlv
                    [topology_response_tlv_size]); // 1905 neighbor device list tlv data length
    topology_response_tlv_size += 2;
    std::copy_n(neighbor_1905_tlv, neighbor_1905_tlv_size,
                &topology_response_tlv[topology_response_tlv_size]); // tlv data
    topology_response_tlv_size += neighbor_1905_tlv_size;

    // SupportedService TLV
    topology_response_tlv[topology_response_tlv_size++] = 0x80;
    number_of_map_services                              = 3;
    std::copy_n((uint8_t *)&number_of_map_services, sizeof(number_of_map_services),
                &topology_response_tlv[topology_response_tlv_size]); // 3 octets as put below
    topology_response_tlv_size += 2;
    topology_response_tlv[topology_response_tlv_size++] = 2;    // 2 services
    topology_response_tlv[topology_response_tlv_size++] = 0x00; // Multi-AP Controller
    topology_response_tlv[topology_response_tlv_size++] = 0x01; // Multi-AP Agent

    // prepare AP Operational BSS TLV
    memset(ap_operational_bss_tlv, 0, sizeof(ap_operational_bss_tlv));
    ap_operational_bss_tlv_size                         = 0;
    ap_operational_bss_tlv[ap_operational_bss_tlv_size] = ap_bss_info_.ap_count;
    ap_operational_bss_tlv_size += 1;
    for (i = 0; i < ap_bss_info_.ap_count; i++) {
        std::copy_n(ap_bss_info_.ap_list[i].apmac, ETH_ALEN,
                    &ap_operational_bss_tlv[ap_operational_bss_tlv_size]);
        ap_operational_bss_tlv_size += ETH_ALEN;
        ap_operational_bss_tlv[ap_operational_bss_tlv_size] = ap_bss_info_.ap_list[i].bss_count;
        ap_operational_bss_tlv_size += 1;
        for (j = 0; j < ap_bss_info_.ap_list[i].bss_count; j++) {
            std::copy_n(ap_bss_info_.ap_list[i].bss_list[j].bssmac, ETH_ALEN,
                        &ap_operational_bss_tlv[ap_operational_bss_tlv_size]);
            ap_operational_bss_tlv_size += ETH_ALEN;
            ssid_length = std::string((ap_bss_info_.ap_list[i].bss_list[j].ssid)).size();
            ap_operational_bss_tlv[ap_operational_bss_tlv_size] = ssid_length;
            ap_operational_bss_tlv_size += 1;
            std::copy_n(ap_bss_info_.ap_list[i].bss_list[j].ssid, ssid_length,
                        &ap_operational_bss_tlv[ap_operational_bss_tlv_size]);
            ap_operational_bss_tlv_size += ssid_length;
        }
    }
    topology_response_tlv[topology_response_tlv_size++] = 0x83; // AP Operational BSS TLV type
    std::copy_n(
        (uint8_t *)&ap_operational_bss_tlv_size, sizeof(ap_operational_bss_tlv_size),
        &topology_response_tlv[topology_response_tlv_size]); // AP Operational BSS TLV data length
    topology_response_tlv_size += 2;
    std::copy_n(ap_operational_bss_tlv, ap_operational_bss_tlv_size,
                &topology_response_tlv[topology_response_tlv_size]); // tlv data
    topology_response_tlv_size += ap_operational_bss_tlv_size;

    // prepare Associated Clients TLV
    associated_clients_tlv_size                         = 0;
    associated_clients_tlv[associated_clients_tlv_size] = associated_client_info_.bss_count;
    associated_clients_tlv_size += 1;
    for (i = 0; i < associated_client_info_.bss_count; i++) {
        std::copy_n(associated_client_info_.bss_list[i].bssmac, ETH_ALEN,
                    &associated_clients_tlv[associated_clients_tlv_size]);
        associated_clients_tlv_size += ETH_ALEN;
        std::copy_n((uint8_t *)&associated_client_info_.bss_list[i].client_count,
                    sizeof(associated_client_info_.bss_list[i].client_count),
                    &associated_clients_tlv[associated_clients_tlv_size]);
        associated_clients_tlv_size += 2;
        std::copy_n((uint8_t *)associated_client_info_.bss_list[i].client_list,
                    associated_client_info_.bss_list[i].client_count *
                        sizeof(associated_client_info_.bss_list[i]),
                    &associated_clients_tlv[associated_clients_tlv_size]);
        associated_clients_tlv_size += associated_client_info_.bss_list[i].client_count *
                                       sizeof(associated_client_info_.bss_list[i].client_list[0]);
    }
    topology_response_tlv[topology_response_tlv_size++] = 0x84; // Associated Clients TLV type
    std::copy_n(
        (uint8_t *)&associated_clients_tlv_size, sizeof(associated_clients_tlv_size),
        &topology_response_tlv[topology_response_tlv_size]); // Associated Clients TLV data length
    topology_response_tlv_size += 2;
    std::copy_n(associated_clients_tlv, associated_clients_tlv_size,
                &topology_response_tlv[topology_response_tlv_size]); // tlv data
    topology_response_tlv_size += associated_clients_tlv_size;

    //		metadata	+		cmdu		+ 		TLVs				+ EndofMsg TLV
    // dst : 1905 multicast MAC		last fragment : 1		AL MAC address, MAC address type TLV
    // src : interface MAC			relay bit     : 0
    // msg type : 0000

    std::copy_n(rxmsg->metadata()->src, ETH_ALEN, topology_response_message.metadata()->dst);
    std::copy_n(rxmsg->metadata()->dst, ETH_ALEN, topology_response_message.metadata()->src);
    topology_response_message.metadata()->ether_type = ETH_P_1905_1;
    topology_response_message.metadata()->msg_type   = TOPOLOGY_RESPONSE_MESSAGE;
    topology_response_message.metadata()->if_type    = CmduXxMessage::IF_TYPE_NET;
    topology_response_message.metadata()->if_index =
        rxmsg->metadata()->if_index; // TODO check if correct or to use almac_ ?

    topology_response_message.metadata()->length =
        sizeof(ieee1905_header) + topology_response_tlv_size + 3; // end of message tlv : 3

    std::copy_n((uint8_t *)&ieee1905_header, sizeof(ieee1905_header),
                topology_response_message.data());
    std::copy_n(topology_response_tlv, topology_response_tlv_size,
                topology_response_message.data() + sizeof(ieee1905_header));

    memset(topology_response_message.data() + sizeof(ieee1905_header) + topology_response_tlv_size,
           0, 3);

    bus_.publisher().Send(topology_response_message);

    return 0;
}

int DiscAgent::HandleWlanApEvent(struct ap_operational_bss *rx_ap_bss_info)
{
    // TODO invoke member function of TopologyDiscoveryAgentLocalDB so that it's stored in that class
    ap_bss_info_.ap_count = rx_ap_bss_info->ap_count;
    std::copy_n((uint8_t *)&rx_ap_bss_info->ap_list,
                rx_ap_bss_info->ap_count * sizeof(ap_bss_info_.ap_list[0]),
                (uint8_t *)&ap_bss_info_.ap_list);

    return 0;
}

int DiscAgent::HandleClientAssociationEvent(
    struct associated_client_list *rx_associated_client_info)
{
    // TODO invoke member function of TopologyDiscoveryAgentLocalDB so that it's stored in that class
    associated_client_info_.bss_count = rx_associated_client_info->bss_count;
    std::copy_n((uint8_t *)&rx_associated_client_info->bss_list,
                rx_associated_client_info->bss_count *
                    sizeof(rx_associated_client_info->bss_list[0]),
                (uint8_t *)&associated_client_info_.bss_list);

    return 0;
}

int DiscAgent::HandleTopologyNotify(struct topology_notification_format *topology_notification)
{
    int return_value               = 0;
    uint8_t neighbor_mac[ETH_ALEN] = {0};

    std::copy_n(topology_notification->al_mac_tlv.almac, ETH_ALEN, neighbor_mac);

#if 0 // relay multicast handling is part of transport agent
	if(iflist_head != NULL) {
		t_node = iflist_head;
		while(t_node != NULL) {
			// generic multicast, no need to specify src details
			memcpy(msg.metadata()->dst, MCAST_ADDR_1905, ETH_ALEN);
			//memcpy(msg.metadata()->src, t_node->ifinfo.mac, ETH_ALEN);
			msg.metadata()->ether_type = ETH_P_1905_1;
			msg.metadata()->msg_type = TOPOLOGY_NOTIFICATION_MESSAGE;
			msg.metadata()->relay = 1; // relayed multicast
			ifindex_t = t_node->ifinfo.index;
			if(ifindex_t == -1 || (ifindex != -1 && ifindex_t == ifindex)) {
				// skip sending topology notification message on rx interface
				continue;
			}
			msg.metadata()->if_type = CmduXxMessage::IF_TYPE_NET;
			msg.metadata()->if_index = ifindex_t;
			msg.metadata()->length = sizeof(ieee1905_header) + 8 + 3;
			// use topology discovery pdu but only first 8 bytes needed for AL MAC address TLV
			memset(&topology_notify, 0, sizeof(topology_notify));
			topology_notify.type1 = 1;
			topology_notify.len1 = 6;
			memcpy(topology_notify.almac, mac, sizeof(mac));
			memcpy(msg.data(), ieee1905_header, sizeof(ieee1905_header));
			memcpy(msg.data() + sizeof(ieee1905_header), (void *)&topology_notify, 8);
			memset(msg.data() + sizeof(ieee1905_header) + 8, 0, 3);
			rc = bus_.publisher().Send(msg);
			t_node = t_node->next;
		}
	}
#endif

    // store neighbor details if notification was generated from local device (matching AL MAC)
    if (memcmp(neighbor_mac, almac_, ETH_ALEN) == 0) {
        return_value = UpdateNeighborEntry(
            -1, topology_notification->client_association_tlv.bssid, neighbor_mac, NEIGHBOR_1905,
            (topology_notification->client_association_tlv.flags >> 7));
        for (auto it = interface_list.begin(); it != interface_list.end(); ++it) {
            TopologyDiscoveryAgentLocalDB::ShowNeighborList(it);
        }
    }

    return return_value;
}

} // namespace mapf
