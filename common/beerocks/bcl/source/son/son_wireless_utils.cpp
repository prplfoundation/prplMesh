/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include <bcl/beerocks_utils.h>
#include <bcl/network/network_utils.h>
#include <bcl/son/son_wireless_utils.h>

#include <easylogging++.h>

#include <cmath>

using namespace son;

constexpr beerocks::eWiFiAntNum
    wireless_utils::phy_rate_table_mode_to_ant_num[PHY_RATE_TABLE_ANT_MODE_MAX];
constexpr beerocks::eWiFiSS wireless_utils::phy_rate_table_mode_to_ss[PHY_RATE_TABLE_ANT_MODE_MAX];
constexpr wireless_utils::sPhyRateTableEntry
    wireless_utils::phy_rate_table[PHY_RATE_TABLE_ANT_MODE_MAX][PHY_RATE_TABLE_MCS_MAX];
constexpr wireless_utils::sPhyRateBitRateEntry
    wireless_utils::bit_rate_max_table_mbps[BIT_RATE_MAX_TABLE_SIZE];

wireless_utils::sPhyUlParams
wireless_utils::estimate_ul_params(int ul_rssi, uint16_t sta_phy_tx_rate_100kb,
                                   const beerocks::message::sRadioCapabilities *sta_capabilities,
                                   beerocks::eWiFiBandwidth ap_bw, bool is_5ghz)
{
    int ul_rssi_lut = ul_rssi * 10;
    int estimated_ul_rssi_lut;
    int estimated_ul_rssi_lut_delta;
    int estimated_ul_rssi_lut_delta_min = 120 * 10;
    sPhyUlParams estimation             = {0, beerocks::RSSI_INVALID};

    if (ul_rssi == beerocks::RSSI_INVALID) {
        LOG(DEBUG) << "estimate_sta_ul_params: RSSI_INVALID";
        return estimation;
    }

    int max_ant_mode = (sta_capabilities->ant_num == beerocks::ANT_1X1)
                           ? beerocks::ANT_MODE_1X1_SS1
                           : beerocks::ANT_MODE_2X2_SS2;
    int max_mcs = (is_5ghz && (sta_capabilities->wifi_standard & int(beerocks::STANDARD_AC)))
                      ? sta_capabilities->vht_mcs
                      : sta_capabilities->ht_mcs;
    int max_bw = (is_5ghz && (sta_capabilities->wifi_standard & int(beerocks::STANDARD_AC)))
                     ? sta_capabilities->vht_bw
                     : sta_capabilities->ht_bw;
    if (ap_bw < max_bw) {
        max_bw = ap_bw;
    }

    max_bw = (max_bw > beerocks::BANDWIDTH_160 ? beerocks::BANDWIDTH_160 : max_bw);

    for (int ant_mode = max_ant_mode; ant_mode > -1; ant_mode--) { // filter by ant_num
        for (int bw = max_bw; bw > -1; bw--) {                     // filter by mac_bw
            for (int mcs = max_mcs; mcs > -1; mcs--) {             // filter by mcs

                estimated_ul_rssi_lut = phy_rate_table[ant_mode][mcs].bw_values[bw].rssi;
                estimated_ul_rssi_lut_delta =
                    (int)std::abs((float)(ul_rssi_lut - estimated_ul_rssi_lut));

                // same rate && min rssi_delta
                if ((sta_phy_tx_rate_100kb ==
                     phy_rate_table[ant_mode][mcs].bw_values[bw].gi_long_rate) ||
                    (sta_phy_tx_rate_100kb ==
                     phy_rate_table[ant_mode][mcs].bw_values[bw].gi_short_rate)) {
                    if (estimated_ul_rssi_lut_delta <= estimated_ul_rssi_lut_delta_min) {
                        estimated_ul_rssi_lut_delta_min = estimated_ul_rssi_lut_delta;
                        estimation.tx_power = is_5ghz ? phy_rate_table[ant_mode][mcs].tx_power_5
                                                      : phy_rate_table[ant_mode][mcs].tx_power_2_4;
                        estimation.rssi =
                            int(float(estimated_ul_rssi_lut) / 10.0 + 0.5); //round value
                    }
                }
            }
        }
    }

    float rssi_delta = float(estimated_ul_rssi_lut_delta_min) / 10.0;
    LOG(DEBUG) << "estimate_sta_ul_params:" << std::endl
               << "  ul_rssi = " << ul_rssi
               << "  sta_phy_tx_rate [Mbps] = " << (double(sta_phy_tx_rate_100kb) / 10.0f)
               << "  sta_ant_num = " << int(sta_capabilities->ant_num)
               << " sta_max_mcs = " << max_mcs << "  is_5ghz = " << is_5ghz
               << "  max_bw = " << max_bw << std::endl
               << "  estimation: delta_rssi = " << rssi_delta
               << "  tx_power = " << estimation.tx_power << " ul_rssi = " << estimation.rssi
               << std::endl;

    return estimation;
}

