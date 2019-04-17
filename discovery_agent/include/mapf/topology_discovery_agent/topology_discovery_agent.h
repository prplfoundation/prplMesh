#ifndef _MAP_DISCOVERY_AGENT_H_
#define _MAP_DISCOVERY_AGENT_H_

#include <mapf/common/socket.h>
#include <mapf/transport/ieee1905_transport.h>
#include <mapf/transport/ieee1905_transport_messages.h>
#include <iostream>
#include <net/if.h>
#include <signal.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/timerfd.h>
#include <sys/poll.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <netinet/ether.h>
#include <list>
#include <mapf/topology_discovery_agent/topology_discovery_agent_common.h>
#include <mapf/topology_discovery_agent/topology_discovery_agent_database.h>
//#include <linux/ipv6.h>

/* CMDU format

For LLDP CMDU,
	metadata + data{TLVs}
Others,
	metadata + data{1905 header (refer Table 6-3) + TLVs}

For topology notification,
	- send AL MAC address type TLV
	- on each known interface with relay bit set to 1

1905 multicast MAC address : 01:80:c2:00:00:13
LLDP multicast MAC address : 01:80:c2:00:00:0e

*/
namespace mapf
{

class DiscAgent : public TopologyDiscoveryAgentLocalDB
{
public:
	struct sub_device_info {
		uint8_t	mac[ETH_ALEN];
		uint16_t media_type;
		uint16_t length;
		//char	*med_spec; TODO not available
	};

	struct device_info_tlv {
		uint8_t	type;
		uint16_t length;
		uint8_t	al_mac[ETH_ALEN];
		uint8_t	interface_count;
		struct sub_device_info *sub_device;
	};

	struct lldp_discovery_format {
		struct {
			uint8_t    type; // bit 2 set
			uint8_t    length;
			uint8_t    subtype; // indicates chassid ID is MAC address (1905.1 AL MAC address)
			uint8_t    id[ETH_ALEN]; // 6 octets
		} chassis_id;
		struct {
			uint8_t    type; // bit 3 set
			uint8_t    length;
			uint8_t    subtype; // indicates chassid ID is MAC address (interface's MAC address)
			uint8_t    id[ETH_ALEN]; // 6 octets
		} port_id;
		struct {
			uint8_t    type; // bits 2, 3 set
			uint8_t    length;
			uint16_t   value; // defined in Sec 6.1 of IEEE_Std_1905.1-2013.pdf
		} ttl;
		uint8_t end[2] = {0};
	};

	struct topology_discovery_format {
		struct {
			uint8_t	type;
			uint8_t	length;
			uint8_t	almac[ETH_ALEN];
		} al_mac_tlv;
		struct {
			uint8_t	type;
			uint8_t	length;
			uint8_t	ifmac[ETH_ALEN];
		} mac_address_tlv;
	};

	struct topology_notification_format {
		struct {
			uint8_t	type;
			uint8_t	length;
			uint8_t	almac[ETH_ALEN];
		} al_mac_tlv;
		struct {
			uint8_t	type;
			uint16_t length;
			uint8_t	cilent_mac[ETH_ALEN];
			uint8_t bssid[ETH_ALEN];
			uint8_t	flags;
		} client_association_tlv;
	};

	struct higher_layer_response_format {
		struct {
			uint8_t	type;
			uint8_t	length;
			uint8_t	almac[ETH_ALEN];
		} al_mac_tlv;
		struct {
			uint8_t	type;
			uint16_t length;
			uint8_t	profile_ver; // 0x00:1905.1, 0x01:1905.1a, 0x02~0xFF:reserved
		} profile_version_tlv;
		struct {
			uint8_t	type;
			uint16_t length;
			char friendly_name[MAX_NAME_LEN];
			char manufacturer_name[MAX_NAME_LEN];
			char manufacturer_model[MAX_NAME_LEN];
		} device_identification_tlv;
		struct {
			uint8_t  type;
			uint16_t length;
			char	 url[MAX_URL_LEN]; // formed from AL interface IP address
		} control_url_tlv;
	};

        struct ap_operational_bss {
            uint8_t ap_count;
            struct {
                uint8_t apmac[ETH_ALEN];
                uint8_t bss_count;
                struct {
                    uint8_t bssmac[ETH_ALEN];
                    char	ssid[MAX_SSID_LEN];
                } bss_list[MAX_BSS];
            } ap_list[MAX_AP];
        };

