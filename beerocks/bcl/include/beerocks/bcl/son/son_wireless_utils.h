/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _SON_WIRELESS_UTILS_H_
#define _SON_WIRELESS_UTILS_H_

#include "../beerocks_defines.h"
#include "../beerocks_message_structs.h"

#include <vector>

#define NOISE_FIGURE            8
#define TX_POWER_MIN            19
#define ESTIMATE_RSSI_DELTA     4

#define PHY_RATE_TABLE_ANT_MODE_MAX        (beerocks::ANT_MODE_2X2_SS2 + 1)
#define PHY_RATE_TABLE_MCS_MAX             (beerocks::MCS_9 + 1) 
#define PHY_RATE_TABLE_BANDWIDTH_MAX       (beerocks::BANDWIDTH_160 + 1)

#define BIT_RATE_MAX_TABLE_SIZE 42

#define LAST_2G_CHANNEL 14

#define START_OF_HIGH_BAND 100
#define END_OF_HIGH_BAND 165  
#define END_OF_LOW_BAND 64

#define START_OF_LOW_BAND_NON_DFS 36
#define START_OF_HIGH_BAND_NON_DFS 149
#define END_OF_HIGH_BAND_NON_DFS 161
#define END_OF_LOW_BAND_NON_DFS 48
//DFS CHANNELS
#define START_OF_LOW_DFS_SUBBAND 52
#define START_OF_HIGH_DFS_SUBBAND 100
#define END_OF_LOW_DFS_SUBBAND 64
#define END_OF_HIGH_DFS_SUBBAND 144

#define START_OF_FIRST_DFS_SUBBAND 100
#define START_OF_SECOND_DFS_SUBBAND 116
#define START_OF_THIRD_DFS_SUBBAND 132
#define END_OF_FIRST_DFS_SUBBAND 112
#define END_OF_SECOND_DFS_SUBBAND 128
#define END_OF_THIRD_DFS_SUBBAND 144
//

#define BAND_5G_CHANNEL_CHECK 14
#define RADAR_STATS_LIST_MAX 10

#define RESTRICTED_2G_OVERLAPP_SIZE 5

namespace son {
    class wireless_utils {
        public:

            typedef struct{
                int tx_power;
                int rssi;
            }sPhyUlParams;

            typedef struct {
                bool is_5ghz;
                beerocks::eWiFiBandwidth bw;
                beerocks::eWiFiAntNum ant_num;
                int ant_gain;
                int conducted_power;
            }sPhyApParams;

          typedef struct{
                uint16_t phy_rate_100kb;
                double bit_rate_max_mbps;
            }sPhyRateBitRateEntry;          

            static sPhyUlParams estimate_ul_params(int ul_rssi, uint16_t sta_phy_tx_rate_100kb, const beerocks::message::sRadioCapabilities* capabilities, beerocks::eWiFiBandwidth ap_bw, bool is_5ghz);
            static int estimate_dl_rssi(int ul_rssi, int tx_power, const sPhyApParams &ap_params);
            static double estimate_ap_tx_phy_rate(int estimated_dl_rssi, const beerocks::message::sRadioCapabilities *capabilities, beerocks::eWiFiBandwidth ap_bw, bool is_5ghz);

            static double get_load_max_bit_rate_mbps(double phy_rate_100kb);
            static bool get_mcs_from_rate(const uint16_t rate, const beerocks::eWiFiAntMode ant_mode, const beerocks::eWiFiBandwidth bw, uint8_t &mcs, uint8_t &short_gi);