int wireless_utils::estimate_dl_rssi(int ul_rssi, int tx_power, const sPhyApParams &ap_params)
{
    int eirp_sta = tx_power;
    int eirp_ap  = ap_params.ant_gain + ap_params.conducted_power;
    int ant_factor;

    int noise_figure = NOISE_FIGURE;
    float pathloss;
    int dl_rssi;

    //ant_factor
    if (ap_params.ant_num == beerocks::ANT_4X4) {
        ant_factor = ANT_FACTOR_4X4;
    } else if (ap_params.ant_num == beerocks::ANT_3X3) {
        ant_factor = ANT_FACTOR_3X3;
    } else { // ap_params.ant_num == ANT_1X1 | ANT_2X2
        ant_factor = ANT_FACTOR_2X2;
    }

    //pathloss
    pathloss = eirp_sta - ul_rssi + noise_figure - ant_factor - ap_params.ant_gain;
    dl_rssi  = eirp_ap - pathloss;
    return dl_rssi;
}

double wireless_utils::estimate_ap_tx_phy_rate(
    int estimated_dl_rssi, const beerocks::message::sRadioCapabilities *sta_capabilities,
    beerocks::eWiFiBandwidth ap_bw, bool is_5ghz)
{
    int estimated_dl_rssi_lut = estimated_dl_rssi * 10;
    int dl_rssi_lut;
    double estimated_phy_rate = 0;

    int max_ant_mode = (sta_capabilities->ant_num == beerocks::ANT_1X1)
                           ? beerocks::ANT_MODE_1X1_SS1
                           : beerocks::ANT_MODE_2X2_SS2;
    int max_mcs = (is_5ghz && (sta_capabilities->wifi_standard & int(beerocks::STANDARD_AC)))
                      ? sta_capabilities->vht_mcs
                      : sta_capabilities->ht_mcs;
    int max_bw = (is_5ghz && (sta_capabilities->wifi_standard & int(beerocks::STANDARD_AC)))
                     ? sta_capabilities->vht_bw
                     : sta_capabilities->ht_bw;
    if (ap_bw < max_bw) {
        max_bw = ap_bw;
    }

    // Beerocks is not supporting estimation above 80 Mhz
    max_bw = (max_bw > beerocks::BANDWIDTH_160 ? beerocks::BANDWIDTH_160 : max_bw);

    for (int ant_mode = max_ant_mode; ant_mode > -1; ant_mode--) { // filter by ant_num
        for (int bw = max_bw; bw > -1; bw--) {                     // filter by mac_bw
            for (int mcs = max_mcs; mcs > -1; mcs--) {             // filter by mcs

                dl_rssi_lut = phy_rate_table[ant_mode][mcs].bw_values[bw].rssi;
                // same rate && min rssi_delta
                if (estimated_dl_rssi_lut >= dl_rssi_lut) {
                    estimated_phy_rate =
                        1e+5 * double(phy_rate_table[ant_mode][mcs].bw_values[bw].gi_short_rate);
                    break;
                }
            }
            if (estimated_phy_rate != 0)
                break;
        }
        if (estimated_phy_rate != 0)
            break;
    }

    if (estimated_phy_rate == 0) {
        estimated_phy_rate = 1e+5 * double(phy_rate_table[0][0].bw_values[0].gi_short_rate);
    }
    return estimated_phy_rate;
}

