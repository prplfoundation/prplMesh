/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

/**
 * This file uses code copied from `iw` (http://git.sipsolutions.net/iw.git/)
 *
 * Copyright (c) 2007, 2008 Johannes Berg
 * Copyright (c) 2007    Andy Lutomirski
 * Copyright (c) 2007    Mike Kershaw
 * Copyright (c) 2008-2009   Luis R. Rodriguez
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 */

#include "nl80211_client_impl.h"

#include <bcl/beerocks_utils.h>
#include <bwl/base_802_11_defs.h>

#include <easylogging++.h>

#include <linux/if_ether.h>
#include <linux/nl80211.h>
#include <net/if.h>
#include <netlink/genl/genl.h>

#include <math.h>
namespace bwl {

nl80211_client_impl::nl80211_client_impl(std::unique_ptr<nl80211_socket> socket)
    : m_socket(std::move(socket))
{
}

bool nl80211_client_impl::get_interface_info(const std::string &interface_name,
                                             interface_info &interface_info)
{
    interface_info = {};

    if (!m_socket) {
        LOG(ERROR) << "Socket is NULL!";
        return false;
    }

    // Get the interface index for given interface name
    int iface_index = if_nametoindex(interface_name.c_str());
    if (0 == iface_index) {
        LOG(ERROR) << "Failed to read the index of interface " << interface_name << ": "
                   << strerror(errno);

        return false;
    }

    return m_socket.get()->send_receive_msg(
        NL80211_CMD_GET_INTERFACE, 0,
        [&](struct nl_msg *msg) -> bool {
            nla_put_u32(msg, NL80211_ATTR_IFINDEX, iface_index);

            return true;
        },
        [&](struct nl_msg *msg) {
            struct nlattr *tb[NL80211_ATTR_MAX + 1];
            struct genlmsghdr *gnlh = (struct genlmsghdr *)nlmsg_data(nlmsg_hdr(msg));

            // Parse the netlink message
            if (nla_parse(tb, NL80211_ATTR_MAX, genlmsg_attrdata(gnlh, 0), genlmsg_attrlen(gnlh, 0),
                          NULL)) {
                LOG(ERROR) << "Failed to parse netlink message!";
                return;
            }

            if (tb[NL80211_ATTR_IFNAME]) {
                size_t length = nla_len(tb[NL80211_ATTR_IFNAME]);
                const uint8_t *data =
                    static_cast<const uint8_t *>(nla_data(tb[NL80211_ATTR_IFNAME]));

                for (size_t i = 0; i < length; i++) {
                    interface_info.name += static_cast<char>(data[i]);
                }
            }

            if (tb[NL80211_ATTR_IFINDEX]) {
                interface_info.index = nla_get_u32(tb[NL80211_ATTR_IFINDEX]);
            } else {
                LOG(DEBUG) << "NL80211_ATTR_IFINDEX attribute is missing";
            }

            if (tb[NL80211_ATTR_MAC]) {
                const uint8_t *data = static_cast<const uint8_t *>(nla_data(tb[NL80211_ATTR_MAC]));

                std::copy_n(data, sizeof(interface_info.addr.oct), interface_info.addr.oct);
            }

            if (tb[NL80211_ATTR_SSID]) {
                size_t length       = nla_len(tb[NL80211_ATTR_SSID]);
                const uint8_t *data = static_cast<const uint8_t *>(nla_data(tb[NL80211_ATTR_SSID]));

                for (size_t i = 0; i < length; i++) {
                    interface_info.ssid += static_cast<char>(data[i]);
                }
            }

            if (tb[NL80211_ATTR_IFTYPE]) {
                interface_info.type = nla_get_u32(tb[NL80211_ATTR_IFTYPE]);
            } else {
                LOG(DEBUG) << "NL80211_ATTR_IFTYPE attribute is missing";
            }

            if (tb[NL80211_ATTR_WIPHY]) {
                interface_info.wiphy = nla_get_u32(tb[NL80211_ATTR_WIPHY]);
            } else {
                LOG(DEBUG) << "NL80211_ATTR_WIPHY attribute is missing";
            }

            if (tb[NL80211_ATTR_CHANNEL_WIDTH]) {
                LOG(DEBUG) << "NL80211_ATTR_CHANNEL_WIDTH"
                           << nla_get_u32(tb[NL80211_ATTR_CHANNEL_WIDTH]);
            }

            if (tb[NL80211_ATTR_CENTER_FREQ1]) {
                LOG(DEBUG) << "NL80211_ATTR_CENTER_FREQ1"
                           << nla_get_u32(tb[NL80211_ATTR_CENTER_FREQ1]);
            }
            if (tb[NL80211_ATTR_CENTER_FREQ2]) {
                LOG(DEBUG) << "NL80211_ATTR_CENTER_FREQ2"
                           << nla_get_u32(tb[NL80211_ATTR_CENTER_FREQ2]);
            }
        });
}

bool nl80211_client_impl::get_radio_info(const std::string &interface_name, radio_info &radio_info)
{
    int last_band    = -1;
    bool width_40    = false;
    bool width_80    = false;
    bool width_80_80 = false;
    bool width_160   = false;

    radio_info = {};

    if (!m_socket) {
        LOG(ERROR) << "Socket is NULL!";
        return false;
    }

    // Get the interface index for given interface name
    int iface_index = if_nametoindex(interface_name.c_str());
    if (0 == iface_index) {
        LOG(ERROR) << "Failed to read the index of interface " << interface_name << ": "
                   << strerror(errno);

        return false;
    }

    return m_socket.get()->send_receive_msg(
        NL80211_CMD_GET_WIPHY, NLM_F_DUMP,
        [&](struct nl_msg *msg) -> bool {
            nla_put_u32(msg, NL80211_ATTR_IFINDEX, iface_index);
            nla_put_flag(msg, NL80211_ATTR_SPLIT_WIPHY_DUMP);

            return true;
        },
        [&](struct nl_msg *msg) {
            struct nlattr *tb[NL80211_ATTR_MAX + 1];
            struct genlmsghdr *gnlh = static_cast<genlmsghdr *>(nlmsg_data(nlmsg_hdr(msg)));

            // Parse the netlink message
            if (nla_parse(tb, NL80211_ATTR_MAX, genlmsg_attrdata(gnlh, 0), genlmsg_attrlen(gnlh, 0),
                          NULL)) {
                LOG(ERROR) << "Failed to parse netlink message!";
                return;
            }

            if (!tb[NL80211_ATTR_WIPHY_BANDS]) {
                return;
            }

            struct nlattr *nl_band;
            int rem_band;

            nla_for_each_nested(nl_band, tb[NL80211_ATTR_WIPHY_BANDS], rem_band)
            {
                struct nlattr *tb_band[NL80211_BAND_ATTR_MAX + 1];

                if (last_band != nl_band->nla_type) {
                    width_40    = false;
                    width_80    = false;
                    width_80_80 = false;
                    width_160   = false;
                    last_band   = nl_band->nla_type;

                    band_info band{};
                    radio_info.bands.push_back(band);
                }

                if (radio_info.bands.empty()) {
                    LOG(ERROR) << "Array of bands is empty";
                    return;
                }

                band_info &band = radio_info.bands.at(radio_info.bands.size() - 1);

                if (nla_parse(tb_band, NL80211_BAND_ATTR_MAX,
                              static_cast<nlattr *>(nla_data(nl_band)), nla_len(nl_band), NULL)) {
                    LOG(ERROR) << "Failed to parse wiphy band " << nl_band->nla_type + 1 << "!";
                    return;
                }

                if (tb_band[NL80211_BAND_ATTR_HT_CAPA]) {
                    band.ht_capability = nla_get_u16(tb_band[NL80211_BAND_ATTR_HT_CAPA]);

                    if (band.ht_capability & BIT(1)) {
                        width_40 = true;
                    }

                    if (tb_band[NL80211_BAND_ATTR_HT_MCS_SET]) {
                        size_t expected_length = sizeof(band.ht_mcs_set);
                        size_t actual_length   = nla_len(tb_band[NL80211_BAND_ATTR_HT_MCS_SET]);

                        if (actual_length <= expected_length) {
                            memcpy(band.ht_mcs_set, nla_data(tb_band[NL80211_BAND_ATTR_HT_MCS_SET]),
                                   actual_length);
                        } else {
                            LOG(DEBUG) << "Invalid length for NL80211_BAND_ATTR_HT_MCS_SET "
                                          "attribute. Expected length: "
                                       << expected_length << ", actual length: " << actual_length;
                        }
                    }
                }

                if (tb_band[NL80211_BAND_ATTR_VHT_CAPA]) {
                    band.vht_capability = nla_get_u32(tb_band[NL80211_BAND_ATTR_VHT_CAPA]);

                    width_80 = true;

                    switch ((band.vht_capability >> 2) & 3) {
                    case 2:
                        width_80_80 = true;
                        width_160   = true;
                        break;
                    case 1:
                        width_160 = true;
                        break;
                    }
                }

                if (tb_band[NL80211_BAND_ATTR_VHT_MCS_SET]) {
                    size_t expected_length = sizeof(band.vht_mcs_set);
                    size_t actual_length   = nla_len(tb_band[NL80211_BAND_ATTR_VHT_MCS_SET]);

                    if (actual_length <= expected_length) {
                        memcpy(band.vht_mcs_set, nla_data(tb_band[NL80211_BAND_ATTR_VHT_MCS_SET]),
                               actual_length);
                    } else {
                        LOG(DEBUG) << "Invalid length for NL80211_BAND_ATTR_VHT_MCS_SET "
                                      "attribute. Expected length: "
                                   << expected_length << ", actual length: " << actual_length;
                    }
                }

                if (!tb_band[NL80211_BAND_ATTR_FREQS]) {
                    return;
                }

                struct nlattr *nl_freq;
                int rem_freq;
                static struct nla_policy freq_policy[NL80211_FREQUENCY_ATTR_MAX + 1];
                freq_policy[NL80211_FREQUENCY_ATTR_FREQ]         = {NLA_U32, 0, 0};
                freq_policy[NL80211_FREQUENCY_ATTR_DISABLED]     = {NLA_FLAG, 0, 0};
                freq_policy[NL80211_FREQUENCY_ATTR_RADAR]        = {NLA_FLAG, 0, 0};
                freq_policy[NL80211_FREQUENCY_ATTR_MAX_TX_POWER] = {NLA_U32, 0, 0};

                nla_for_each_nested(nl_freq, tb_band[NL80211_BAND_ATTR_FREQS], rem_freq)
                {
                    struct nlattr *tb_freq[NL80211_FREQUENCY_ATTR_MAX + 1];
                    channel_info channel{};

                    if (nla_parse(tb_freq, NL80211_FREQUENCY_ATTR_MAX,
                                  static_cast<nlattr *>(nla_data(nl_freq)), nla_len(nl_freq),
                                  freq_policy)) {
                        LOG(ERROR) << "Failed to parse frequency " << nl_freq->nla_type + 1 << "!";
                        return;
                    }

                    if (!tb_freq[NL80211_FREQUENCY_ATTR_FREQ] ||
                        tb_freq[NL80211_FREQUENCY_ATTR_DISABLED] ||
                        tb_freq[__NL80211_FREQUENCY_ATTR_NO_IBSS]) {
                        continue;
                    }

                    uint32_t freq  = nla_get_u32(tb_freq[NL80211_FREQUENCY_ATTR_FREQ]);
                    channel.number = son::wireless_utils::freq_to_channel(freq);

                    if (tb_freq[NL80211_FREQUENCY_ATTR_MAX_TX_POWER]) {
                        channel.tx_power =
                            nla_get_u32(tb_freq[NL80211_FREQUENCY_ATTR_MAX_TX_POWER]) / 100;
                    }

                    if (tb_freq[NL80211_FREQUENCY_ATTR_RADAR]) {
                        channel.is_dfs = true;
                    }

                    if (tb_freq[NL80211_FREQUENCY_ATTR_NO_20MHZ]) {
                        /**
                         * NO_20MHZ will only be set for channels that are limited to 5 or 10MHz,
                         * which is probably not something we will ever want to use.
                         * So we can safely skip a channel that has NO_20MHZ set.
                         */
                        continue;
                    } else {
                        channel.supported_bandwidths.push_back(
                            beerocks::eWiFiBandwidth::BANDWIDTH_20);
                    }
                    if (width_40 && (!tb_freq[NL80211_FREQUENCY_ATTR_NO_HT40_MINUS] ||
                                     !tb_freq[NL80211_FREQUENCY_ATTR_NO_HT40_PLUS])) {
                        channel.supported_bandwidths.push_back(
                            beerocks::eWiFiBandwidth::BANDWIDTH_40);
                    }
                    if (width_80 && !tb_freq[NL80211_FREQUENCY_ATTR_NO_80MHZ]) {
                        channel.supported_bandwidths.push_back(
                            beerocks::eWiFiBandwidth::BANDWIDTH_80);
                    }
                    if (width_80_80 && !tb_freq[NL80211_FREQUENCY_ATTR_NO_80MHZ]) {
                        // NL80211_FREQUENCY_ATTR_NO_80MHZ includes 80+80 channels
                        channel.supported_bandwidths.push_back(
                            beerocks::eWiFiBandwidth::BANDWIDTH_80_80);
                    }
                    if (width_160 && !tb_freq[NL80211_FREQUENCY_ATTR_NO_160MHZ]) {
                        // NL80211_FREQUENCY_ATTR_NO_160MHZ does not include 80+80 channels
                        channel.supported_bandwidths.push_back(
                            beerocks::eWiFiBandwidth::BANDWIDTH_160);
                    }

                    if (tb_freq[NL80211_FREQUENCY_ATTR_DFS_STATE] &&
                        nla_get_u32(tb_freq[NL80211_FREQUENCY_ATTR_DFS_STATE]) ==
                            NL80211_DFS_UNAVAILABLE) {
                        channel.radar_affected = true;
                    }

                    band.supported_channels[channel.number] = channel;
                }
            }
        });
}

bool nl80211_client_impl::get_sta_info(const std::string &interface_name,
                                       const sMacAddr &sta_mac_address, sta_info &sta_info)
{
    static struct nla_policy stats_policy[NL80211_STA_INFO_MAX + 1];
    stats_policy[NL80211_STA_INFO_INACTIVE_TIME] = {NLA_U32, 0, 0};
    stats_policy[NL80211_STA_INFO_RX_BYTES]      = {NLA_U32, 0, 0};
    stats_policy[NL80211_STA_INFO_RX_PACKETS]    = {NLA_U32, 0, 0};
    stats_policy[NL80211_STA_INFO_TX_BYTES]      = {NLA_U32, 0, 0};
    stats_policy[NL80211_STA_INFO_TX_PACKETS]    = {NLA_U32, 0, 0};
    stats_policy[NL80211_STA_INFO_TX_RETRIES]    = {NLA_U32, 0, 0};
    stats_policy[NL80211_STA_INFO_TX_FAILED]     = {NLA_U32, 0, 0};
    stats_policy[NL80211_STA_INFO_SIGNAL]        = {NLA_U8, 0, 0};
    stats_policy[NL80211_STA_INFO_SIGNAL_AVG]    = {NLA_U8, 0, 0};
    stats_policy[NL80211_STA_INFO_TX_BITRATE]    = {NLA_NESTED, 0, 0};
    stats_policy[NL80211_STA_INFO_RX_BITRATE]    = {NLA_NESTED, 0, 0};

    static struct nla_policy rate_policy[NL80211_RATE_INFO_MAX + 1];
    rate_policy[NL80211_RATE_INFO_BITRATE]   = {NLA_U16, 0, 0};
    rate_policy[NL80211_RATE_INFO_BITRATE32] = {NLA_U32, 0, 0};

    sta_info = {};

    if (!m_socket) {
        LOG(ERROR) << "Socket is NULL!";
        return false;
    }

    // Get the interface index for given interface name
    int iface_index = if_nametoindex(interface_name.c_str());
    if (0 == iface_index) {
        LOG(ERROR) << "Failed to read the index of interface " << interface_name << ": "
                   << strerror(errno);

        return false;
    }

    return m_socket.get()->send_receive_msg(
        NL80211_CMD_GET_STATION, NLM_F_DUMP,
        [&](struct nl_msg *msg) -> bool {
            nla_put_u32(msg, NL80211_ATTR_IFINDEX, iface_index);
            nla_put(msg, NL80211_ATTR_MAC, ETH_ALEN, sta_mac_address.oct);

            return true;
        },
        [&](struct nl_msg *msg) {
            struct nlattr *tb[NL80211_ATTR_MAX + 1];
            struct genlmsghdr *gnlh = static_cast<genlmsghdr *>(nlmsg_data(nlmsg_hdr(msg)));
            struct nlattr *sinfo[NL80211_STA_INFO_MAX + 1];

            // Parse the netlink message
            if (nla_parse(tb, NL80211_ATTR_MAX, genlmsg_attrdata(gnlh, 0), genlmsg_attrlen(gnlh, 0),
                          NULL)) {
                LOG(ERROR) << "Failed to parse netlink message!";
                return;
            }

            if (!tb[NL80211_ATTR_STA_INFO]) {
                LOG(ERROR) << "STA stats missing!";
                return;
            }

            // Parse nested station stats
            if (nla_parse_nested(sinfo, NL80211_STA_INFO_MAX, tb[NL80211_ATTR_STA_INFO],
                                 stats_policy)) {
                LOG(ERROR) << "Failed to parse nested STA attributes!";
                return;
            }

            if (sinfo[NL80211_STA_INFO_INACTIVE_TIME]) {
                sta_info.inactive_time_ms = nla_get_u32(sinfo[NL80211_STA_INFO_INACTIVE_TIME]);
            } else {
                LOG(DEBUG) << "NL80211_STA_INFO_INACTIVE_TIME attribute is missing";
            }

            if (sinfo[NL80211_STA_INFO_RX_BYTES]) {
                sta_info.rx_bytes = nla_get_u32(sinfo[NL80211_STA_INFO_RX_BYTES]);
            } else {
                LOG(DEBUG) << "NL80211_STA_INFO_RX_BYTES attribute is missing";
            }

            if (sinfo[NL80211_STA_INFO_RX_PACKETS]) {
                sta_info.rx_packets = nla_get_u32(sinfo[NL80211_STA_INFO_RX_PACKETS]);
            } else {
                LOG(DEBUG) << "NL80211_STA_INFO_RX_PACKETS attribute is missing";
            }

            if (sinfo[NL80211_STA_INFO_TX_BYTES]) {
                sta_info.tx_bytes = nla_get_u32(sinfo[NL80211_STA_INFO_TX_BYTES]);
            } else {
                LOG(DEBUG) << "NL80211_STA_INFO_TX_BYTES attribute is missing";
            }

            if (sinfo[NL80211_STA_INFO_TX_PACKETS]) {
                sta_info.tx_packets = nla_get_u32(sinfo[NL80211_STA_INFO_TX_PACKETS]);
            } else {
                LOG(DEBUG) << "NL80211_STA_INFO_TX_PACKETS attribute is missing";
            }

            if (sinfo[NL80211_STA_INFO_TX_RETRIES]) {
                sta_info.tx_retries = nla_get_u32(sinfo[NL80211_STA_INFO_TX_RETRIES]);
            } else {
                LOG(DEBUG) << "NL80211_STA_INFO_TX_RETRIES attribute is missing";
            }

            if (sinfo[NL80211_STA_INFO_TX_FAILED]) {
                sta_info.tx_failed = nla_get_u32(sinfo[NL80211_STA_INFO_TX_FAILED]);
            } else {
                LOG(DEBUG) << "NL80211_STA_INFO_TX_FAILED attribute is missing";
            }

            if (sinfo[NL80211_STA_INFO_SIGNAL]) {
                sta_info.signal_dbm = nla_get_u8(sinfo[NL80211_STA_INFO_SIGNAL]);
            } else {
                LOG(DEBUG) << "NL80211_STA_INFO_SIGNAL attribute is missing";
            }

            if (sinfo[NL80211_STA_INFO_SIGNAL_AVG]) {
                sta_info.signal_avg_dbm = nla_get_u8(sinfo[NL80211_STA_INFO_SIGNAL_AVG]);
            } else {
                LOG(DEBUG) << "NL80211_STA_INFO_SIGNAL_AVG attribute is missing";
            }

            // Bit rate parsing helper function
            auto parse_bitrate_func = [&](struct nlattr *bitrate_attr) -> int {
                struct nlattr *rinfo[NL80211_RATE_INFO_MAX + 1];

                int rate = 0;
                if (nla_parse_nested(rinfo, NL80211_RATE_INFO_MAX, bitrate_attr, rate_policy)) {
                    LOG(ERROR) << "Failed to parse nested rate attributes!";
                } else if (rinfo[NL80211_RATE_INFO_BITRATE32]) {
                    rate = nla_get_u32(rinfo[NL80211_RATE_INFO_BITRATE32]);
                } else if (rinfo[NL80211_RATE_INFO_BITRATE]) {
                    rate = nla_get_u16(rinfo[NL80211_RATE_INFO_BITRATE]);
                } else {
                    LOG(DEBUG) << "NL80211_RATE_INFO_BITRATE attribute is missing";
                }

                return rate;
            };

            if (sinfo[NL80211_STA_INFO_TX_BITRATE]) {
                sta_info.tx_bitrate_100kbps =
                    parse_bitrate_func(sinfo[NL80211_STA_INFO_TX_BITRATE]);
            } else {
                LOG(DEBUG) << "NL80211_STA_INFO_TX_BITRATE attribute is missing";
            }

            if (sinfo[NL80211_STA_INFO_RX_BITRATE]) {
                sta_info.rx_bitrate_100kbps =
                    parse_bitrate_func(sinfo[NL80211_STA_INFO_RX_BITRATE]);
            } else {
                LOG(DEBUG) << "NL80211_STA_INFO_RX_BITRATE attribute is missing";
            }
        });
}

bool nl80211_client_impl::set_tx_power_limit(const std::string &interface_name, uint32_t limit)
{
    if (!m_socket) {
        LOG(ERROR) << "Socket is NULL!";
        return false;
    }

    // Get the interface index for given interface name
    int iface_index = if_nametoindex(interface_name.c_str());
    if (0 == iface_index) {
        LOG(ERROR) << "Failed to read the index of interface '" << interface_name
                   << "': " << strerror(errno);

        return false;
    }

    return m_socket.get()->send_receive_msg(
        NL80211_CMD_SET_WIPHY, 0,
        [&](struct nl_msg *msg) -> bool {
            nla_put_u32(msg, NL80211_ATTR_IFINDEX, iface_index);
            nla_put_u32(msg, NL80211_ATTR_WIPHY_TX_POWER_SETTING, NL80211_TX_POWER_LIMITED);
            nla_put_u32(msg, NL80211_ATTR_WIPHY_TX_POWER_LEVEL, limit * 100); //mBm
            return true;
        },
        [&](struct nl_msg *msg) {});
}

} // namespace bwl