        struct associated_client_list {
            uint8_t bss_count;
            struct {
                uint8_t bssmac[ETH_ALEN];
                int     client_count;
                struct {
                    uint8_t clmac[ETH_ALEN];        
                    int     associated_time;
                } client_list[MAX_CL];
            } bss_list[MAX_BSS];
        };

	struct Ieee1905CmduHeader {
		uint8_t messageVersion;
		uint8_t reservedField0;
		uint16_t messageType;
		uint16_t messageId;
		uint8_t fragmentId;
		uint8_t flags;
		Ieee1905CmduHeader(uint16_t messageTypeToSet, uint8_t flagsToSet)
		{
			messageVersion =  reservedField0 = fragmentId = flags = 0;
			messageType = messageId = 0;
			if(messageTypeToSet != 0) {
				this->messageType |= messageTypeToSet;
			}
			if(flagsToSet != 0) {
				this->flags |= flagsToSet;
			}
		}
	};

	DiscAgent(): bus_(Context::Instance())
	{
		int rc = 0, i =0;

		timerfd_ = 0;
		timerselapsed_ = 0;
		memset(almac_, 0, sizeof(almac_));
		memset(&ap_bss_info_, 0, sizeof(ap_bss_info_));
		memset(&associated_client_info_, 0, sizeof(associated_client_info_));

		// connect to local bus interface, subscribe to topic(s)
		bus_.Init();
		for(i=0; i<10; i++) {
			rc = bus_.subscriber().Subscribe(da_topics[i]);
			mapf_assert(rc == 0);
		}
		rc = bus_.subscriber().Subscribe<InterfaceConfigurationIndicationMessage>();
		rc = bus_.subscriber().Subscribe<CmduRxMessage>(CmduRxMessage::ieee1905_topic(TOPOLOGY_QUERY_MESSAGE));
		rc = bus_.subscriber().Subscribe<CmduRxMessage>(CmduRxMessage::ieee1905_topic(TOPOLOGY_NOTIFICATION_MESSAGE));
		rc = bus_.subscriber().Subscribe<CmduRxMessage>(CmduRxMessage::ieee1905_topic(TOPOLOGY_DISCOVERY_MESSAGE));
		rc = bus_.subscriber().Subscribe<CmduRxMessage>(CmduRxMessage::lldp_topic());
		rc = bus_.subscriber().Subscribe<CmduRxMessage>(CmduRxMessage::ieee1905_topic(HIGHER_LAYER_QUERY_MESSAGE));
		mapf_assert(rc == 0);
		bus_.Sync();

		// add a poller and poll to see if the message arrived
		rc = poller_.Add(bus_.subscriber());
		if(rc != 0) {
			MAPF_ERR("Poller registration on subscriber failed.");
			return;
		}

		// start thread for recurring discovery procedure setup
		rc = TimerSetup();
		mapf_assert(rc == 0);

		// if volatile db exists read contents
		// TODO
	}
	~DiscAgent() {}

	void Run();

protected:
	LocalBusInterface bus_;
	Poller poller_;
	int timerfd_;
	long long int timerselapsed_;
	uint8_t almac_[ETH_ALEN];
	struct ap_operational_bss ap_bss_info_;
	struct associated_client_list associated_client_info_;
	const std::string kDeviceFriendlyName = "same_name_multiap_router";
	const std::string kDeviceManufacturerName = "sample_name_intel";
	const std::string kDeviceManufacturerModel = "sample_name_multiap";

	int TimerSetup();
	bool get_interface_mac_addr(unsigned int if_index, uint8_t *addr);
	int HandleHigherLayerQuery(CmduRxMessage *rxmsg);
	int HandleTopologyDiscovery(CmduRxMessage *rxmsg);
	int HandleLldpDiscovery(CmduRxMessage *rxmsg);
	int HandleTimerEvent();
	int SendDiscoveryMessage(struct interface_list_item);
	int HandleTopologyQuery(CmduRxMessage *rxmsg);
	int HandleTopologyNotify(struct topology_notification_format *topology_notification);
	int HandleWlanApEvent(struct ap_operational_bss *rx_ap_bss_info);
	int HandleClientAssociationEvent(struct associated_client_list *rx_associated_client_info);
	int HandleLocalBusInterfaceIndication(mapf::InterfaceConfigurationIndicationMessage::Metadata *interface_metadata);
};

}

#endif // #ifndef _MAP_DISCOVERY_AGENT_H_