double wireless_utils::get_load_max_bit_rate_mbps(double phy_rate_100kb)
{
    int size = BIT_RATE_MAX_TABLE_SIZE;
    int i;
    for (i = 0; i < size; i++) {
        if (phy_rate_100kb < bit_rate_max_table_mbps[i].phy_rate_100kb) {
            break;
        }
    }
    if ((i == 0) || (i == size)) {
        if (i == size)
            i--;
        return bit_rate_max_table_mbps[i].bit_rate_max_mbps;
    } else {
        int phy_rate_delta = bit_rate_max_table_mbps[i].phy_rate_100kb -
                             bit_rate_max_table_mbps[i - 1].phy_rate_100kb;
        int bit_rate_delta = bit_rate_max_table_mbps[i].bit_rate_max_mbps -
                             bit_rate_max_table_mbps[i - 1].bit_rate_max_mbps;
        int percentage = (phy_rate_delta == 0)
                             ? 0
                             : ((phy_rate_100kb - bit_rate_max_table_mbps[i - 1].phy_rate_100kb) /
                                phy_rate_delta);
        return (bit_rate_max_table_mbps[i - 1].bit_rate_max_mbps + bit_rate_delta * percentage);
    }
}

bool wireless_utils::get_mcs_from_rate(const uint16_t rate, const beerocks::eWiFiAntMode ant_mode,
                                       const beerocks::eWiFiBandwidth bw, uint8_t &mcs,
                                       uint8_t &short_gi)
{
    uint16_t rate_temp;
    uint16_t diff_temp;
    uint16_t nearest_diff = 0xFFFF;
    for (int mcs_idx = 0; mcs_idx < 8; mcs_idx++) {
        if (phy_rate_table[ant_mode][mcs_idx].bw_values[bw].gi_short_rate == rate) {
            mcs      = mcs_idx;
            short_gi = 1;
            return true;
        } else if (phy_rate_table[ant_mode][mcs_idx].bw_values[bw].gi_long_rate == rate) {
            mcs      = mcs_idx;
            short_gi = 1;
            return true;
        }

        rate_temp = phy_rate_table[ant_mode][mcs_idx].bw_values[bw].gi_short_rate;
        diff_temp = abs(rate - rate_temp);
        if (diff_temp < nearest_diff) {
            nearest_diff = diff_temp;
            mcs          = mcs_idx;
            short_gi     = 1;
        }

        rate_temp = phy_rate_table[ant_mode][mcs_idx].bw_values[bw].gi_long_rate;
        diff_temp = abs(rate - rate_temp);
        if (diff_temp < nearest_diff) {
            nearest_diff = diff_temp;
            mcs          = mcs_idx;
            short_gi     = 0;
        }
    }
    return false;
}

beerocks::eFreqType wireless_utils::which_freq(uint32_t chn)
{
    if ((1 <= chn) && (chn <= BAND_5G_CHANNEL_CHECK)) {
        return beerocks::eFreqType::FREQ_24G;
    }

    if (36 <= chn) {
        return beerocks::eFreqType::FREQ_5G;
    }

    LOG(ERROR) << " Something incorrect in which_freq chn = " << int(chn);
    return beerocks::eFreqType::FREQ_UNKNOWN;
}

bool wireless_utils::is_same_freq_band(int chn1, int chn2)
{
    if ((which_freq(chn1) == which_freq(chn2)) &&
        (which_freq(chn1) != beerocks::eFreqType::FREQ_UNKNOWN)) {
        return true;
    }

    return false;
}

bool wireless_utils::is_same_interface(const std::string &ifname1, const std::string &ifname2)
{
    return (ifname1 == ifname2);
}