            static beerocks::eFreqType which_freq(uint32_t chn);
            static bool is_same_freq_band(int chn1, int chn2);
            static beerocks::eSubbandType which_subband(uint32_t chn);
            static bool is_low_subband(const uint32_t chn);
            static bool is_high_subband(const uint32_t chn);
            static bool is_dfs_channel(const uint32_t chn);
            static bool is_same_interface(const std::string &ifname1, const std::string &ifname2);
            static std::vector<std::pair<uint8_t,beerocks::eWifiChannelType>> split_channel_to_20MHz(int channel, beerocks::eWiFiBandwidth bw, bool channel_ext_above_secondary ,bool channel_ext_above_primary);
            static uint8_t channel_step_multiply( bool channel_ext_above_secondary ,bool channel_ext_above_primary );
            static std::vector<uint8_t> get_5g_20MHz_channels( beerocks::eWiFiBandwidth bw, uint16_t vht_center_frequency );
            static std::vector<uint8_t> calc_5g_20MHz_subband_channels( beerocks::eWiFiBandwidth prev_bw, uint16_t prev_vht_center_frequency, 
                                                                     beerocks::eWiFiBandwidth bw, uint16_t vht_center_frequency );

        private:

            enum eAntennaFactor {
                ANT_FACTOR_1X1 = 0,
                ANT_FACTOR_2X2 = 0,
                ANT_FACTOR_3X3 = 2,
                ANT_FACTOR_4X4 = 3,
            };

            typedef struct {
                uint16_t gi_long_rate;
                uint16_t gi_short_rate;
                int16_t rssi;
            }sPhyRateTableValues;

            typedef struct{
                int8_t tx_power_2_4;
                int8_t tx_power_5;
                sPhyRateTableValues bw_values[PHY_RATE_TABLE_BANDWIDTH_MAX]; //20/40/80/160
            }sPhyRateTableEntry;

