/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include "base_wlan_hal_brcm.h"

#include <beerocks_defines.h>
#include <network/network_utils.h>

#include <easylogging++.h>

#include <netinet/ether.h>

// BRCM utils
#define _NET_ETHERNET_H_ // Fix symbol redifinition errors
extern "C" {
#include "utils/bwl_utils.h"
#include <wlioctl.h>
}

namespace bwl {
namespace brcm {

//////////////////////////////////////////////////////////////////////////////
/////////////////////////// Local Module Functions ///////////////////////////
//////////////////////////////////////////////////////////////////////////////

static int wl_format_ssid(char *ssid_buf, uint8 *ssid, int ssid_len)
{
    int i, c;
    char *p = ssid_buf;

    if (ssid_len > 32)
        ssid_len = 32;

    for (i = 0; i < ssid_len; i++) {
        c = (int)ssid[i];
        if (c == '\\') {
            *p++ = '\\';
            *p++ = '\\';
        } else if (isprint((uchar)c)) {
            *p++ = (char)c;
        } else {
            p += sprintf(p, "\\x%02X", c);
        }
    }
    *p = '\0';

    return p - ssid_buf;
}

// 80MHz channels in 5GHz band
static const uint8 wf_5g_80m_chans[] = {42, 58, 106, 122, 138, 155};

static const uint8 wf_chspec_bw_mhz[] = {5, 10, 20, 40, 80, 160, 160};

#define WF_NUM_BW (sizeof(wf_chspec_bw_mhz) / sizeof(uint8))

// Convert bandwidth from chanspec to MHz
static int bw_chspec_to_mhz(chanspec_t chspec)
{
    uint bw;
    bw = (chspec & WL_CHANSPEC_BW_MASK) >> WL_CHANSPEC_BW_SHIFT;
    return (bw >= WF_NUM_BW ? 0 : wf_chspec_bw_mhz[bw]);
}

// BW in MHz, return the channel count from the center channel to the the channel at the edge of the band
static int center_chan_to_edge(uint bw)
{
    /* edge channels separated by BW - 10MHz on each side
	 * delta from cf to edge is half of that,
	 * MHz to channel num conversion is 5MHz/channel
	 */
    return (int)(((bw - 20) / 2) / 5);
}

// Return channel number of the low edge of the band given the center channel and BW
static int channel_low_edge(uint center_ch, uint bw)
{
    return (int)(center_ch - center_chan_to_edge(bw));
}

// Return control channel given center channel and side band
static int channel_to_ctl_chan(uint center_ch, uint bw, uint sb)
{
    return (channel_low_edge(center_ch, bw) + sb * 4);
}

static int chspec_to_ctlchan(chanspec_t chspec)
{
    uint center_chan;
    uint bw_mhz;
    uint sb;

    // Is there a sideband ?
    if (CHSPEC_IS20(chspec)) {
        return CHSPEC_CHANNEL(chspec);
    }

    sb = CHSPEC_CTL_SB(chspec) >> WL_CHANSPEC_CTL_SB_SHIFT;

    if (CHSPEC_IS8080(chspec)) {
        /* For an 80+80 MHz channel, the sideband 'sb' field is an 80 MHz sideband
            * (LL, LU, UL, LU) for the 80 MHz frequency segment 0.
            */
        uint chan_id = CHSPEC_CHAN1(chspec);
        bw_mhz       = 80;
        /* convert from channel index to channel number */
        center_chan = wf_5g_80m_chans[chan_id];
    } else {
        bw_mhz      = bw_chspec_to_mhz(chspec);
        center_chan = CHSPEC_CHANNEL(chspec) >> WL_CHANSPEC_CHAN_SHIFT;
    }

    return (channel_to_ctl_chan(center_chan, bw_mhz, sb));
}

//////////////////////////////////////////////////////////////////////////////
/////////////////////////////// Implementation ///////////////////////////////
//////////////////////////////////////////////////////////////////////////////

base_wlan_hal_brcm::base_wlan_hal_brcm(HALType type, std::string iface_name, bool acs_enabled,
                                       IfaceType iface_type, hal_event_cb_t callback)
    : base_wlan_hal(type, iface_name, iface_type, false, callback)
{
}

base_wlan_hal_brcm::~base_wlan_hal_brcm() {}

bool base_wlan_hal_brcm::refresh_radio_info()
{
    /*** Radio Info ***/

    // Channel
    // channel_info_t ci;
    // int ret = bwl_wl_ioctl((char*)m_radio_info.iface_name.c_str(), WLC_GET_CHANNEL, &ci, sizeof(ci));
    // if (ret < 0) {
    //     LOG(ERROR) << "Failed reading radio channel!";
    //     return false;
    // }

    // m_radio_info.channel = ci.hw_channel;
    // if (m_radio_info.channel > 14)
    //     m_radio_info.is_5ghz = 1;

    strcpy(wl_ioctl_buf, "chanspec");
    int ret = bwl_wl_ioctl((char *)m_radio_info.iface_name.c_str(), WLC_GET_VAR, wl_ioctl_buf,
                           sizeof(wl_ioctl_buf));
    if (ret < 0) {
        LOG(ERROR) << "WLC_GET_VAR(sta_info) failed: " << ret;
        return false;
    }

    chanspec_t chspec = (chanspec_t)((int(wl_ioctl_buf[1]) << 8) | (int(wl_ioctl_buf[0])));
    // LOG(DEBUG) << "Chanspec: " << std::hex << int(chspec) << std::dec;

    m_radio_info.channel   = chspec_to_ctlchan(chspec);
    m_radio_info.is_5ghz   = CHSPEC_IS5G(chspec);
    m_radio_info.bandwidth = bw_chspec_to_mhz(chspec);

    // TX Enabled
    m_radio_info.wifi_ctrl_enabled = 1;
    uint32_t tx_enabled;
    ret = bwl_wl_ioctl((char *)m_radio_info.iface_name.c_str(), WLC_GET_RADIO, &tx_enabled,
                       sizeof(tx_enabled));
    if (ret < 0) {
        LOG(ERROR) << "Failed reading radio status!";
        return false;
    }

    // 0 - Enabled, 1 - Disabled
    m_radio_info.tx_enabled = !tx_enabled;

    // TODO: Read from the HW
    m_radio_info.ant_num           = 2;
    m_radio_info.conducted_power   = 0;
    m_radio_info.channel_ext_above = 0;

    // LOG(DEBUG) << "Radio enabled: " << int(m_radio_info.tx_enabled)
    //            << ", channel: " << int(m_radio_info.channel)
    //            << ", bandwidth: " << bw_chspec_to_mhz(chspec);

    /*** Vap Info ***/

    char ssidbuf[64 + 1];
    wlc_ssid_t ssid = {0, {0}};
    //wl_getbssid((char*)radio_info.iface.c_str(), mac);

    // struct ether_addr ea;
    // bwl_wl_ioctl((char*)radio_info.iface.c_str(), WLC_GET_BSSID, &ea, ETHER_ADDR_LEN);
    // available_vaps[beerocks::IFACE_VAP_ID_MIN].mac.assign(network_utils::mac_to_string((const uint8_t*)&ea));

    network_utils::linux_iface_get_mac(m_radio_info.iface_name,
                                       m_radio_info.available_vaps[beerocks::IFACE_VAP_ID_MIN].mac);
    bwl_wl_ioctl((char *)m_radio_info.iface_name.c_str(), WLC_GET_SSID, &ssid, sizeof(ssid));

    //ssid.SSID_len = dtoh32(ssid.SSID_len);
    wl_format_ssid(ssidbuf, ssid.SSID, ssid.SSID_len);
    m_radio_info.available_vaps[beerocks::IFACE_VAP_ID_MIN].ssid.assign(ssidbuf);

    // Update radio info
    get_radio_info();

    // LOG(DEBUG) << "Interface " << radio_info.iface << " ssid: " << ssidbuf;
    // LOG(DEBUG) << "Interface " << radio_info.iface << " mac: " << network_utils::mac_to_string((const uint8_t*)&ea);

    return true;
}

void base_wlan_hal_brcm::refresh_vaps_info(int id) { return true; }

std::string base_wlan_hal_brcm::get_radio_mac()
{
    // Check if the main VAP exists
    if (m_radio_info.available_vaps.find(beerocks::IFACE_VAP_ID_MIN) !=
        m_radio_info.available_vaps.end()) {

        // Return the MAC address of the main VAP
        return (m_radio_info.available_vaps[beerocks::IFACE_VAP_ID_MIN].mac);
    } else {
        return std::string();
    }
}

} // namespace brcm
} // namespace bwl