beerocks::eSubbandType wireless_utils::which_subband(uint32_t chn)
{

    if ((36 <= chn) && (chn <= 64)) {
        return beerocks::eSubbandType::LOW_SUBBAND;
    }

    if ((100 <= chn) && (chn <= 165)) {
        return beerocks::eSubbandType::HIGH_SUBBAND;
    }

    LOG(WARNING) << " Something incorrect in which_subband chn = " << (int)chn;
    return beerocks::eSubbandType::SUBBAND_UNKNOWN;
}

bool wireless_utils::is_low_subband(const uint32_t chn)
{
    if (chn <= END_OF_LOW_BAND) {
        LOG(DEBUG) << "is_low_subband:: channel: " << (int)chn << " is LOW_SUBBAND ";
        return true;
    }

    LOG(INFO) << "is_low_subband: chn = " << (int)chn << " returns false ";
    return false;
}
bool wireless_utils::is_high_subband(const uint32_t chn)
{
    if ((chn >= START_OF_HIGH_BAND)) {
        LOG(DEBUG) << "is_high_subband:: channel: " << (int)chn << " is HIGH_SUBBAND ";
        return true;
    }

    LOG(INFO) << "is_high_subband:: chn = " << (int)chn << " returns false ";
    return false;
}
bool wireless_utils::is_dfs_channel(const uint32_t chn)
{
    if (((chn >= START_OF_LOW_DFS_SUBBAND) && chn <= (END_OF_LOW_DFS_SUBBAND)) ||
        ((chn >= START_OF_HIGH_DFS_SUBBAND) && (chn <= END_OF_HIGH_DFS_SUBBAND))) {
        return true;
    }
    return false;
}

std::vector<std::pair<uint8_t, beerocks::eWifiChannelType>>
wireless_utils::split_channel_to_20MHz(int channel, beerocks::eWiFiBandwidth bw,
                                       bool channel_ext_above_secondary,
                                       bool channel_ext_above_primary)
{
    //split ch and bw to 20MHz channels.
    //example ch = 149 , bw = 80Mhz , channel_ext_above_secondary - true , channel_ext_above_primary = true.
    //channel_ext_above_secondary = (ch < center_freq) ? true:false;
    //channel_ext_above_primary - true , 153 is also primary.
    //split output: 149, 153, 157, 161.
    LOG(INFO) << "split_channel_to_20MHz";
    std::vector<std::pair<uint8_t, beerocks::eWifiChannelType>> ret;
    const int channel_step_5g = 4;
    const int channel_step_2g = 5;
    bool is_2g                = (which_freq(channel) == beerocks::eFreqType::FREQ_24G);
    bool high_band            = wireless_utils::is_high_subband(channel);
    int last_channel          = high_band ? END_OF_HIGH_BAND : END_OF_LOW_BAND;
    LOG(INFO) << "channel = " << int(channel)
              << " channel_ext_above_secondary = " << int(channel_ext_above_secondary)
              << " channel_ext_above_primary = " << int(channel_ext_above_primary);
    auto start_of_band_channel =
        channel - channel_step_5g * (channel_step_multiply(channel_ext_above_secondary,
                                                           channel_ext_above_primary));

    if (bw == beerocks::BANDWIDTH_20) {
        LOG(INFO) << "ret.push_back( {channel, beerocks::CH_PRIMARY} ); = " << int(channel)
                  << " beerocks::CH_ " << int(beerocks::CH_PRIMARY);
        ret.push_back({channel, beerocks::CH_PRIMARY});
    } else if (bw == beerocks::BANDWIDTH_40 && is_2g) {
        LOG(INFO) << "ret.push_back( {channel, beerocks::CH_PRIMARY} ); = " << int(channel)
                  << " beerocks::CH_ " << int(beerocks::CH_PRIMARY);
        ret.push_back({channel, beerocks::CH_PRIMARY});

        if (channel_ext_above_secondary) {
            channel += channel_step_2g;
            if (which_freq(channel) == beerocks::eFreqType::FREQ_24G)
                ret.push_back({channel, beerocks::CH_SECONDARY});
        } else {
            channel -= channel_step_2g;
            if (channel > 0)
                ret.push_back({channel, beerocks::CH_SECONDARY});
        }
    } else if (bw == beerocks::BANDWIDTH_40 || bw == beerocks::BANDWIDTH_80 ||
               bw == beerocks::BANDWIDTH_160) // 5G channels
    {
        LOG(INFO) << "ret.push_back( {channel, beerocks::CH_PRIMARY} ); = "
                  << int(start_of_band_channel) << " last_channel  = " << int(last_channel);

        int iterations =
            beerocks::utils::convert_bandwidth_to_int(bw) /
            40; // 40 = 20*2. 20 for number of channels, 2 for taking half to primary and half to secondary
        beerocks::eWifiChannelType earlyIterationsChannelType =
            channel_ext_above_secondary ? beerocks::CH_PRIMARY : beerocks::CH_SECONDARY;
        beerocks::eWifiChannelType lateIterationsChannelType =
            channel_ext_above_secondary ? beerocks::CH_SECONDARY : beerocks::CH_PRIMARY;
        for (int i = 0; i < iterations; i++) {
            if (start_of_band_channel <= last_channel) {
                ret.push_back({start_of_band_channel, earlyIterationsChannelType});
                start_of_band_channel += channel_step_5g;
            }
        }

        for (int i = 0; i < iterations; i++) {
            if (start_of_band_channel <= last_channel) {
                ret.push_back({start_of_band_channel, lateIterationsChannelType});
                start_of_band_channel += channel_step_5g;
            }
        }
    }

    LOG(INFO) << "channel_step_5g" << int(channel_step_5g);
    return ret;
}