            // LUT for phy parameters //
            static constexpr beerocks::eWiFiAntNum phy_rate_table_mode_to_ant_num[PHY_RATE_TABLE_ANT_MODE_MAX] = {beerocks::ANT_1X1, beerocks::ANT_2X2, beerocks::ANT_2X2};
            static constexpr beerocks::eWiFiSS phy_rate_table_mode_to_ss[PHY_RATE_TABLE_ANT_MODE_MAX] = {beerocks::SS_1, beerocks::SS_1, beerocks::SS_2};
            static constexpr sPhyRateTableEntry phy_rate_table[PHY_RATE_TABLE_ANT_MODE_MAX][PHY_RATE_TABLE_MCS_MAX] = {
                // 1X1_SS1_table
                {//MCS 0-9:{TX_power 2.4/5 ,{{20_rate_long/short,20_RSSI},{40_rate_long/short,40_RSSI},{80_rate_long/short,80_RSSI},{160_rate_long/short,160_RSSI}}}
                {18, 16,    {{65,   72,  -892},     {135,     150,    -860},	{293,   325,    -824}, {585,   650,    -794}}},
                {18, 16,    {{130,  144, -865},     {270,     300,    -841},	{585,   650,    -805}, {1170,  1300,   -775}}},
                {17, 15,    {{195,  217, -808},     {405,     450,    -787},	{878,   975,    -756}, {1175,  1950,   -726}}},
                {17, 15,    {{260,  289, -811},     {540,     600,    -790},	{1170,  1300,   -753}, {2340,  2600,   -723}}},
                {17, 15,    {{390,  433, -755},     {810,     900,    -727},	{1755,  1950,   -698}, {3510,  3900,   -668}}},
                {17, 15,    {{520,  578, -717},     {1080,    1200,   -686},	{2340,  2600,   -656}, {4680,  5200,   -626}}},
                {17, 14,    {{585,  650, -698},     {1215,    1350,   -672},	{2633,  2925,   -641}, {5265,  5850,   -611}}},
                {16, 13,    {{650,  722, -673},     {1350,    1500,   -647},	{2925,  3250,   -632}, {5850,  6500,   -602}}},
                {0,  10,    {{780,  867, -644},     {1620,    1800,   -619},	{3510,  3900,   -587}, {7020,  7800,   -557}}},
                {0,  8,     {{0,    0,   0},        {1800,    2000,   -599},	{3900,  4333,   -558}, {7800,  8667,   -528}}},
                },
                // 2X2_SS1_table
                {//MCS 0-9:{TX_power,{{20_rate_long/short,20_RSSI},{40_rate_long/short,40_RSSI},{80_rate_long/short,80_RSSI},{160_rate_long/short,160_RSSI}}}
                {18, 16,    {{65,  72,  -912},      {135,     150,    -894},    {293,     325,    -855}, {585,   650,    -825}}},
                {18, 16,    {{130, 144, -900},      {270,     300,    -879},    {585,     650,    -840}, {1170,  1300,   -810}}},
                {17, 15,    {{195, 217, -869},      {405,     450,    -843},    {878,     975,    -805}, {1175,  1950,   -775}}},
                {17, 15,    {{260, 289, -858},      {540,     600,    -831},    {1170,    1300,   -792}, {2340,  2600,   -762}}},
                {17, 15,    {{390, 433, -810},      {810,     900,    -785},    {1755,    1950,   -748}, {3510,  3900,   -718}}},
                {17, 15,    {{520, 578, -773},      {1080,    1200,   -742},    {2340,    2600,   -704}, {4680,  5200,   -674}}},
                {17, 14,    {{585, 650, -756},      {1215,    1350,   -729},    {2633,    2925,   -692}, {5265,  5850,   -662}}},
                {16, 13,    {{650, 722, -735},      {1350,    1500,   -709},    {2925,    3250,   -684}, {5850,  6500,   -654}}},
                {0,  10,    {{780, 867, -699},      {1620,    1800,   -674},    {3510,    3900,   -637}, {7020,  7800,   -607}}},
                {0,  8,     {{0,   0,   0},         {1800,    2000,   -659},    {3900,    4333,   -614}, {7800,  8667,   -584}}},
                },
                // 2X2_SS2_table
                {//MCS 0-9:{TX_power,{{20_rate_long/short,20_RSSI},{40_rate_long/short,40_RSSI},{80_rate_long/short,80_RSSI},{160_rate_long/short,160_RSSI}}}
                {18, 16,    {{130,  144,	-890},  {270,   300,    -861},      {585,   650,    -834}, {1170,    1300,    -804}}},
                {18, 16,    {{260,  288,	-855},  {540,   600,    -828},      {1170,  1300,   -795}, {2340,    2600,    -765}}},
                {17, 15,    {{390,  434,	-815},  {810,   900,    -784},      {1755,  1950,   -756}, {3510,    3900,    -726}}},
                {17, 15,    {{520,  578,	-774},  {1080,  1200,   -755},      {2340,  2600,   -718}, {4680,    5200,    -688}}},
                {17, 15,    {{780,  866,	-720},  {1620,  1800,	-699},      {3510,  3900,   -669}, {7020,    7800,    -639}}},
                {17, 15,    {{1040, 1156,	-657},  {2160,  2400,	-640},      {4680,  5200,   -604}, {9360,    10400,   -574}}},
                {17, 14,    {{1170, 1300,	-648},  {2430,  2700,	-628},      {5265,  5850,	-594}, {10530,   11700,   -564}}},
                {16, 13,    {{1300, 1444,	-634},  {2700,  3000,	-617},      {5850,  6500,	-574}, {11700,   13000,   -544}}},
                {0,  10,    {{1560, 1733,	-581},  {3240,  3600,	-584},      {7020,  7800,	-525}, {14040,   15600,   -495}}},
                {0,  8,     {{0,    0,      0},     {3600,  4000,	-556},      {7800,  8666,	-497}, {15600,   17333,   -467}}},
                }
            };

         static constexpr sPhyRateBitRateEntry bit_rate_max_table_mbps[BIT_RATE_MAX_TABLE_SIZE] = {  
                {65,3},     {130,86},   {135,71},   {195,89},   {234,107},  {260,92},   {270,125},  {293,129},  {390,133},  {405,139},
                {520,140},  {540,141},  {585,146},  {650,147},  {780,148},  {810,151},  {878,151},  {1040,161}, {1080,172}, {1170,182},
                {1215,156}, {1300,189}, {1350,157}, {1560,196}, {1620,202}, {1755,208}, {1800,161}, {2160,213}, {2340,241}, {2430,220},
                {2633,169}, {2700,232}, {2925,189}, {3240,237}, {3510,251}, {3600,246}, {3900,328}, {4680,251}, {5265,253}, {5850,261}, 
                {7020,266}, {7800,266}  
        };

    };
}

#endif
