#include "../include/mapf/topology_discovery_agent/topology_discovery_agent.h"
#include <mapf/broker/broker.h>
#include <mapf/local_bus.h>
#include <unistd.h>
#include <iostream>

#define ETH0_1_MAC "\x02\x9A\x96\xF3\x58\x30"
#define ETH0_2_MAC "\x02\x9A\x96\xF3\x58\x31"
#define BR_LAN_MAC "\x02\x9A\x96\xF3\x58\x30"
#define BSS_MAC    "\x11\x22\x33\x44\x55\x88"
#define AP_MAC     "\x11\x22\x33\x44\x55\x66"
#define CL_MAC     "\x22\x33\x44\x55\x66\x77"
#define DST_MAC    "\x02\x9A\x96\xF3\x58\x30"

MAPF_INITIALIZE_LOGGER
int main(int argc, char *argv[])
{
	int i, j;

	mapf::LocalBusInterface bus(mapf::Context::Instance());
	bus.Init();

	bus.Sync();

	// WLAN AP up event
	struct mapf::DiscAgent::ap_operational_bss apopbss_in;
	apopbss_in.ap_count = 2;
	for(i=0; i<apopbss_in.ap_count; i++) {
		memcpy(apopbss_in.ap_list[i].apmac, AP_MAC, ETH_ALEN);
		apopbss_in.ap_list[i].bss_count = 2;
		for(j=0; j<apopbss_in.ap_list[i].bss_count; j++) {
			memcpy(apopbss_in.ap_list[i].bss_list[j].bssmac, BSS_MAC, ETH_ALEN);
			snprintf(apopbss_in.ap_list[i].bss_list[j].ssid, 9, "%s", "test_ssid");
		}
	}
	mapf::Message::Frame data8(sizeof(apopbss_in), (const void *)&apopbss_in);
	mapf::Message msg8(mapf::da_topics[1], {data8});
	bus.publisher().Send(msg8);

	// Client associated event
	struct mapf::DiscAgent::associated_client_list assoccl_in;
	assoccl_in.bss_count = 2;
	for(i=0; i<assoccl_in.bss_count; i++) {
		memcpy(assoccl_in.bss_list[i].bssmac, BSS_MAC, ETH_ALEN);
		assoccl_in.bss_list[i].client_count = 1;
		memcpy(assoccl_in.bss_list[i].client_list[0].clmac, CL_MAC, ETH_ALEN);
		assoccl_in.bss_list[i].client_list[0].associated_time = 300;
	}
	mapf::Message::Frame data9(sizeof(assoccl_in), (const void *)&assoccl_in);
	mapf::Message msg9(mapf::da_topics[2], {data9});
	bus.publisher().Send(msg9);

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