std::vector<uint8_t> wireless_utils::get_5g_20MHz_channels(beerocks::eWiFiBandwidth bw,
                                                           uint16_t vht_center_frequency)
{
    std::vector<uint8_t> channels;
    LOG(INFO) << "vht_center_frequency = " << int(vht_center_frequency);
    switch (bw) {
    case beerocks::BANDWIDTH_20: {
        channels.push_back(beerocks::utils::wifi_freq_to_channel(vht_center_frequency));
        break;
    }
    case beerocks::BANDWIDTH_40: {
        channels.push_back(beerocks::utils::wifi_freq_to_channel(vht_center_frequency - 10));
        channels.push_back(beerocks::utils::wifi_freq_to_channel(vht_center_frequency + 10));
        break;
    }
    case beerocks::BANDWIDTH_80: {
        channels.push_back(beerocks::utils::wifi_freq_to_channel(vht_center_frequency - 30));
        channels.push_back(beerocks::utils::wifi_freq_to_channel(vht_center_frequency - 10));
        channels.push_back(beerocks::utils::wifi_freq_to_channel(vht_center_frequency + 10));
        channels.push_back(beerocks::utils::wifi_freq_to_channel(vht_center_frequency + 30));
        break;
    }
    case beerocks::BANDWIDTH_160: {
        channels.push_back(beerocks::utils::wifi_freq_to_channel(vht_center_frequency - 70));
        channels.push_back(beerocks::utils::wifi_freq_to_channel(vht_center_frequency - 50));
        channels.push_back(beerocks::utils::wifi_freq_to_channel(vht_center_frequency - 30));
        channels.push_back(beerocks::utils::wifi_freq_to_channel(vht_center_frequency - 10));
        channels.push_back(beerocks::utils::wifi_freq_to_channel(vht_center_frequency + 10));
        channels.push_back(beerocks::utils::wifi_freq_to_channel(vht_center_frequency + 30));
        channels.push_back(beerocks::utils::wifi_freq_to_channel(vht_center_frequency + 50));
        channels.push_back(beerocks::utils::wifi_freq_to_channel(vht_center_frequency + 70));
        break;
    }
    default: {
        LOG(INFO) << "bw not valid";
    }
    }
    std::for_each(std::begin(channels), std::end(channels),
                  [](uint8_t channnel) { LOG(DEBUG) << "channnel = " << int(channnel); });
    return channels;
}

