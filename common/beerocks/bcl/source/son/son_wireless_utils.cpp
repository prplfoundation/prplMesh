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
    sPhyUlParams estimation = {0, beerocks::RSSI_INVALID, ESTIMATION_FAILURE_INVALID_RSSI};

    const int max_ant_mode = (sta_capabilities->ant_num == beerocks::ANT_1X1)
                                 ? beerocks::ANT_MODE_1X1_SS1
                                 : beerocks::ANT_MODE_2X2_SS2;
    const int max_mcs = (is_5ghz && (sta_capabilities->wifi_standard & int(beerocks::STANDARD_AC)))
                            ? sta_capabilities->vht_mcs
                            : sta_capabilities->ht_mcs;
    int max_bw = (is_5ghz && (sta_capabilities->wifi_standard & int(beerocks::STANDARD_AC)))
                     ? sta_capabilities->vht_bw
                     : sta_capabilities->ht_bw;
    if (ap_bw < max_bw) {
        max_bw = ap_bw;
    }

    max_bw = (max_bw > beerocks::BANDWIDTH_160 ? beerocks::BANDWIDTH_160 : max_bw);

    LOG(DEBUG) << "UL RSSI:" << ul_rssi << " | sta_phy_tx_rate:" << sta_phy_tx_rate_100kb / 10
               << " Mbps | AP BW:"
               << beerocks::utils::convert_bandwidth_to_int((beerocks::eWiFiBandwidth)ap_bw)
               << " | is_5ghz:" << is_5ghz << " | ant_num:" << int(sta_capabilities->ant_num)
               << " | max_ant_mode:" << max_ant_mode << " | max MCS:" << max_mcs << " | max BW:"
               << beerocks::utils::convert_bandwidth_to_int((beerocks::eWiFiBandwidth)max_bw);

    if (ul_rssi == beerocks::RSSI_INVALID) {
        LOG(DEBUG) << "Can not estimate UL parameters (invalid RSSI)";
        return estimation;
    }

    // If station phyrate value is below table's minimum, return minimal estimation
    if (sta_phy_tx_rate_100kb < phy_rate_table[0][0].bw_values[0].gi_long_rate) {
        LOG(DEBUG) << "Can not estimate UL parameters (STA phyrate is too low)";
        estimation.tx_power =
            is_5ghz ? phy_rate_table[0][0].tx_power_5 : phy_rate_table[0][0].tx_power_2_4;
        estimation.rssi   = int(ceil(phy_rate_table[0][0].bw_values[0].rssi / 10.0));
        estimation.status = ESTIMATION_FAILURE_BELOW_RANGE;

        return estimation;
    }

    // If station phyrate value is above table's maximum, return maximal estimation
    if (sta_phy_tx_rate_100kb >
        phy_rate_table[max_ant_mode][max_mcs].bw_values[max_bw].gi_short_rate) {
        LOG(DEBUG) << "STA phy rate (" << sta_phy_tx_rate_100kb / 10
                   << " Mbps) is above maximum possible in current MCS/NSS/BW mode ("
                   << phy_rate_table[max_ant_mode][max_mcs].bw_values[max_bw].gi_short_rate / 10
                   << " Mbps)";

        estimation.status   = ESTIMATION_SUCCESS;
        estimation.tx_power = is_5ghz ? phy_rate_table[max_ant_mode][max_mcs].tx_power_5
                                      : phy_rate_table[max_ant_mode][max_mcs].tx_power_2_4;
        estimation.rssi =
            int(ceil(phy_rate_table[max_ant_mode][max_mcs].bw_values[max_bw].rssi / 10.0));

        LOG(DEBUG) << "Return maximal estimation values | tx_power:" << estimation.tx_power
                   << " | RSSI:" << estimation.rssi;

        return estimation;
    }

    for (int ant_mode = max_ant_mode; ant_mode > -1; ant_mode--) { // filter by ant_mode
        for (int bw = max_bw; bw > -1; bw--) {                     // filter by max_bw
            for (int mcs = max_mcs; mcs > -1; mcs--) {             // filter by mcs

                estimated_ul_rssi_lut       = phy_rate_table[ant_mode][mcs].bw_values[bw].rssi;
                estimated_ul_rssi_lut_delta = std::abs(ul_rssi_lut - estimated_ul_rssi_lut);

                auto gi_long_rate  = phy_rate_table[ant_mode][mcs].bw_values[bw].gi_long_rate;
                auto gi_short_rate = phy_rate_table[ant_mode][mcs].bw_values[bw].gi_short_rate;

                // Check if the current rate is between the gi_long_rate to the gi_short_rate
                if ((sta_phy_tx_rate_100kb >= gi_long_rate) &&
                    (sta_phy_tx_rate_100kb <= gi_short_rate)) {
                    // phyrate is in range - use table values
                    if (estimated_ul_rssi_lut_delta <= estimated_ul_rssi_lut_delta_min) {
                        estimated_ul_rssi_lut_delta_min = estimated_ul_rssi_lut_delta;
                        estimation.tx_power = is_5ghz ? phy_rate_table[ant_mode][mcs].tx_power_5
                                                      : phy_rate_table[ant_mode][mcs].tx_power_2_4;
                        estimation.rssi = int(ceil(estimated_ul_rssi_lut / 10.0));
                    }
                    continue;
                }

                // phyrate is not in range - use average rssi delta

                // Since we use the avarage, and using the [mcs -1], continue in case of mcs=0
                // to prevent segfault.
                if (mcs == 0) {
                    continue;
                }

                // Skip if the current rate is not between current gi_long_rate to the
                // (mcs -1) gi_short_rate.
                if (!((sta_phy_tx_rate_100kb <= gi_long_rate) &&
                      (sta_phy_tx_rate_100kb >=
                       phy_rate_table[ant_mode][mcs - 1].bw_values[bw].gi_short_rate))) {
                    continue;
                }

                // update rssi estimation and delta
                estimated_ul_rssi_lut = (phy_rate_table[ant_mode][mcs].bw_values[bw].rssi +
                                         phy_rate_table[ant_mode][mcs - 1].bw_values[bw].rssi) /
                                        2;

                estimated_ul_rssi_lut_delta = std::abs(ul_rssi_lut - estimated_ul_rssi_lut);

                if (estimated_ul_rssi_lut_delta <= estimated_ul_rssi_lut_delta_min) {
                    estimated_ul_rssi_lut_delta_min = estimated_ul_rssi_lut_delta;

                    estimation.tx_power = is_5ghz ? phy_rate_table[ant_mode][mcs].tx_power_5
                                                  : phy_rate_table[ant_mode][mcs].tx_power_2_4;

                    estimation.rssi = int(ceil(estimated_ul_rssi_lut / 10.0));
                }
            }
        }
    }

    estimation.status = ESTIMATION_SUCCESS;

    LOG(DEBUG) << "Successful estimation | tx_power:" << estimation.tx_power
               << " | RSSI:" << estimation.rssi;

    return estimation;
}

