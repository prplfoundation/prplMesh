/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2020 MaxLinear
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include "nl80211_client_impl.h"

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

bool nl80211_client_impl::get_sta_info(const std::string &local_interface_name,
                                       const sMacAddr &sta_mac_address, sStaInfo &sta_info)
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
    int iface_index = if_nametoindex(local_interface_name.c_str());
    if (0 == iface_index) {
        LOG(ERROR) << "Failed to read the index of interface " << local_interface_name << ": "
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
        [&](struct nl_msg *msg) -> bool {
            struct nlattr *tb[NL80211_ATTR_MAX + 1];
            struct genlmsghdr *gnlh = (struct genlmsghdr *)nlmsg_data(nlmsg_hdr(msg));
            struct nlattr *sinfo[NL80211_STA_INFO_MAX + 1];

            // Parse the netlink message
            if (nla_parse(tb, NL80211_ATTR_MAX, genlmsg_attrdata(gnlh, 0), genlmsg_attrlen(gnlh, 0),
                          NULL)) {
                LOG(ERROR) << "Failed to parse netlink message!";
                return false;
            }

            if (!tb[NL80211_ATTR_STA_INFO]) {
                LOG(ERROR) << "STA stats missing!";
                return false;
            }

            // Parse nested station stats
            if (nla_parse_nested(sinfo, NL80211_STA_INFO_MAX, tb[NL80211_ATTR_STA_INFO],
                                 stats_policy)) {
                LOG(ERROR) << "Failed to parse nested STA attributes!";
                return false;
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
                sta_info.signal_dBm = nla_get_u8(sinfo[NL80211_STA_INFO_SIGNAL]);
            } else {
                LOG(DEBUG) << "NL80211_STA_INFO_SIGNAL attribute is missing";
            }

            if (sinfo[NL80211_STA_INFO_SIGNAL_AVG]) {
                sta_info.signal_avg_dBm = nla_get_u8(sinfo[NL80211_STA_INFO_SIGNAL_AVG]);
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

            return true;
        });
}

bool nl80211_client_impl::set_tx_power_limit(const std::string &local_interface_name,
                                             uint32_t limit)
{
    if (!m_socket) {
        LOG(ERROR) << "Socket is NULL!";
        return false;
    }

    // Get the interface index for given interface name
    int iface_index = if_nametoindex(local_interface_name.c_str());
    if (0 == iface_index) {
        LOG(ERROR) << "Failed to read the index of interface " << local_interface_name << ": "
                   << strerror(errno);

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
        [&](struct nl_msg *msg) -> bool { return true; });
}

} // namespace bwl