uint8_t wireless_utils::channel_step_multiply(bool channel_ext_above_secondary,
                                              bool channel_ext_above_primary)
{
    if (!channel_ext_above_secondary && !channel_ext_above_primary) {
        return 3;
    } else if (!channel_ext_above_secondary && channel_ext_above_primary) {
        return 2;
    } else if (channel_ext_above_secondary && !channel_ext_above_primary) {
        return 1;
    }
    return 0; //(channel_ext_above_secondary && !channel_ext_above_primary )
}

std::vector<uint8_t> wireless_utils::calc_5g_20MHz_subband_channels(
    beerocks::eWiFiBandwidth prev_bw, uint16_t prev_vht_center_frequency,
    beerocks::eWiFiBandwidth bw, uint16_t vht_center_frequency)
{
    std::vector<uint8_t> channels;

    if (prev_bw > bw) {
        std::vector<uint8_t> prev_channels =
            get_5g_20MHz_channels(prev_bw, prev_vht_center_frequency);
        std::vector<uint8_t> current_channels = get_5g_20MHz_channels(bw, vht_center_frequency);
        std::set_difference(prev_channels.begin(), prev_channels.end(), current_channels.begin(),
                            current_channels.end(), std::inserter(channels, channels.end()));
    } else if (prev_bw <= bw) {
        channels = get_5g_20MHz_channels(prev_bw, prev_vht_center_frequency);
    }
    LOG(DEBUG) << "prev_bw = " << int(prev_bw) << " bw = " << int(bw)
               << " channels empty = " << int(channels.empty());
    std::for_each(std::begin(channels), std::end(channels),
                  [](uint8_t channnel) { LOG(DEBUG) << "channnel = " << int(channnel); });

    return channels;
}

/**
 * @brief convert operating class to channel set based on Table 4-E in the ieee 802.11 specification
 *
 * @param operating_class operating class
 * @return std::set<uint8_t> set of supported channels by the operating class or empty if failure
 */
std::set<uint8_t> wireless_utils::operating_class_to_channel_set(uint8_t operating_class)
{
    static const std::map<uint8_t, std::set<uint8_t>> operating_class_to_channel_set = {
        {81, {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13}},
        {82, {14}},
        {83, {1, 2, 3, 4, 5, 6, 7, 8, 9}},
        // TODO channels for operating classes 85,96,87 should be taken from the regulatory domain
        {94, {133, 137}},
        {95, {132, 134, 136, 138}},
        {96, {131, 132, 133, 134, 135, 136, 137, 138}},
        {101, {21, 25}},
        {102, {11, 13, 15, 17, 19}},
        {103, {1, 2, 3, 4, 5, 6, 7, 8, 9, 10}},
        {104, {184, 192}},
        {105, {188, 196}},
        {106, {191, 195}},
        {107, {189, 191, 193, 195, 197}},
        {108, {188, 189, 190, 191, 192, 193, 194, 195, 196, 197}},
        {109, {184, 188, 192, 196}},
        {110, {183, 184, 185, 186, 187, 189}},
        {111, {182, 183, 184, 185, 186, 187, 189}},
        {112, {8, 12, 16}},
        {113, {7, 8, 9, 10, 11}},
        {114, {6, 7, 8, 9, 10, 11}},
        {115, {36, 40, 44, 48}},
        {116, {36, 44}},
        {117, {40, 48}},
        {118, {52, 56, 60, 64}},
        {119, {52, 60}},
        {120, {56, 64}},
        {121, {100, 104, 108, 112, 116, 120, 124, 128, 132, 136, 140, 144}},
        {122, {100, 108, 116, 124, 132, 140}},
        {123, {104, 112, 120, 128, 136, 144}},
        {124, {149, 153, 157, 161}},
        {125, {149, 153, 157, 161, 165, 169}},
        {126, {149, 157}},
        {127, {153, 161}},
        {180, {1, 2, 3, 4, 5, 6}}};

    auto it = operating_class_to_channel_set.find(operating_class);
    if (it == operating_class_to_channel_set.end()) {
        LOG(ERROR) << "reserved operating class " << int(operating_class);
        return std::set<uint8_t>();
    }
    return it->second;
}
