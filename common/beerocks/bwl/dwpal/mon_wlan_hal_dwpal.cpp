/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include "mon_wlan_hal_dwpal.h"

#include <bcl/beerocks_utils.h>
#include <bcl/network/network_utils.h>
#include <bcl/son/son_wireless_utils.h>

#include <easylogging++.h>
#include <net/if.h>

#include <cmath>
#include <functional>

extern "C" {
#include <dwpal.h>
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////// DWPAL////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

namespace bwl {
namespace dwpal {

//////////////////////////////////////////////////////////////////////////////
////////////////////////// Local Module Definitions //////////////////////////
//////////////////////////////////////////////////////////////////////////////

enum ie_type : uint8_t {
    TYPE_SSID                     = 0,
    TYPE_SUPPORTED_RATES          = 1,
    TYPE_TIM                      = 5,
    TYPE_BSS_LOAD                 = 11,
    TYPE_RSN                      = 48,
    TYPE_EXTENDED_SUPPORTED_RATES = 50,
    TYPE_HT_OPERATION             = 61,
    TYPE_VHT_OPERATION            = 192,
    TYPE_VENDOR                   = 221
};

#define WLAN_CAPABILITY_ESS (1 << 0)
#define WLAN_CAPABILITY_IBSS (1 << 1)
#define WLAN_CAPABILITY_PRIVACY (1 << 4)
#define GET_OP_CLASS(channel) ((channel < 14) ? 4 : 5)

// Allocate a char array wrapped in a shared_ptr
#define ALLOC_SMART_BUFFER(size)                                                                   \
    std::shared_ptr<char>(new char[size], [](char *obj) {                                          \
        if (obj)                                                                                   \
            delete[] obj;                                                                          \
    })

//////////////////////////////////////////////////////////////////////////////
/////////////////////////// Local Module Functions ///////////////////////////
//////////////////////////////////////////////////////////////////////////////

static mon_wlan_hal::Event dwpal_to_bwl_event(const std::string &opcode)
{
    if (opcode == "RRM-CHANNEL-LOAD-RECEIVED") {
        return mon_wlan_hal::Event::RRM_Channel_Load_Response;
    } else if (opcode == "RRM-BEACON-REP-RECEIVED") {
        return mon_wlan_hal::Event::RRM_Beacon_Response;
    } else if (opcode == "RRM-STA-STATISTICS-RECEIVED") {
        return mon_wlan_hal::Event::RRM_STA_Statistics_Response;
    } else if (opcode == "AP-ENABLED") {
        return mon_wlan_hal::Event::AP_Enabled;
    } else if (opcode == "AP-DISABLED") {
        return mon_wlan_hal::Event::AP_Disabled;
    } else if (opcode == "AP-STA-CONNECTED") {
        return mon_wlan_hal::Event::STA_Connected;
    } else if (opcode == "AP-STA-DISCONNECTED") {
        return mon_wlan_hal::Event::STA_Disconnected;
    }

    return mon_wlan_hal::Event::Invalid;
}

static mon_wlan_hal::Event dwpal_nl_to_bwl_event(uint8_t cmd)
{
    switch (cmd) {
    case NL80211_CMD_TRIGGER_SCAN:
        return mon_wlan_hal::Event::Channel_Scan_Triggered;
    case NL80211_CMD_NEW_SCAN_RESULTS:
        return mon_wlan_hal::Event::Channel_Scan_Dump_Result;
    case NL80211_CMD_SCAN_ABORTED:
        return mon_wlan_hal::Event::Channel_Scan_Abort;
    case SCAN_FINISH_CB:
        return mon_wlan_hal::Event::Channel_Scan_Finished;
    default:
        LOG(ERROR) << "Unknown event received: " << int(cmd);
        return mon_wlan_hal::Event::Invalid;
    }
}

/**
 * @brief get channel pool frquencies for channel scan parameters.
 *
 * @param [in] channel_pool list of channels to be scanned.
 * @param [in] curr_channel channel teh radio is currently on.
 * @param [in] iface radio interface name.
 * @param [out] scan_params for saving channel frequencies for next scan. 
 * @return true on success
 */
static bool dwpal_get_channel_scan_freq(const std::vector<unsigned int> &channel_pool,
                                        unsigned int curr_channel, const std::string &iface,
                                        ScanParams &scan_params)
{
    int freq_index = 0;
    //configure center frequency for each scanned channel
    for (auto channel : channel_pool) {
        //channel validation
        LOG(DEBUG) << "validating pool channel=" << channel;
        if (son::wireless_utils::which_freq(curr_channel) !=
            son::wireless_utils::which_freq(channel)) {
            LOG(ERROR) << "cannot scan channel = " << channel
                       << " not on the same radio interface =  " << iface;
            return false;
        }

        scan_params.freq[freq_index] = son::wireless_utils::channel_to_freq(int(channel));
        LOG(DEBUG) << "channel scan pool add center frequency=" << scan_params.freq[freq_index];
        freq_index++;
    }
    return true;
};

static bool read_nl_data_from_msg(struct nlattr **bss, struct nl_msg *msg)
{
    struct genlmsghdr *gnlh = (genlmsghdr *)nlmsg_data(nlmsg_hdr(msg));
    struct nlattr *tb[NL80211_ATTR_MAX + 1];
    static struct nla_policy bss_policy[NL80211_BSS_MAX + 1];

    if (!bss || !msg) {
        LOG(ERROR) << "invalid input bss=" << bss << ", msg=" << msg;
        return false;
    }

    bss_policy[NL80211_BSS_BSSID]                = {};
    bss_policy[NL80211_BSS_FREQUENCY].type       = NLA_U32;
    bss_policy[NL80211_BSS_TSF].type             = NLA_U64;
    bss_policy[NL80211_BSS_BEACON_INTERVAL].type = NLA_U16;
    bss_policy[NL80211_BSS_CAPABILITY].type      = NLA_U16;
    bss_policy[NL80211_BSS_INFORMATION_ELEMENTS] = {};
    bss_policy[NL80211_BSS_SIGNAL_MBM].type      = NLA_U32;
    bss_policy[NL80211_BSS_SIGNAL_UNSPEC].type   = NLA_U8;
    bss_policy[NL80211_BSS_STATUS].type          = NLA_U32;
    bss_policy[NL80211_BSS_SEEN_MS_AGO].type     = NLA_U32;
    bss_policy[NL80211_BSS_BEACON_IES]           = {};

    nla_parse(tb, NL80211_ATTR_MAX, genlmsg_attrdata(gnlh, 0), genlmsg_attrlen(gnlh, 0), NULL);

    if (!tb[NL80211_ATTR_BSS]) {
        LOG(ERROR) << "netlink message is missing the BSS attribute";
        return false;
    }
    if (nla_parse_nested(bss, NL80211_BSS_MAX, tb[NL80211_ATTR_BSS], bss_policy)) {
        LOG(ERROR) << "Failed parsing nested netlink BSS attribute";
        return false;
    }
    if (!bss[NL80211_BSS_BSSID]) {
        LOG(ERROR) << "netlink message is missing the BSSID attribute";
        return false;
    }

    return true;
}

static void get_ht_oper(const uint8_t *data, sChannelScanResults &results)
{
    if (!data) {
        LOG(ERROR) << "data buffer is NULL";
        return;
    }

    if (!(data[1] & 0x3)) {
        results.operating_channel_bandwidth =
            eChannelScanResultChannelBandwidth::eChannel_Bandwidth_20MHz;
    } else if ((data[1] & 0x3) != 2) {
        results.operating_channel_bandwidth =
            eChannelScanResultChannelBandwidth::eChannel_Bandwidth_40MHz;
    }

    results.supported_standards.push_back(eChannelScanResultStandards::eStandard_802_11n);
    results.operating_standards = eChannelScanResultStandards::eStandard_802_11n;
}

static void get_vht_oper(const uint8_t *data, sChannelScanResults &results)
{
    if (!data) {
        LOG(ERROR) << "data buffer is NULL";
        return;
    }

    if (data[0] == 0x01) {
        if (data[2]) {
            results.operating_channel_bandwidth =
                eChannelScanResultChannelBandwidth::eChannel_Bandwidth_160MHz;
        } else {
            results.operating_channel_bandwidth =
                eChannelScanResultChannelBandwidth::eChannel_Bandwidth_80MHz;
        }
    }

    if (data[0] == 0x02) {
        results.operating_channel_bandwidth =
            eChannelScanResultChannelBandwidth::eChannel_Bandwidth_80MHz;
    }

    if (data[0] == 0x03) {
        results.operating_channel_bandwidth =
            eChannelScanResultChannelBandwidth::eChannel_Bandwidth_80_80;
    }

    if (data[0] > 0x03) {
        LOG(ERROR) << "illegal TYPE_VHT_OPERATION value=" << data[0];
    }

    if (results.operating_frequency_band ==
        eChannelScanResultOperatingFrequencyBand::eOperating_Freq_Band_5GHz) {
        results.supported_standards.push_back(eChannelScanResultStandards::eStandard_802_11ac);
        results.operating_standards = eChannelScanResultStandards::eStandard_802_11ac;
    }
}

static void get_supprates(const uint8_t *data, uint8_t len, sChannelScanResults &results)
{
    uint8_t rate_mbs_fp_8_1;
    uint32_t rate_kbs;

    if (!data) {
        LOG(ERROR) << "data buffer is NULL";
        return;
    }

    for (int i = 0; i < len; i++) {
        rate_mbs_fp_8_1 = data[i] & 0x7f;

        if (rate_mbs_fp_8_1 / 2 == 11) {
            if (results.operating_frequency_band ==
                eChannelScanResultOperatingFrequencyBand::eOperating_Freq_Band_2_4GHz) {
                results.supported_standards.push_back(
                    eChannelScanResultStandards::eStandard_802_11b);
                results.operating_standards = eChannelScanResultStandards::eStandard_802_11b;
            }
        } else if (rate_mbs_fp_8_1 / 2 == 54) {
            if (results.operating_frequency_band ==
                eChannelScanResultOperatingFrequencyBand::eOperating_Freq_Band_5GHz) {
                results.supported_standards.push_back(
                    eChannelScanResultStandards::eStandard_802_11a);
                results.operating_standards = eChannelScanResultStandards::eStandard_802_11a;
            }
        }

        /**
         * rate_mbs_fp_8_1 is tx data rate in mbps 
         * represented with fixed point u<8,1>.
         * converting to kbps (no fixpoint) for simplicity u<8,0> 
         */
        rate_kbs = (rate_mbs_fp_8_1 / 2) * 1000 + (5 * (rate_mbs_fp_8_1 & 1)) * 100;

        if (data[i] & 0x80) {
            results.basic_data_transfer_rates_kbps.push_back(rate_kbs);
        } else {
            results.supported_data_transfer_rates_kbps.push_back(rate_kbs);
        }
    }
}

static void parse_info_elements(unsigned char *ie, int ielen, sChannelScanResults &results)
{
    if (!ie) {
        LOG(ERROR) << "info elements buffer is NULL";
        return;
    }

    while (ielen >= 2 && ielen >= ie[1]) {
        auto key      = ie[0];
        auto length   = ie[1];
        uint8_t *data = ie + 2;

        switch (key) {
        case ie_type::TYPE_SSID: {
            if (length > 32) {
                LOG(ERROR) << "TYPE_SSID doesn't match min and max length criteria";
                break;
            }
            std::copy_n(data, length, results.ssid);
        } break;

        case ie_type::TYPE_SUPPORTED_RATES: {
            get_supprates(data, length, results);
        } break;

        case ie_type::TYPE_TIM: {
            if (length < 4) {
                LOG(ERROR) << "TYPE_TIM doesn't match min and max length criteria";
                break;
            }
            results.dtim_period = (uint32_t)data[1];
        } break;

        case ie_type::TYPE_BSS_LOAD: {
            if (length != 5) {
                LOG(ERROR) << "TYPE_BSS_LOAD doesn't match min and max length criteria";
                break;
            }
            results.channel_utilization = (uint32_t)(data[2] / 255);
        } break;

        case ie_type::TYPE_RSN: {
            if (length < 2) {
                LOG(ERROR) << "TYPE_RSN doesn't match min and max length criteria";
                break;
            }
            results.encryption_mode.push_back(
                eChannelScanResultEncryptionMode::eEncryption_Mode_AES);
            results.security_mode_enabled.push_back(
                eChannelScanResultSecurityMode::eSecurity_Mode_WPA2);
        } break;

        case ie_type::TYPE_EXTENDED_SUPPORTED_RATES: {

            if (results.operating_frequency_band ==
                eChannelScanResultOperatingFrequencyBand::eOperating_Freq_Band_2_4GHz) {
                results.supported_standards.push_back(
                    eChannelScanResultStandards::eStandard_802_11g);
                results.operating_standards = eChannelScanResultStandards::eStandard_802_11g;
            }

            get_supprates(data, length, results);

        } break;

        case ie_type::TYPE_HT_OPERATION: {
            if (length != 22) {
                LOG(ERROR) << "TYPE_HT_OPERATION doesn't match min and max length criteria";
                break;
            }
            get_ht_oper(data, results);
        } break;

        case ie_type::TYPE_VHT_OPERATION: {
            if (length < 5) {
                LOG(ERROR) << "TYPE_VHT_OPERATION doesn't match min and max length criteria";
                break;
            }
            get_vht_oper(data, results);
        } break;

        default: {
            LOG(ERROR) << "Unhandled element received: " << int(key);
        } break;
        }

        ielen -= length + 2;
        ie += length + 2;
    }
}

static bool translate_nl_data_to_bwl_results(sChannelScanResults &results,
                                             const struct nlattr **bss)
{
    if (!bss[NL80211_BSS_BSSID]) {
        LOG(ERROR) << "Invalid BSSID in the netlink message";
        return false;
    }

    std::copy_n(reinterpret_cast<unsigned char *>(nla_data(bss[NL80211_BSS_BSSID])),
                sizeof(results.bssid), results.bssid.oct);

    //get channel and operating frequency band
    if (bss[NL80211_BSS_FREQUENCY]) {
        int freq = nla_get_u32(bss[NL80211_BSS_FREQUENCY]);
        if (freq >= 5180) {
            results.operating_frequency_band =
                eChannelScanResultOperatingFrequencyBand::eOperating_Freq_Band_5GHz;
        } else {
            results.operating_frequency_band =
                eChannelScanResultOperatingFrequencyBand::eOperating_Freq_Band_2_4GHz;
        }
        results.channel = son::wireless_utils::freq_to_channel(freq);
    }

    // get beacon period
    if (bss[NL80211_BSS_BEACON_INTERVAL]) {
        results.beacon_period_ms = (unsigned int)nla_get_u16(bss[NL80211_BSS_BEACON_INTERVAL]);
    }

    // get signal strength, signal strength units not specified, scaled to 0-100
    if (bss[NL80211_BSS_SIGNAL_UNSPEC]) {
        //signal strength of the probe response/beacon in unspecified units, scaled to 0..100 <u8>
        results.signal_strength_dBm = int32_t(nla_get_u8(bss[NL80211_BSS_SIGNAL_UNSPEC]));
    } else if (bss[NL80211_BSS_SIGNAL_MBM]) {
        //signal strength of probe response/beacon in mBm (100 * dBm) <s32>
        results.signal_strength_dBm = int32_t(nla_get_u32(bss[NL80211_BSS_SIGNAL_MBM])) / 100;
    }

    //get information elements from information-elements-buffer or from beacon
    if (bss[NL80211_BSS_BEACON_IES] || bss[NL80211_BSS_INFORMATION_ELEMENTS]) {
        enum nl80211_bss ies_index = (bss[NL80211_BSS_INFORMATION_ELEMENTS])
                                         ? NL80211_BSS_INFORMATION_ELEMENTS
                                         : NL80211_BSS_BEACON_IES;
        parse_info_elements((unsigned char *)nla_data(bss[ies_index]), nla_len(bss[ies_index]),
                            results);
    }

    //get capabilities: mode, security_mode_enabled
    if (bss[NL80211_BSS_CAPABILITY]) {
        uint16_t capa = nla_get_u16(bss[NL80211_BSS_CAPABILITY]);

        if (capa & WLAN_CAPABILITY_IBSS) {
            results.mode = eChannelScanResultMode::eMode_AdHoc;
        } else if (capa & WLAN_CAPABILITY_ESS) {
            results.mode = eChannelScanResultMode::eMode_Infrastructure;
        }

        if (results.security_mode_enabled.size() == 0) {
            if (capa & WLAN_CAPABILITY_PRIVACY) {
                results.security_mode_enabled.push_back(
                    eChannelScanResultSecurityMode::eSecurity_Mode_WEP);
            } else {
                results.security_mode_enabled.push_back(
                    eChannelScanResultSecurityMode::eSecurity_Mode_None);
            }
        }
    }

    return true;
}

static bool get_scan_results_from_nl_msg(sChannelScanResults &results, struct nl_msg *msg)
{
    struct nlattr *bss[NL80211_BSS_MAX + 1];

    if (!msg) {
        LOG(ERROR) << "invalid input: msg==NULL" << msg;
        return false;
    }

    //read msg buffer into nl attributes struct
    if (!read_nl_data_from_msg(bss, msg)) {
        LOG(ERROR) << "failed to parse netlink message";
        return false;
    }

    if (!translate_nl_data_to_bwl_results(results, (const nlattr **)bss)) {
        LOG(ERROR) << "failed to translate nl data to BWL results";
        return false;
    }

    return true;
}

static std::shared_ptr<char> generate_client_assoc_event(const std::string &event, int vap_id)
{
    auto msg_buff = ALLOC_SMART_BUFFER(sizeof(sACTION_MONITOR_CLIENT_ASSOCIATED_NOTIFICATION));
    auto msg = reinterpret_cast<sACTION_MONITOR_CLIENT_ASSOCIATED_NOTIFICATION *>(msg_buff.get());

    if (!msg) {
        LOG(FATAL) << "Memory allocation failed";
        return nullptr;
    }

    // Initialize the message
    memset(msg_buff.get(), 0, sizeof(sACTION_MONITOR_CLIENT_ASSOCIATED_NOTIFICATION));

    char client_mac[MAC_ADDR_SIZE] = {0};
    // These parameters are not needed, but the dwpal_string_to_struct_parse fails without them.
    int supported_rates[16]  = {0};
    int HT_MCS[16]           = {0};
    int16_t VHT_MCS[1]       = {0};
    char ht_cap[8]           = {0};
    char ht_mcs[64]          = {0};
    char vht_cap[16]         = {0};
    char vht_mcs[24]         = {0};
    int8_t max_tx_power      = 0;
    size_t numOfValidArgs[7] = {0};

    FieldsToParse fieldsToParse[] = {
        {(void *)client_mac, &numOfValidArgs[0], DWPAL_STR_PARAM, NULL, sizeof(client_mac)},
        {(void *)supported_rates, &numOfValidArgs[1], DWPAL_INT_HEX_ARRAY_PARAM,
         "supported_rates=", sizeof(supported_rates)},
        {(void *)ht_cap, &numOfValidArgs[2], DWPAL_STR_PARAM, "ht_caps_info=", sizeof(ht_cap)},
        {(void *)ht_mcs, &numOfValidArgs[3], DWPAL_STR_PARAM, "ht_mcs_bitmask=", sizeof(ht_mcs)},
        {(void *)vht_cap, &numOfValidArgs[4], DWPAL_STR_PARAM, "vht_caps_info=", sizeof(vht_cap)},
        {(void *)vht_mcs, &numOfValidArgs[5], DWPAL_STR_PARAM, "rx_vht_mcs_map=", sizeof(vht_mcs)},
        {(void *)&max_tx_power, &numOfValidArgs[6], DWPAL_CHAR_PARAM, "max_txpower=", 0},
        /* Must be at the end */
        {NULL, NULL, DWPAL_NUM_OF_PARSING_TYPES, NULL, 0}};

    if (dwpal_string_to_struct_parse((char *)event.c_str(), event.length(), fieldsToParse,
                                     sizeof(client_mac)) == DWPAL_FAILURE) {
        LOG(ERROR) << "DWPAL parse error ==> Abort";
        return nullptr;
    }

    (void)supported_rates;
    (void)HT_MCS;
    (void)VHT_MCS;
    (void)ht_cap;
    (void)ht_mcs;
    (void)vht_cap;
    (void)vht_mcs;

    LOG(DEBUG) << "client_mac: " << client_mac;

    for (uint8_t i = 0; i < (sizeof(numOfValidArgs) / sizeof(size_t)); i++) {
        if (numOfValidArgs[i] == 0) {
            LOG(ERROR) << "Failed reading parsed parameter " << (int)i
                       << " ==> Continue with default values";
        }
    }

    msg->vap_id = vap_id;
    msg->mac    = tlvf::mac_from_string(client_mac);

    // return the buffer
    return msg_buff;
}

//////////////////////////////////////////////////////////////////////////////
/////////////////////////////// Implementation ///////////////////////////////
//////////////////////////////////////////////////////////////////////////////

mon_wlan_hal_dwpal::mon_wlan_hal_dwpal(const std::string &iface_name, hal_event_cb_t callback,
                                       const bwl::hal_conf_t &hal_conf)
    : base_wlan_hal(bwl::HALType::Monitor, iface_name, IfaceType::Intel, callback, hal_conf),
      base_wlan_hal_dwpal(bwl::HALType::Monitor, iface_name, callback, hal_conf)
{
}

mon_wlan_hal_dwpal::~mon_wlan_hal_dwpal() {}

bool mon_wlan_hal_dwpal::update_radio_stats(SRadioStats &radio_stats)
{
    char *reply = nullptr;

    if (!dwpal_send_cmd("GET_RADIO_INFO", &reply)) {
        LOG(ERROR) << " failed";
        return false;
    }

    size_t numOfValidArgs[7] = {0}, replyLen = strnlen(reply, HOSTAPD_TO_DWPAL_MSG_LENGTH);
    uint64_t BytesSent = 0, BytesReceived = 0, PacketsSent = 0, PacketsReceived = 0;
    FieldsToParse fieldsToParse[] = {
        {(void *)&BytesSent, &numOfValidArgs[0], DWPAL_LONG_LONG_INT_PARAM, "BytesSent=", 0},
        {(void *)&BytesReceived, &numOfValidArgs[1], DWPAL_LONG_LONG_INT_PARAM,
         "BytesReceived=", 0},
        {(void *)&PacketsSent, &numOfValidArgs[2], DWPAL_LONG_LONG_INT_PARAM, "PacketsSent=", 0},
        {(void *)&PacketsReceived, &numOfValidArgs[3], DWPAL_LONG_LONG_INT_PARAM,
         "PacketsReceived=", 0},
        {(void *)&radio_stats.errors_sent, &numOfValidArgs[4], DWPAL_INT_PARAM, "ErrorsSent=", 0},
        {(void *)&radio_stats.errors_received, &numOfValidArgs[5], DWPAL_INT_PARAM,
         "ErrorsReceived=", 0},
        {(void *)&radio_stats.noise, &numOfValidArgs[6], DWPAL_CHAR_PARAM, "Noise=", 0},
        /* Must be at the end */
        {NULL, NULL, DWPAL_NUM_OF_PARSING_TYPES, NULL, 0}};

    if (dwpal_string_to_struct_parse(reply, replyLen, fieldsToParse, sizeof(SRadioStats)) ==
        DWPAL_FAILURE) {
        LOG(ERROR) << "DWPAL parse error ==> Abort";
        return false;
    }

    /* TEMP: Traces... */
    // LOG(DEBUG) << "GET_RADIO_INFO reply= \n" << reply;
    // LOG(DEBUG) << "numOfValidArgs[0]= " << numOfValidArgs[0] << " BytesSent= " << BytesSent;
    // LOG(DEBUG) << "numOfValidArgs[1]= " << numOfValidArgs[1] << " BytesReceived= " << BytesReceived;
    // LOG(DEBUG) << "numOfValidArgs[2]= " << numOfValidArgs[2] << " PacketsSent= " << PacketsSent;
    // LOG(DEBUG) << "numOfValidArgs[3]= " << numOfValidArgs[3] << " PacketsReceived= " << PacketsReceived;
    // LOG(DEBUG) << "numOfValidArgs[4]= " << numOfValidArgs[4] << " ErrorsSent= " << radio_stats.errors_sent;
    // LOG(DEBUG) << "numOfValidArgs[5]= " << numOfValidArgs[5] << " ErrorsReceived= " << radio_stats.errors_received;
    // LOG(DEBUG) << "numOfValidArgs[6]= " << numOfValidArgs[6] << " Noise= " << (int)radio_stats.noise;
    /* End of TEMP: Traces... */

    for (uint8_t i = 0; i < (sizeof(numOfValidArgs) / sizeof(size_t)); i++) {
        if (numOfValidArgs[i] == 0) {
            LOG(ERROR) << "Failed reading parsed parameter " << (int)i << " ==> Abort";
            return false;
        }
    }

    calc_curr_traffic(BytesSent, radio_stats.tx_bytes_cnt, radio_stats.tx_bytes);
    calc_curr_traffic(BytesReceived, radio_stats.rx_bytes_cnt, radio_stats.rx_bytes);
    calc_curr_traffic(PacketsSent, radio_stats.tx_packets_cnt, radio_stats.tx_packets);
    calc_curr_traffic(PacketsReceived, radio_stats.rx_packets_cnt, radio_stats.rx_packets);

    return true;
}

bool mon_wlan_hal_dwpal::update_vap_stats(const std::string &vap_iface_name, SVapStats &vap_stats)
{
    char *reply = nullptr;

    std::string cmd = "GET_VAP_MEASUREMENTS " + vap_iface_name;

    if (!dwpal_send_cmd(cmd, &reply)) {
        LOG(ERROR) << __func__ << " failed";
        return false;
    }

    size_t numOfValidArgs[7] = {0}, replyLen = strnlen(reply, HOSTAPD_TO_DWPAL_MSG_LENGTH);
    uint64_t BytesSent = 0, BytesReceived = 0, PacketsSent = 0, PacketsReceived = 0;
    FieldsToParse fieldsToParse[] = {
        {(void *)&BytesSent, &numOfValidArgs[0], DWPAL_LONG_LONG_INT_PARAM, "BytesSent=", 0},
        {(void *)&BytesReceived, &numOfValidArgs[1], DWPAL_LONG_LONG_INT_PARAM,
         "BytesReceived=", 0},
        {(void *)&PacketsSent, &numOfValidArgs[2], DWPAL_LONG_LONG_INT_PARAM, "PacketsSent=", 0},
        {(void *)&PacketsReceived, &numOfValidArgs[3], DWPAL_LONG_LONG_INT_PARAM,
         "PacketsReceived=", 0},
        {(void *)&vap_stats.retrans_count, &numOfValidArgs[4], DWPAL_INT_PARAM, "RetransCount=", 0},
        {(void *)&vap_stats.errors_sent, &numOfValidArgs[5], DWPAL_INT_PARAM, "ErrorsSent=", 0},
        {(void *)&vap_stats.errors_received, &numOfValidArgs[6], DWPAL_INT_PARAM,
         "ErrorsReceived=", 0},
        /* Must be at the end */
        {NULL, NULL, DWPAL_NUM_OF_PARSING_TYPES, NULL, 0}};

    if (dwpal_string_to_struct_parse(reply, replyLen, fieldsToParse, sizeof(SVapStats)) ==
        DWPAL_FAILURE) {
        LOG(ERROR) << "DWPAL parse error ==> Abort";
        return false;
    }

    /* TEMP: Traces... */
    // LOG(DEBUG) << "GET_VAP_MEASUREMENTS reply= \n" << reply;
    // LOG(DEBUG) << "numOfValidArgs[0]= " << numOfValidArgs[0] << " BytesSent= " << BytesSent;
    // LOG(DEBUG) << "numOfValidArgs[1]= " << numOfValidArgs[1] << " BytesReceived= " << BytesReceived;
    // LOG(DEBUG) << "numOfValidArgs[2]= " << numOfValidArgs[2] << " PacketsSent= " << PacketsSent;
    // LOG(DEBUG) << "numOfValidArgs[3]= " << numOfValidArgs[3] << " PacketsReceived= " << PacketsReceived;
    // LOG(DEBUG) << "numOfValidArgs[4]= " << numOfValidArgs[4] << " RetransCount= " << vap_stats.retrans_count;
    // LOG(DEBUG) << "numOfValidArgs[5]= " << numOfValidArgs[5] << " ErrorsSent= " << vap_stats.errors_sent;
    // LOG(DEBUG) << "numOfValidArgs[6]= " << numOfValidArgs[6] << " ErrorsReceived= " << vap_stats.errors_received;
    /* End of TEMP: Traces... */

    for (uint8_t i = 0; i < (sizeof(numOfValidArgs) / sizeof(size_t)); i++) {
        if (numOfValidArgs[i] == 0) {
            LOG(ERROR) << "Failed reading parsed parameter " << (int)i << " ==> Abort";
            return false;
        }
    }

    calc_curr_traffic(BytesSent, vap_stats.tx_bytes_cnt, vap_stats.tx_bytes);
    calc_curr_traffic(BytesReceived, vap_stats.rx_bytes_cnt, vap_stats.rx_bytes);
    calc_curr_traffic(PacketsSent, vap_stats.tx_packets_cnt, vap_stats.tx_packets);
    calc_curr_traffic(PacketsReceived, vap_stats.rx_packets_cnt, vap_stats.rx_packets);

    // TODO: Handle timeouts/deltas externally!
    // auto now = std::chrono::steady_clock::now();
    // auto time_span = std::chrono::duration_cast<std::chrono::milliseconds>(now - vap_stats->last_update_time);
    // vap_stats->delta_ms = float(time_span.count());
    // vap_stats->last_update_time = now;

    return true;
}

bool mon_wlan_hal_dwpal::update_stations_stats(const std::string &vap_iface_name,
                                               const std::string &sta_mac, SStaStats &sta_stats)
{
    const char *tmp_str;
    int64_t tmp_int;
    parsed_line_t reply;

    std::string cmd = "GET_STA_MEASUREMENTS " + vap_iface_name + " " + sta_mac;

    if (!dwpal_send_cmd(cmd, reply)) {
        LOG(ERROR) << __func__ << " failed";
        return false;
    }

    // RSSI
    if (!read_param("ShortTermRSSIAverage", reply, &tmp_str)) {
        LOG(ERROR) << "Failed reading ShortTermRSSIAverage parameter!";
        return false;
    }
    // Format "ShortTermRSSIAverage=%d %d %d %d"
    auto samples = beerocks::string_utils::str_split(tmp_str, ' ');
    for (const auto &s : samples) {
        float s_float = float(beerocks::string_utils::stoi(s));
        if (s_float > beerocks::RSSI_MIN) {
            sta_stats.rx_rssi_watt += std::pow(10, s_float / float(10));
            sta_stats.rx_rssi_watt_samples_cnt++;
        }
    }

    // SNR
    if (!read_param("SNR", reply, &tmp_str)) {
        LOG(ERROR) << "Failed reading SNR parameter!";
        return false;
    }
    // Format "SNR=%d %d %d %d"
    samples = beerocks::string_utils::str_split(tmp_str, ' ');
    for (const auto &s : samples) {
        float s_float = float(beerocks::string_utils::stoi(s));
        if (s_float >= beerocks::SNR_MIN) {
            sta_stats.rx_snr_watt += std::pow(10, s_float / float(10));
            sta_stats.rx_snr_watt_samples_cnt++;
        }
    }

    // Last Downlink (TX) Rate
    if (!read_param("LastDataDownlinkRate", reply, tmp_int)) {
        LOG(ERROR) << "Failed reading LastDataDownlinkRate parameter!";
        return false;
    }
    sta_stats.tx_phy_rate_100kb = (tmp_int / 100);

    // Last Uplink (RX) Rate
    if (!read_param("LastDataUplinkRate", reply, tmp_int)) {
        LOG(ERROR) << "Failed reading LastDataUplinkRate parameter!";
        return false;
    }
    sta_stats.rx_phy_rate_100kb = (tmp_int / 100);

    // TX Bytes
    if (!read_param("BytesSent", reply, tmp_int)) {
        LOG(ERROR) << "Failed reading BytesSent parameter!";
        return false;
    }
    calc_curr_traffic(tmp_int, sta_stats.tx_bytes_cnt, sta_stats.tx_bytes);

    // RX Bytes
    if (!read_param("BytesReceived", reply, tmp_int)) {
        LOG(ERROR) << "Failed reading BytesReceived parameter!";
        return false;
    }
    calc_curr_traffic(tmp_int, sta_stats.rx_bytes_cnt, sta_stats.rx_bytes);

    // TX Packets
    if (!read_param("PacketsSent", reply, tmp_int)) {
        LOG(ERROR) << "Failed reading PacketsSent parameter!";
        return false;
    }
    calc_curr_traffic(tmp_int, sta_stats.rx_packets_cnt, sta_stats.rx_packets);

    // Retranmission Count
    if (!read_param("RetransCount", reply, tmp_int, true)) {
        LOG(ERROR) << "Failed reading RetransCount parameter!";
        return false;
    }
    sta_stats.retrans_count = tmp_int;

    return true;
}

bool mon_wlan_hal_dwpal::sta_channel_load_11k_request(const SStaChannelLoadRequest11k &req)
{
    LOG(TRACE) << __func__;

    return true;
}

bool mon_wlan_hal_dwpal::sta_beacon_11k_request(const SBeaconRequest11k &req, int &dialog_token)
{
    LOG(TRACE) << __func__;
    char *reply = nullptr;

    // parameters preperations

    // Mode
    auto request = (!req.enable) ? 0 : req.request;
    auto report  = (!req.enable) ? 0 : req.report;

    uint8_t req_mode = (req.parallel | (req.enable ? 0x02 : 0) | (request ? 0x04 : 0) |
                        (report ? 0x08 : 0) | (req.mandatory_duration ? 0x10 : 0));

    auto op_class = req.op_class < 0 ? GET_OP_CLASS(get_radio_info().channel) : req.op_class;

    std::string measurement_mode;
    switch ((SBeaconRequest11k::MeasurementMode)(req.measurement_mode)) {
    case SBeaconRequest11k::MeasurementMode::Passive:
        measurement_mode = "passive";
        break;
    case SBeaconRequest11k::MeasurementMode::Active:
        measurement_mode = "active";
        break;
    case SBeaconRequest11k::MeasurementMode::Table:
        measurement_mode = "table";
        break;
    default:
        LOG(WARNING) << "Invalid measuremetn mode: " << int(req.measurement_mode)
                     << ", using PASSIVE...";
        measurement_mode = "passive";
    }

    // build command
    std::string cmd = "REQ_BEACON " + tlvf::mac_to_string(req.sta_mac.oct) +
                      " " +                                 // Destination MAC Address
                      std::to_string(req.repeats) + " " +   // Number of repitions
                      std::to_string(req_mode) + " " +      // Measurements Request Mode
                      std::to_string(op_class) + " " +      // Operating Class
                      std::to_string(req.channel) + " " +   // Channel
                      std::to_string(req.rand_ival) + " " + // Random Interval
                      std::to_string(req.duration) + " " +  // Duration
                      measurement_mode + " " +              // Measurement Mode
                      tlvf::mac_to_string(req.bssid.oct);   // Target BSSID

    /////////////////////////////////////////////////
    //////////////// Optional Fields ////////////////
    /////////////////////////////////////////////////

    // SSID
    if (req.use_optional_ssid) {
        std::string req_ssid = '"' + std::string((char *)req.ssid) + '"';
        cmd += " ssid=" + req_ssid;
    }

    // use ap_ch_report in the request.
    if (0 != req.use_optional_ap_ch_report) {

        if (255 != req.channel) {
            LOG(ERROR) << "ap-channel-report was set although channel was not set to 255";
            return false;
        }
        std::stringstream ch_report_ss;
        uint8_t current_channel = 0;
        for (; current_channel < req.use_optional_ap_ch_report - 1; ++current_channel) {
            ch_report_ss << +req.ap_ch_report[current_channel] << ",";
        }
        ch_report_ss << +req.ap_ch_report[current_channel];

        cmd += " ap_ch_report=" + ch_report_ss.str();
    } else {
        if (255 == req.channel) {
            LOG(ERROR) << "ap-channel-report was not set although channel was set to 255";
        }
    }

    // report detail: rep_detail=
    // this value should be set by default to 2 if not stated otherwise.
    // however hostap does not set it to 2. So we add it hardcoded here
    // note: currently there is no field in SBeaconRequest11k that holds this optional field
    cmd += " rep_detail=2";

    LOG(DEBUG) << " the command: " << cmd;

    // send command
    if (!dwpal_send_cmd(cmd, &reply)) {
        LOG(ERROR) << __func__ << " failed";
        return false;
    }

    size_t numOfValidArgs[1] = {0}, replyLen = strnlen(reply, HOSTAPD_TO_DWPAL_MSG_LENGTH);
    FieldsToParse fieldsToParse[] = {
        {(void *)&dialog_token, &numOfValidArgs[0], DWPAL_INT_PARAM, "dialog_token=", 0},
        /* Must be at the end */
        {NULL, NULL, DWPAL_NUM_OF_PARSING_TYPES, NULL, 0}};

    if (dwpal_string_to_struct_parse(reply, replyLen, fieldsToParse, sizeof(dialog_token)) ==
        DWPAL_FAILURE) {
        LOG(ERROR) << "DWPAL parse error ==> Abort";
        return false;
    }

    /* TEMP: Traces... */
    LOG(DEBUG) << "REQ_BEACON reply= \n" << reply;
    LOG(DEBUG) << "numOfValidArgs[0]= " << numOfValidArgs[0] << " dialog_token= " << dialog_token;
    /* End of TEMP: Traces... */

    for (uint8_t i = 0; i < (sizeof(numOfValidArgs) / sizeof(size_t)); i++) {
        if (numOfValidArgs[i] == 0) {
            LOG(ERROR) << "Failed reading parsed parameter " << (int)i << " ==> Abort";
            return false;
        }
    }

    return true;
}

bool mon_wlan_hal_dwpal::sta_link_measurements_11k_request(const std::string &sta_mac)
{
    LOG(TRACE) << __func__;
    return true;
}

bool mon_wlan_hal_dwpal::channel_scan_trigger(int dwell_time_msec,
                                              const std::vector<unsigned int> &channel_pool)
{
    LOG(DEBUG) << "Channel scan trigger received on interface=" << m_radio_info.iface_name;

    //build scan parameters
    ScanParams channel_scan_params = {0};
    sScanCfgParams org_fg, new_fg;   //foreground scan param
    sScanCfgParamsBG org_bg, new_bg; //background scan param

    // get original scan params
    if (!dwpal_get_scan_params_fg(org_fg) || !dwpal_get_scan_params_bg(org_bg)) {
        LOG(ERROR) << "Failed getting original scan parameters";
        return false;
    }

    // prepare new scan params with changed dwell time
    new_fg                    = org_fg;
    new_bg                    = org_bg;
    new_fg.passive_dwell_time = dwell_time_msec;
    new_fg.active_dwell_time  = dwell_time_msec;
    new_bg.passive_dwell_time = dwell_time_msec;
    new_bg.active_dwell_time  = dwell_time_msec;

    // set new scan params & get newly set values for validation
    if (!dwpal_set_scan_params_fg(new_fg) || !dwpal_set_scan_params_bg(new_bg)) {
        LOG(ERROR) << "Failed setting new values, restoring original scan parameters";
        dwpal_set_scan_params_fg(org_fg);
        dwpal_set_scan_params_bg(org_bg);
        return false;
    }

    if (!dwpal_get_scan_params_fg(new_fg) || !dwpal_get_scan_params_bg(new_bg) ||
        (new_fg.active_dwell_time != dwell_time_msec) ||
        (new_fg.passive_dwell_time != dwell_time_msec) ||
        (new_bg.active_dwell_time != dwell_time_msec) ||
        (new_bg.passive_dwell_time != dwell_time_msec)) {
        LOG(ERROR) << "Validation failed, restoring original scan parameters";
        dwpal_set_scan_params_fg(org_fg);
        dwpal_set_scan_params_bg(org_bg);
        return false;
    }

    // get frequencies from channel pool and set in scan_params
    if (!dwpal_get_channel_scan_freq(channel_pool, m_radio_info.channel, m_radio_info.iface_name,
                                     channel_scan_params)) {
        LOG(ERROR) << "Failed getting frequencies, restoring original scan parameters";
        dwpal_set_scan_params_fg(org_fg);
        dwpal_set_scan_params_bg(org_bg);
        return false;
    }

    // must as single wifi won't allow scan on ap without this flag
    channel_scan_params.ap_force = 1;

    if (dwpal_driver_nl_scan_trigger(get_dwpal_nl_ctx(), (char *)m_radio_info.iface_name.c_str(),
                                     &channel_scan_params) != DWPAL_SUCCESS) {
        LOG(ERROR) << " scan trigger failed! Abort scan, restoring original scan parameters";
        dwpal_set_scan_params_fg(org_fg);
        dwpal_set_scan_params_bg(org_bg);
        return false;
    }

    // restoring channel scan params with original dwell time.
    // dwpal_driver_nl_scan_trigger() API doesn't include dwell time parameter
    // so we have to change and restore driver scan parameters on the fly.
    // no reason to check since we restore the original params here anyway
    // and the next validation will validate the change.
    dwpal_set_scan_params_fg(org_fg);
    dwpal_set_scan_params_bg(org_bg);

    // validate if "set" function to original values worked
    if (!dwpal_get_scan_params_fg(new_fg) || !dwpal_get_scan_params_bg(new_bg) ||
        (new_fg.active_dwell_time != org_fg.active_dwell_time) ||
        (new_fg.passive_dwell_time != org_fg.passive_dwell_time) ||
        (new_bg.active_dwell_time != org_bg.active_dwell_time) ||
        (new_bg.passive_dwell_time != org_bg.passive_dwell_time)) {
        LOG(ERROR) << "Validation failed, original scan parameters were not restored";
        return false;
    }

    return true;
}

bool mon_wlan_hal_dwpal::channel_scan_dump_results()
{
    if (!dwpal_nl_cmd_scan_dump()) {
        LOG(ERROR) << "Channel scan results dump failed";
        return false;
    }

    return true;
}

bool mon_wlan_hal_dwpal::generate_connected_clients_events()
{
    bool queried_first = false;
    std::string cmd;
    std::string client_mac;
    bool ret = true;

    for (const auto &vap_element : m_radio_info.available_vaps) {
        char *reply;
        size_t replyLen;

        const int &vap_id = vap_element.first;
        auto vap_iface_name =
            beerocks::utils::get_iface_string_from_iface_vap_ids(get_iface_name(), vap_id);
        LOG(TRACE) << __func__ << " for vap interface: " << vap_iface_name;

        do {
            if (queried_first) {
                cmd = "STA-NEXT " + vap_iface_name + " " + client_mac;
            } else {
                cmd           = "STA-FIRST " + vap_iface_name;
                queried_first = true;
            }

            reply = nullptr;

            // Send command
            if (!dwpal_send_cmd(cmd, &reply)) {
                LOG(ERROR) << __func__ << ": cmd='" << cmd << "' failed!";
                ret = false;
                break;
            }

            replyLen = strnlen(reply, HOSTAPD_TO_DWPAL_MSG_LENGTH);

            if (replyLen == 0) {
                LOG(DEBUG) << "cmd:" << cmd << ", reply: EMPTY";
                break;
            } else {
                LOG(DEBUG) << "cmd: " << cmd << ", replylen: " << (int)replyLen
                           << ", reply: " << reply;
            }

            auto msg_buff = generate_client_assoc_event(reply, vap_id);

            if (!msg_buff)
                break;

            // update client mac
            auto msg =
                reinterpret_cast<sACTION_MONITOR_CLIENT_ASSOCIATED_NOTIFICATION *>(msg_buff.get());
            client_mac = tlvf::mac_to_string(msg->mac);

            event_queue_push(Event::STA_Connected, msg_buff); // send message to the Monitor

        } while (replyLen > 0);

        if (!ret)
            return false;

        queried_first = false;
    }

    return ret;
}

bool mon_wlan_hal_dwpal::process_dwpal_event(char *buffer, int bufLen, const std::string &opcode)
{
    LOG(TRACE) << __func__ << " - opcode: |" << opcode << "|";

    auto event = dwpal_to_bwl_event(opcode);

    // Handle the event
    switch (event) {
    case Event::RRM_Beacon_Response: {
        LOG(DEBUG) << "RRM-BEACON-REP-RECEIVED buffer= \n" << buffer;
        // Allocate response object
        auto resp_buff = ALLOC_SMART_BUFFER(sizeof(SBeaconResponse11k));
        auto resp      = reinterpret_cast<SBeaconResponse11k *>(resp_buff.get());

        if (!resp) {
            LOG(FATAL) << "Memory allocation failed!";
            return false;
        }

        // Initialize the message
        memset(resp_buff.get(), 0, sizeof(SBeaconResponse11k));

        size_t numOfValidArgs[11]      = {0};
        char MACAddress[MAC_ADDR_SIZE] = {0}, bssid[MAC_ADDR_SIZE] = {0};
        FieldsToParse fieldsToParse[] = {
            {NULL /*opCode*/, &numOfValidArgs[0], DWPAL_STR_PARAM, NULL, 0},
            {NULL, &numOfValidArgs[1], DWPAL_STR_PARAM, NULL, 0},
            {(void *)MACAddress, &numOfValidArgs[2], DWPAL_STR_PARAM, NULL, sizeof(MACAddress)},
            {(void *)&resp->channel, &numOfValidArgs[3], DWPAL_CHAR_PARAM, "channel=", 0},
            {(void *)&resp->dialog_token, &numOfValidArgs[4], DWPAL_CHAR_PARAM, "dialog_token=", 0},
            {(void *)&resp->rep_mode, &numOfValidArgs[5], DWPAL_CHAR_PARAM,
             "measurement_rep_mode=", 0},
            {(void *)&resp->op_class, &numOfValidArgs[6], DWPAL_CHAR_PARAM, "op_class=", 0},
            {(void *)&resp->duration, &numOfValidArgs[7], DWPAL_SHORT_INT_PARAM, "duration=", 0},
            {(void *)&resp->rcpi, &numOfValidArgs[8], DWPAL_CHAR_PARAM, "rcpi=", 0},
            {(void *)&resp->rsni, &numOfValidArgs[9], DWPAL_CHAR_PARAM, "rsni=", 0},
            {(void *)bssid, &numOfValidArgs[10], DWPAL_STR_PARAM, "bssid=", sizeof(bssid)},
            /* Must be at the end */
            {NULL, NULL, DWPAL_NUM_OF_PARSING_TYPES, NULL, 0}};

        if (dwpal_string_to_struct_parse(buffer, bufLen, fieldsToParse,
                                         sizeof(SBeaconResponse11k)) == DWPAL_FAILURE) {
            LOG(ERROR) << "DWPAL parse error ==> Abort";
            return false;
        }

        /* TEMP: Traces... */
        LOG(DEBUG) << "numOfValidArgs[2]= " << numOfValidArgs[2] << " MACAddress= " << MACAddress;
        LOG(DEBUG) << "numOfValidArgs[3]= " << numOfValidArgs[3]
                   << " channel= " << (int)resp->channel;
        LOG(DEBUG) << "numOfValidArgs[4]= " << numOfValidArgs[4]
                   << " Retransmissions= " << (int)resp->dialog_token;
        LOG(DEBUG) << "numOfValidArgs[5]= " << numOfValidArgs[5]
                   << " measurement_rep_mode= " << (int)resp->rep_mode;
        LOG(DEBUG) << "numOfValidArgs[6]= " << numOfValidArgs[6]
                   << " op_class= " << (int)resp->op_class;
        LOG(DEBUG) << "numOfValidArgs[7]= " << numOfValidArgs[7]
                   << " duration= " << (int)resp->duration;
        LOG(DEBUG) << "numOfValidArgs[8]= " << numOfValidArgs[8] << " rcpi= " << (int)resp->rcpi;
        LOG(DEBUG) << "numOfValidArgs[9]= " << numOfValidArgs[9] << " rsni= " << (int)resp->rsni;
        LOG(DEBUG) << "numOfValidArgs[10]= " << numOfValidArgs[10] << " bssid= " << bssid;
        /* End of TEMP: Traces... */

        for (uint8_t i = 0; i < (sizeof(numOfValidArgs) / sizeof(size_t)); i++) {
            if (numOfValidArgs[i] == 0) {
                LOG(ERROR) << "Failed reading parsed parameter " << (int)i << " ==> Abort";
                return false;
            }
        }

        tlvf::mac_from_string(resp->sta_mac.oct, MACAddress);
        tlvf::mac_from_string(resp->bssid.oct, bssid);

        // Add the message to the queue
        event_queue_push(event, resp_buff);
        break;
    }

    case Event::AP_Enabled: {
        auto msg_buff = ALLOC_SMART_BUFFER(sizeof(sHOSTAP_ENABLED_NOTIFICATION));
        if (!msg_buff) {
            LOG(FATAL) << "Memory allocation failed!";
            return false;
        }
        auto msg = reinterpret_cast<sHOSTAP_ENABLED_NOTIFICATION *>(msg_buff.get());
        if (!msg) {
            LOG(FATAL) << "Memory allocation failed!";
            return false;
        }

        memset(msg_buff.get(), 0, sizeof(sHOSTAP_ENABLED_NOTIFICATION));
        LOG(DEBUG) << "AP_ENABLED buffer= \n" << buffer;

        char interface[SSID_MAX_SIZE] = {0};
        size_t numOfValidArgs[2]      = {0};
        FieldsToParse fieldsToParse[] = {
            {NULL /*opCode*/, &numOfValidArgs[0], DWPAL_STR_PARAM, NULL, 0},
            {(void *)interface, &numOfValidArgs[1], DWPAL_STR_PARAM, NULL, sizeof(interface)},

            /* Must be at the end */
            {NULL, NULL, DWPAL_NUM_OF_PARSING_TYPES, NULL, 0}};

        if (dwpal_string_to_struct_parse(buffer, bufLen, fieldsToParse, sizeof(interface)) ==
            DWPAL_FAILURE) {
            LOG(ERROR) << "DWPAL parse error ==> Abort";
            return false;
        }

        auto iface_ids = beerocks::utils::get_ids_from_iface_string(interface);
        msg->vap_id    = iface_ids.vap_id;

        if (iface_ids.vap_id == beerocks::IFACE_RADIO_ID) {
            // Ignore AP-ENABLED on radio
            return true;
        }

        event_queue_push(event, msg_buff);
        break;
    }

    case Event::AP_Disabled: {
        auto msg_buff = ALLOC_SMART_BUFFER(sizeof(sHOSTAP_DISABLED_NOTIFICATION));
        if (!msg_buff) {
            LOG(FATAL) << "Memory allocation failed!";
            return false;
        }

        auto msg = reinterpret_cast<sHOSTAP_DISABLED_NOTIFICATION *>(msg_buff.get());
        if (!msg) {
            LOG(FATAL) << "Memory allocation failed!";
            return false;
        }

        memset(msg_buff.get(), 0, sizeof(sHOSTAP_DISABLED_NOTIFICATION));
        LOG(INFO) << "AP_Disabled buffer= \n" << buffer;

        char interface[SSID_MAX_SIZE] = {0};
        size_t numOfValidArgs[2]      = {0};
        FieldsToParse fieldsToParse[] = {
            {NULL /*opCode*/, &numOfValidArgs[0], DWPAL_STR_PARAM, NULL, 0},
            {(void *)interface, &numOfValidArgs[1], DWPAL_STR_PARAM, NULL, sizeof(interface)},

            /* Must be at the end */
            {NULL, NULL, DWPAL_NUM_OF_PARSING_TYPES, NULL, 0}};

        if (dwpal_string_to_struct_parse(buffer, bufLen, fieldsToParse, sizeof(interface)) ==
            DWPAL_FAILURE) {
            LOG(ERROR) << "DWPAL parse error ==> Abort";
            return false;
        }

        auto iface_ids = beerocks::utils::get_ids_from_iface_string(interface);
        msg->vap_id    = iface_ids.vap_id;

        event_queue_push(event, msg_buff);
        break;
    }

    case Event::STA_Connected: {

        // TODO: Change to HAL objects
        auto msg_buff = ALLOC_SMART_BUFFER(sizeof(sACTION_MONITOR_CLIENT_ASSOCIATED_NOTIFICATION));
        auto msg =
            reinterpret_cast<sACTION_MONITOR_CLIENT_ASSOCIATED_NOTIFICATION *>(msg_buff.get());
        LOG_IF(!msg, FATAL) << "Memory allocation failed!";

        // Initialize the message
        memset(msg_buff.get(), 0, sizeof(sACTION_MONITOR_CLIENT_ASSOCIATED_NOTIFICATION));

        char VAP[SSID_MAX_SIZE]        = {0};
        char MACAddress[MAC_ADDR_SIZE] = {0};
        size_t numOfValidArgs[3]       = {0};

        FieldsToParse fieldsToParse[] = {
            {NULL /*opCode*/, &numOfValidArgs[0], DWPAL_STR_PARAM, NULL, 0},
            {(void *)VAP, &numOfValidArgs[1], DWPAL_STR_PARAM, NULL, sizeof(VAP)},
            {(void *)MACAddress, &numOfValidArgs[2], DWPAL_STR_PARAM, NULL, sizeof(MACAddress)},
            /* Must be at the end */
            {NULL, NULL, DWPAL_NUM_OF_PARSING_TYPES, NULL, 0}};

        if (dwpal_string_to_struct_parse(buffer, bufLen, fieldsToParse, sizeof(VAP)) ==
            DWPAL_FAILURE) {
            LOG(ERROR) << "DWPAL parse error ==> Abort";
            return false;
        }

        LOG(DEBUG) << "vap_id           : " << VAP;
        LOG(DEBUG) << "MACAddress       : " << MACAddress;

        for (uint8_t i = 0; i < (sizeof(numOfValidArgs) / sizeof(size_t)); i++) {
            if (numOfValidArgs[i] == 0) {
                LOG(ERROR) << "Failed reading parsed parameter " << static_cast<int>(i);
                return false;
            }
        }

        msg->vap_id = beerocks::utils::get_ids_from_iface_string(VAP).vap_id;
        msg->mac    = tlvf::mac_from_string(MACAddress);

        event_queue_push(Event::STA_Connected, msg_buff); // send message to the AP manager

        break;
    }

    case Event::STA_Disconnected: {
        // TODO: Change to HAL objects
        auto msg_buff =
            ALLOC_SMART_BUFFER(sizeof(sACTION_MONITOR_CLIENT_DISCONNECTED_NOTIFICATION));
        auto msg =
            reinterpret_cast<sACTION_MONITOR_CLIENT_DISCONNECTED_NOTIFICATION *>(msg_buff.get());
        LOG_IF(!msg, FATAL) << "Memory allocation failed!";

        // Initialize the message
        memset(msg_buff.get(), 0, sizeof(sACTION_MONITOR_CLIENT_DISCONNECTED_NOTIFICATION));

        char VAP[SSID_MAX_SIZE]        = {0};
        char MACAddress[MAC_ADDR_SIZE] = {0};
        size_t numOfValidArgs[3]       = {0};
        FieldsToParse fieldsToParse[]  = {
            {NULL /*opCode*/, &numOfValidArgs[0], DWPAL_STR_PARAM, NULL, 0},
            {(void *)VAP, &numOfValidArgs[1], DWPAL_STR_PARAM, NULL, sizeof(VAP)},
            {(void *)MACAddress, &numOfValidArgs[2], DWPAL_STR_PARAM, NULL, sizeof(MACAddress)},
            /* Must be at the end */
            {NULL, NULL, DWPAL_NUM_OF_PARSING_TYPES, NULL, 0}};

        if (dwpal_string_to_struct_parse(buffer, bufLen, fieldsToParse, sizeof(VAP)) ==
            DWPAL_FAILURE) {
            LOG(ERROR) << "DWPAL parse error ==> Abort";
            return false;
        }

        LOG(DEBUG) << "vap_id     : " << VAP;
        LOG(DEBUG) << "MACAddress : " << MACAddress;

        for (uint8_t i = 0; i < (sizeof(numOfValidArgs) / sizeof(size_t)); i++) {
            if (numOfValidArgs[i] == 0) {
                LOG(ERROR) << "Failed reading parsed parameter " << (int)i << " ==> Abort";
                return false;
            }
        }

        msg->mac = tlvf::mac_from_string(MACAddress);

        event_queue_push(Event::STA_Disconnected, msg_buff); // send message to the AP manager
        break;
    }

    case Event::RRM_STA_Statistics_Response:
    case Event::RRM_Channel_Load_Response:
        break;
    // Gracefully ignore unhandled events
    // TODO: Probably should be changed to an error once dwpal will stop
    //       sending empty or irrelevant events...
    default:
        LOG(WARNING) << "Unhandled event received: " << opcode;
        break;
    }

    return true;
}

bool mon_wlan_hal_dwpal::process_dwpal_nl_event(struct nl_msg *msg)
{
    struct nlmsghdr *nlh    = nlmsg_hdr(msg);
    struct genlmsghdr *gnlh = (genlmsghdr *)nlmsg_data(nlh);
    std::string iface_name;

    struct nlattr *tb[NL80211_ATTR_MAX + 1];
    nla_parse(tb, NL80211_ATTR_MAX, genlmsg_attrdata(gnlh, 0), genlmsg_attrlen(gnlh, 0), NULL);

    if (tb[NL80211_ATTR_IFINDEX]) {
        auto index = nla_get_u32(tb[NL80211_ATTR_IFINDEX]);
        iface_name = beerocks::net::network_utils::linux_get_iface_name(index);
    }

    auto event = dwpal_nl_to_bwl_event(gnlh->cmd);

    switch (event) {
    case Event::Channel_Scan_Triggered: {
        if (m_radio_info.iface_name != iface_name) {
            // ifname doesn't match current interface
            // meaning the event was recevied for a diffrent channel
            return true;
        }
        LOG(DEBUG) << "DWPAL NL event channel scan triggered";

        //start new sequence of dump results
        m_nl_seq = 0;
        event_queue_push(event);
        break;
    }
    case Event::Channel_Scan_Dump_Result: {
        if (m_radio_info.iface_name != iface_name) {
            // ifname doesn't match current interface
            // meaning the event was received for a diffrent channel
            return true;
        }

        /*
            As part of the scan results dump sequence, we always receive at least two messages.
            NL80211_CMD_NEW_SCAN_RESULTS (Channel_Scan_Dump_Result) & SCAN_FINISH_CB
            (Channel_Scan_Finished).

            We receive the Channel_Scan_Dump_Result once to alert us that we have pending results
            waiting, so on the first occurrence of the message, we request the rest of the dump results.

            On the 2nd->Nth Channel_Scan_Dump_Result we receive a sequence number which we can use
            to indicate the current message is part of the active dump sequence.

            The last message received as part of the dump sequence is Channel_Scan_Finished, which
            indicates that there are no more pending scan dumps.

            In case we have no neighboring beacons for a particular scan we would only receive
            the initial Channel_Scan_Dump_Result followed right after by a Channel_Scan_Finished.
            This can cause an issue since we would not have an initial sequence number to later
            validate against.

            To solve this issue we add a "scan dump in progress flag" to verify against later on.
        */
        if (m_nl_seq == 0) {
            if (nlh->nlmsg_seq == 0) {
                // First "empty" Channel_Scan_Dump_Result message
                LOG(DEBUG) << "Results dump are ready";
                event_queue_push(Event::Channel_Scan_New_Results_Ready);
                m_scan_dump_in_progress = true;
                channel_scan_dump_results();
                return true;
            } else {
                //2nd -> Nth Channel_Scan_Dump_Result
                LOG(DEBUG) << "Results dump new sequence:" << int(nlh->nlmsg_seq);
                m_nl_seq = nlh->nlmsg_seq;
            }
        }

        // Check if current Channel_Scan_Dump_Result is part of the dump sequence.
        if (m_nl_seq != nlh->nlmsg_seq) {
            LOG(ERROR) << "channel scan results dump received with unexpected seq number";
            return false;
        }

        LOG(DEBUG) << "DWPAL NL event channel scan results dump, seq = " << int(nlh->nlmsg_seq);

        auto results = std::make_shared<sCHANNEL_SCAN_RESULTS_NOTIFICATION>();

        if (!get_scan_results_from_nl_msg(results->channel_scan_results, msg)) {
            LOG(ERROR) << "read NL msg to monitor msg failed!";
            return false;
        }

        LOG(DEBUG) << "Processing results for BSSID:" << results->channel_scan_results.bssid;
        event_queue_push(event, results);
        break;
    }
    case Event::Channel_Scan_Abort: {

        if (m_radio_info.iface_name != iface_name) {
            // ifname doesn't match current interface
            // meaning the event was recevied for a diffrent channel
            return true;
        }
        LOG(DEBUG) << "DWPAL NL event channel scan aborted";

        //reset scan indicators for next scan
        m_nl_seq                = 0;
        m_scan_dump_in_progress = false;
        event_queue_push(event);
        break;
    }
    case Event::Channel_Scan_Finished: {

        // We are not in a dump sequence, ignoring the message
        if (!m_scan_dump_in_progress) {
            return true;
        }

        // ifname is invalid  for Channel_Scan_Finished event using nlh->nlmsg_seq instead.
        // In case there are no results first check if current sequence number was set.
        if (m_nl_seq != 0 && nlh->nlmsg_seq != m_nl_seq) {
            // Current event has a sequence number not matching the current sequence number
            // meaning the event was recevied for a diffrent channel
            return true;
        }

        LOG(DEBUG) << "DWPAL NL event channel scan results finished for sequence: "
                   << (int)nlh->nlmsg_seq;

        //reset scan indicators for next scan
        m_nl_seq                = 0;
        m_scan_dump_in_progress = false;
        event_queue_push(event);
        break;
    }
    // Gracefully ignore unhandled events
    default:
        LOG(ERROR) << "Unknown DWPAL NL event received: " << int(event);
        break;
    }
    return true;
}

} // namespace dwpal

std::shared_ptr<mon_wlan_hal> mon_wlan_hal_create(const std::string &iface_name,
                                                  base_wlan_hal::hal_event_cb_t callback,
                                                  const bwl::hal_conf_t &hal_conf)
{
    return std::make_shared<dwpal::mon_wlan_hal_dwpal>(iface_name, callback, hal_conf);
}

} // namespace bwl