int wireless_utils::estimate_dl_rssi(int ul_rssi, int tx_power, const sPhyApParams &ap_params)
{
    int eirp_sta   = tx_power;
    int eirp_ap    = ap_params.ant_gain + ap_params.conducted_power;
    int ant_factor = ANT_FACTOR_2X2;
    float pathloss;
    int dl_rssi;

    if (ap_params.ant_num == beerocks::ANT_4X4) {
        ant_factor = ANT_FACTOR_4X4;
    } else if (ap_params.ant_num == beerocks::ANT_3X3) {
        ant_factor = ANT_FACTOR_3X3;
    }

    pathloss = eirp_sta - (ul_rssi - ant_factor - ap_params.ant_gain);

    if (ap_params.is_5ghz) {
        pathloss += NOISE_FIGURE; // 5GHz extra loss
    }

    dl_rssi = eirp_ap - pathloss;

    LOG(DEBUG) << " eirp_sta:" << eirp_sta << " | UL RSSI:" << int(ul_rssi)
               << " | ant_factor:" << ant_factor << " | ant_gain:" << ap_params.ant_gain
               << " | eirp_ap:" << eirp_ap << " | pathloss:" << int(pathloss)
               << " | Returns estimated DL RSSI:" << int(dl_rssi);

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

    LOG(DEBUG) << "estimated DL RSSI:" << int(estimated_dl_rssi)
               << " | AP BW:" << beerocks::utils::convert_bandwidth_to_int(ap_bw)
               << " | Return estimated PHY RATE:" << int(estimated_phy_rate / 1e+6) << " Mbps";

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

    LOG(DEBUG) << "rate:" << rate << " | BW:" << beerocks::utils::convert_bandwidth_to_int(bw)
               << " | ant_mode:" << ant_mode << " | Return MCS:" << mcs << " and GI:" << short_gi;

    return false;
}

