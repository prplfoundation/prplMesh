#ifndef _MAP_DISCOVERY_AGENT_LOCALDB_H_
#define _MAP_DISCOVERY_AGENT_LOCALDB_H_

namespace mapf
{

class TopologyDiscoveryAgentLocalDB {
public:
	enum media_type_set {
		MTYPE_FAST_ETH=0, /*!< Fast Ethernet */
		MTYPE_GIG_ETH=1, /*!< Gig Ethernet */
		MTYPE_80211_A_2_4G=256, /*!< 2.4G 11a */
		MTYPE_80211_B_2_4G=257, /*!< 2.4G 11b */
		MTYPE_80211_A_5G=258, /*!< 5G 11a */
		MTYPE_80211_N_2_4G=259, /*!< 2.5G 11n */
		MTYPE_80211_N_5G=260, /*!< 5G 11n */
		MTYPE_80211_AC_5G=261, /*!< 5G 11ac */
		MTYPE_80211_AD=262, /*!< 11ad - not supported ? */
		MTYPE_80211_AF=263, /*!< 11af - not supported ? */
		MTYPE_1901_WAVELET=512, /*!< 1901 wavelet - not supported */
		MTYPE_1901_FFT=513, /*!< 1901 FFT - not supported */
		MTYPE_MoCA_V1_1=1024 /*!< MoCA v1 1 - not supported */
	};

	enum service_type_set {
		MAP_CONTROLLER, /*!< Multi-AP Controller */
		MAP_AGENT /*!< Multi-AP Agent */
	};

	enum neighbor_type {
		NEIGHBOR_1905,
		NEIGHBOR_NON_1905
	};

	enum Flags {
		ENABLE_IEEE1905_TRANSPORT = 0x00000001,
		IS_BRIDGE = 0x00000002,
	};

	struct neighbor {
		uint8_t mac[ETH_ALEN];
		int	type;
		bool	bridge_exists;
		int	associated_time;
		enum service_type_set service_type;
		neighbor()
		{
			type = associated_time = 0;
			memset(mac, 0, sizeof(mac));
			bridge_exists = false;
			service_type = MAP_AGENT;
		}
		neighbor(uint8_t *mac) : neighbor()
		{
			if(mac != NULL) {
				std::copy_n(mac, ETH_ALEN, this->mac);
			}
		}
		bool operator==(const neighbor & neighbor2) const
		{
			return (memcmp(this->mac, neighbor2.mac, ETH_ALEN) == 0);
		}
	};

	struct interface {
		int	index; /*!< Index of the interface */
		uint8_t mac[ETH_ALEN]; /*!< MAC address of interface in PPQQRRSSTTUU format */
		enum media_type_set media_type;
		bool	ieee802_1bridge;
		bool	transport; /*!< TODO Bit value indicating if interface is transport interface connecting to other devices in network */
		int	status;
		int	parent_bridge_index;
		interface()
		{
			index = status = parent_bridge_index = 0;
			ieee802_1bridge = transport = false;
			memset(mac, 0, sizeof(mac));
			media_type = MTYPE_FAST_ETH;
		}
	};

	struct interface_list_item {
		struct interface instance;
    		std::list<struct neighbor> neighbor_list;
		interface_list_item() {}
		interface_list_item(int index, uint8_t *mac)
		{
			instance.index = index;
			if(mac != NULL) {
				std::copy_n(mac, ETH_ALEN, instance.mac);
			}
		}
		bool operator==(const interface_list_item & interface2) const
		{
			int return_value = 0;
			uint8_t testmac[ETH_ALEN] = {0};
			if(memcmp(interface2.instance.mac, testmac, ETH_ALEN) != 0 && memcmp(this->instance.mac, testmac, ETH_ALEN) != 0) {
				return_value = (memcmp(this->instance.mac, interface2.instance.mac, ETH_ALEN) == 0);
				return_value &= (int)(this->instance.index == interface2.instance.index);
			}
			return return_value;
		}
	};

protected:
	std::list<struct interface_list_item> interface_list;

	int HandleInterfaceEvent(struct interface interface_instance);
	void ShowInterfaceList();
	int UpdateNeighborEntry(int index, uint8_t *mac, uint8_t *neigh_mac, int type, int oper);
	void ShowNeighborList(std::list<struct interface_list_item>::iterator it);
};

};

#endif // ifndef _MAP_DISCOVERY_AGENT_LOCALDB_H_
