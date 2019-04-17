#include "../include/mapf/topology_discovery_agent/topology_discovery_agent.h"
#include <mapf/broker/broker.h>
#include <mapf/local_bus.h>
#include <unistd.h>
#include <iostream>

#define ETH0_1_MAC "\x02\x9A\x96\xF3\x75\x40"
#define ETH0_2_MAC "\x02\x9A\x96\xF3\x75\x41"
#define BR_LAN_MAC "\x02\x9A\x96\xF3\x75\x40"
#define BSS_MAC    "\x11\x22\x33\x44\x55\x88"
#define ETH_W_MAC  "\xAC\x9A\x96\xF3\x75\x42"
#define AP_MAC     "\x11\x22\x33\x44\x55\x66"
#define CL_MAC     "\x22\x33\x44\x55\x66\x77"
#define DST_MAC    "\x02\x9A\x96\xF3\x58\x30"

MAPF_INITIALIZE_LOGGER
int main(int argc, char *argv[])
{
	mapf::LocalBusInterface bus(mapf::Context::Instance());
	bus.Init();

	bus.Sync();

	// send topology query
	mapf::CmduTxMessage msg1;
	//		metadata	+		cmdu		+ 		TLVs				+ EndofMsg TLV
	// dst : 1905 multicast MAC		last fragment : 1		AL MAC address, MAC address type TLV
	// src : interface MAC			relay bit     : 0
	// msg type : XXXX
	memcpy(msg1.metadata()->dst, DST_MAC, 6);
	memcpy(msg1.metadata()->src, ETH_W_MAC, 6);
	msg1.metadata()->ether_type = ETH_P_1905_1;
	msg1.metadata()->if_type = 1;
	msg1.metadata()->if_index = if_nametoindex("eth1_w3");
	msg1.metadata()->msg_type = 0x0002;
	uint8_t ieee1905_header1[] = { 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x80 }; // neighbor multicast - relay bit is 0
	msg1.metadata()->length = sizeof(ieee1905_header1) + 3; // end of message tlv : 3
	memcpy(msg1.data(), ieee1905_header1, sizeof(ieee1905_header1));
	memset(msg1.data() + sizeof(ieee1905_header1), 0, 3);
	bus.publisher().Send(msg1);

	// send higher layer query
	msg1.metadata()->msg_type = 0x000D;
	uint8_t ieee1905_header2[] = { 0x00, 0x00, 0x00, 0x0D, 0x00, 0x00, 0x00, 0x80 }; // neighbor multicast - relay bit is 0
	msg1.metadata()->length = sizeof(ieee1905_header2) + 3; // end of message tlv : 3
	memcpy(msg1.data(), ieee1905_header2, sizeof(ieee1905_header2));
	memset(msg1.data() + sizeof(ieee1905_header2), 0, 3);
	bus.publisher().Send(msg1);
	
#if 0
	// send topology notification - client associate
	struct mapf::DiscAgent::topology_notification_format topology_notification;
	memcpy(topology_notification.al_mac_tlv.almac, BR_LAN_MAC, 6);
	memcpy(topology_notification.client_association_tlv.bssid, BSS_MAC, 6);
	topology_notification.client_association_tlv.flags = (1 << 7);
	mapf::Message::Frame data6(sizeof(topology_notification), (const void *)&topology_notification);
	mapf::Message msg6(mapf::da_topics[0], {data6});
	rc = bus.publisher().Send(msg6);

	// send topology notification - client disassociate
	topology_notification.client_association_tlv.flags = 0;
	mapf::Message::Frame data7(sizeof(topology_notification), (const void *)&topology_notification);
	mapf::Message msg7(mapf::da_topics[2], {data7});
	rc = bus.publisher().Send(msg7);
#endif

	return 0;
}