beerocks::eFreqType wireless_utils::which_freq(uint32_t chn)
{
    if ((1 <= chn) && (chn <= BAND_5G_CHANNEL_CHECK)) {
        return beerocks::eFreqType::FREQ_24G;
    }

    if (START_OF_LOW_BAND_NON_DFS <= chn) {
        return beerocks::eFreqType::FREQ_5G;
    }

    LOG(ERROR) << "Unsupported channel:" << int(chn);
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
    if ((START_OF_LOW_BAND_NON_DFS <= chn) && (chn <= END_OF_LOW_BAND)) {
        return beerocks::eSubbandType::LOW_SUBBAND;
    }

    if ((START_OF_HIGH_BAND <= chn) && (chn <= END_OF_HIGH_BAND)) {
        return beerocks::eSubbandType::HIGH_SUBBAND;
    }

    LOG(ERROR) << "Unsupported channel:" << (int)chn;
    return beerocks::eSubbandType::SUBBAND_UNKNOWN;
}

bool wireless_utils::is_low_subband(const uint32_t chn)
{
    return (which_subband(chn) == beerocks::eSubbandType::LOW_SUBBAND);
}

bool wireless_utils::is_high_subband(const uint32_t chn)
{
    return (which_subband(chn) == beerocks::eSubbandType::HIGH_SUBBAND);
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
        LOG(ERROR) << "INVALID BW:" << bw;
    }
    }
    std::for_each(std::begin(channels), std::end(channels),
                  [](uint8_t channel) { LOG(DEBUG) << "channel:" << int(channel); });
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
    LOG(DEBUG) << "prev_bw:" << beerocks::utils::convert_bandwidth_to_int(prev_bw)
               << " | BW:" << beerocks::utils::convert_bandwidth_to_int(bw)
               << " | channels empty:" << int(channels.empty());

    std::for_each(std::begin(channels), std::end(channels),
                  [](uint8_t channel) { LOG(DEBUG) << "channel:" << int(channel); });

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

std::string wireless_utils::wsc_to_bwl_authentication(WSC::eWscAuth authtype)
{
    switch (authtype) {
    case WSC::eWscAuth::WSC_AUTH_OPEN:
        return "NONE";
    case WSC::eWscAuth::WSC_AUTH_WPAPSK:
        return "WPA-PSK";
    case WSC::eWscAuth::WSC_AUTH_SHARED:
        return "SHARED";
    case WSC::eWscAuth::WSC_AUTH_WPA:
        return "WPA";
    case WSC::eWscAuth::WSC_AUTH_WPA2:
        return "WPA2";
    case WSC::eWscAuth::WSC_AUTH_WPA2PSK:
        return "WPA2-PSK";
    default:
        return "INVALID";
    }
}

std::string wireless_utils::wsc_to_bwl_encryption(WSC::eWscEncr enctype)
{
    switch (enctype) {
    case WSC::eWscEncr::WSC_ENCR_NONE:
        return "NONE";
    case WSC::eWscEncr::WSC_ENCR_WEP:
        return "WEP";
    case WSC::eWscEncr::WSC_ENCR_TKIP:
        return "TKIP";
    case WSC::eWscEncr::WSC_ENCR_AES:
        return "AES";
    default:
        return "INVALID";
    }
}

beerocks::eBssType wireless_utils::wsc_to_bwl_bss_type(WSC::eWscVendorExtSubelementBssType bss_type)
{
    if ((bss_type & WSC::eWscVendorExtSubelementBssType::BACKHAUL_BSS) &&
        (bss_type & WSC::eWscVendorExtSubelementBssType::FRONTHAUL_BSS)) {
        return beerocks::BSS_TYPE_BACK_FRONTHAUL;
    } else if (bss_type & WSC::eWscVendorExtSubelementBssType::BACKHAUL_BSS)
        return beerocks::BSS_TYPE_BACKHAUL;
    else if (bss_type & WSC::eWscVendorExtSubelementBssType::FRONTHAUL_BSS)
        return beerocks::BSS_TYPE_FRONTHAUL;
    else if (bss_type & WSC::eWscVendorExtSubelementBssType::TEARDOWN)
        return beerocks::BSS_TYPE_TEARDOWN;

    return beerocks::BSS_TYPE_INVALID;
}
