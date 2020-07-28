#include "media_type.h"

#include <bcl/network/network_utils.h>

#include "../agent_db.h"

// SPEED values
#include <linux/ethtool.h>

namespace beerocks {
/**
 * IEEE Std 1905.1, Table 6-12—Media type (intfType) for IEEE 802.11 interfaces
 *
 * Value and Description         Frequency  MaxBandwith  Comments
 * 0 IEEE 802.11b (2.4 GHz)      2.4 GHz    22 MHz       Not supported
 * 1 IEEE 802.11g (2.4 GHz)      2.4 GHz    20 MHz
 * 2 IEEE 802.11a (5 GHz)        5 GHz      20 MHz
 * 3 IEEE 802.11n (2.4 GHz)      2.4 GHz    40 MHz
 * 4 IEEE 802.11n (5 GHz)        5 GHz      40 MHz
 * 5 IEEE 802.11ac (5 GHz)       5 GHz      80 MHz
 * 6 IEEE 802.11ad (60 GHz)      60 GHz     160 MHz      Not supported
 * 7 IEEE 802.11af (whitespace)                          Not supported
 * 8 IEEE 802.11ax (2.4 GHz)     2.4 GHz    160 MHz      Not included in Table 6-12—Media type (intfType), WiFi6 is specified to use 0x0108 in R2
 * 8 IEEE 802.11ax (5 GHz)       5 GHz      160 MHz      Not included in Table 6-12—Media type (intfType), WiFi6 is specified to use 0x0108 in R2
 */
static const std::vector<std::tuple<eFreqType, eWiFiBandwidth, ieee1905_1::eMediaType>>
    table_6_12_media_type_802_11{
        std::make_tuple(eFreqType::FREQ_24G, eWiFiBandwidth::BANDWIDTH_20,
                        ieee1905_1::eMediaType::IEEE_802_11G_2_4_GHZ),

        std::make_tuple(eFreqType::FREQ_5G, eWiFiBandwidth::BANDWIDTH_20,
                        ieee1905_1::eMediaType::IEEE_802_11A_5_GHZ),

        std::make_tuple(eFreqType::FREQ_24G, eWiFiBandwidth::BANDWIDTH_40,
                        ieee1905_1::eMediaType::IEEE_802_11N_2_4_GHZ),

        std::make_tuple(eFreqType::FREQ_5G, eWiFiBandwidth::BANDWIDTH_40,
                        ieee1905_1::eMediaType::IEEE_802_11N_5_GHZ),

        std::make_tuple(eFreqType::FREQ_5G, eWiFiBandwidth::BANDWIDTH_80,
                        ieee1905_1::eMediaType::IEEE_802_11AC_5_GHZ),

        std::make_tuple(eFreqType::FREQ_24G, eWiFiBandwidth::BANDWIDTH_160,
                        ieee1905_1::eMediaType::IEEE_802_11AX),

        std::make_tuple(eFreqType::FREQ_5G, eWiFiBandwidth::BANDWIDTH_160,
                        ieee1905_1::eMediaType::IEEE_802_11AX),

    };

ieee1905_1::eMediaType MediaType::get_802_11_media_type(eFreqType frequency_band,
                                                        eWiFiBandwidth max_bandwidth)
{
    for (const auto &tuple : table_6_12_media_type_802_11) {
        if ((std::get<0>(tuple) == frequency_band) && (std::get<1>(tuple) == max_bandwidth)) {
            return std::get<2>(tuple);
        }
    }

    return ieee1905_1::eMediaType::UNKNOWN_MEDIA;
}

bool MediaType::get_media_type(const std::string &interface_name,
                               ieee1905_1::eMediaTypeGroup media_type_group,
                               ieee1905_1::eMediaType &media_type)
{
    bool result = false;

    if (ieee1905_1::eMediaTypeGroup::IEEE_802_3 == media_type_group) {
        uint32_t speed;
        if (net::network_utils::linux_iface_get_speed(interface_name, speed)) {
            if (SPEED_100 == speed) {
                media_type = ieee1905_1::eMediaType::IEEE_802_3U_FAST_ETHERNET;
            } else if (SPEED_1000 <= speed) {
                media_type = ieee1905_1::eMediaType::IEEE_802_3AB_GIGABIT_ETHERNET;
            } else {
                media_type = ieee1905_1::eMediaType::UNKNOWN_MEDIA;
            }

            result = true;
        }
    } else if (ieee1905_1::eMediaTypeGroup::IEEE_802_11 == media_type_group) {

        auto db = AgentDB::get();

        auto radio = db->radio(interface_name);
        if (radio) {
            media_type =
                get_802_11_media_type(radio->front.freq_type, radio->front.max_supported_bw);
            result = true;
        }

    } else if (ieee1905_1::eMediaTypeGroup::IEEE_1901 == media_type_group) {
        // TODO: Not supported yet
        LOG(ERROR) << "IEEE_1901 media is not supported yet";
    } else if (ieee1905_1::eMediaTypeGroup::MoCA == media_type_group) {
        // TODO: Not supported yet
        LOG(ERROR) << "MoCA media is not supported yet";
    } else {
        media_type = ieee1905_1::eMediaType::UNKNOWN_MEDIA;
        result     = true;
    }

    return result;
}
} // namespace beerocks
