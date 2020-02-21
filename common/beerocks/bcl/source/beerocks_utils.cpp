/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include <bcl/beerocks_string_utils.h>
#include <bcl/beerocks_utils.h>
#include <easylogging++.h>
#include <iomanip>

using namespace beerocks;

int utils::write_to_file(std::string full_path, std::string val)
{
    std::ofstream out_file(full_path);
    if (out_file.is_open()) {
        out_file << val;
        out_file.close();
        return 0;
    } else {
        return 1;
    }
}

beerocks::eIfaceType utils::get_iface_type_from_string(std::string iface_type_name)
{
    if (!iface_type_name.compare(0, sizeof(IFACE_TYPE_STR_WIFI_INTEL), IFACE_TYPE_STR_WIFI_INTEL))
        return beerocks::IFACE_TYPE_WIFI_INTEL;
    if (!iface_type_name.compare(0, sizeof(IFACE_TYPE_STR_WIFI_UNSPECIFIED),
                                 IFACE_TYPE_STR_WIFI_UNSPECIFIED))
        return beerocks::IFACE_TYPE_WIFI_UNSPECIFIED;
    if (!iface_type_name.compare(0, sizeof(IFACE_TYPE_STR_ETHERNET), IFACE_TYPE_STR_ETHERNET))
        return beerocks::IFACE_TYPE_ETHERNET;
    if (!iface_type_name.compare(0, sizeof(IFACE_TYPE_STR_GW_BRIDGE), IFACE_TYPE_STR_GW_BRIDGE))
        return beerocks::IFACE_TYPE_GW_BRIDGE;
    if (!iface_type_name.compare(0, sizeof(IFACE_TYPE_STR_BRIDGE), IFACE_TYPE_STR_BRIDGE))
        return beerocks::IFACE_TYPE_BRIDGE;
    return beerocks::IFACE_TYPE_UNSUPPORTED;
}

std::string utils::get_iface_type_string(beerocks::eIfaceType iface_type)
{
    std::string result;
    switch (iface_type) {
    case beerocks::IFACE_TYPE_WIFI_INTEL: {
        result = std::string(IFACE_TYPE_STR_WIFI_INTEL);
        break;
    }
    case beerocks::IFACE_TYPE_WIFI_UNSPECIFIED: {
        result = std::string(IFACE_TYPE_STR_WIFI_UNSPECIFIED);
        break;
    }
    case beerocks::IFACE_TYPE_ETHERNET: {
        result = std::string(IFACE_TYPE_STR_ETHERNET);
        break;
    }
    case beerocks::IFACE_TYPE_BRIDGE: {
        result = std::string(IFACE_TYPE_STR_BRIDGE);
        break;
    }
    case beerocks::IFACE_TYPE_GW_BRIDGE: {
        result = std::string(IFACE_TYPE_STR_GW_BRIDGE);
        break;
    }
    default: {
        result = std::string(IFACE_TYPE_STR_UNSUPPORTED);
    }
    }
    return result;
}

bool utils::is_node_wireless(beerocks::eIfaceType iface_type)
{
    return ((iface_type > beerocks::IFACE_TYPE_UNSUPPORTED) &&
            (iface_type < beerocks::IFACE_TYPE_WIFI_END));
}

utils::sIfaceVapIds utils::get_ids_from_iface_string(const std::string &iface)
{
    utils::sIfaceVapIds ids;
    if (iface.empty()) {
        LOG(ERROR) << "iface_is empty!";
        return ids;
    }

//    std::string::size_type pos = iface.find("wlan");
//    if (pos == std::string::npos) {
//        LOG(ERROR) << "iface does not contain the string 'wlan', function input string: " << iface;
//        return ids;
//    }
//
//    auto iface_num_str = iface.substr(pos + sizeof("wlan") - 1); // "x" or x.x"
//    auto iface_num_vec = string_utils::str_split(iface_num_str, '.');
//    if (iface_num_vec.size() == 0) {
//        LOG(ERROR) << "Invalid interface name " << iface;
//        return ids;
//    }
//
//    ids.iface_id = string_utils::stoi(iface_num_vec[0]);
//    ids.vap_id   = beerocks::IFACE_RADIO_ID;
//    if (iface_num_vec.size() == 2) {
//        int8_t vap_id = string_utils::stoi(iface_num_vec[1]);
//        if ((vap_id < beerocks::IFACE_VAP_ID_MIN) || (vap_id > beerocks::IFACE_VAP_ID_MAX)) {
//            LOG(DEBUG) << "Invalid VAP id " << vap_id << " for interface " << iface;
//            return ids;
//        }
//        ids.vap_id = vap_id;
//    }

        std::string::size_type pos = iface.find("ra");
        if (pos == std::string::npos) {
            LOG(ERROR) << "iface does not contain the string 'ra', function input string: " << iface;
            return ids;
        }

        if (!iface.compare("ra0")){
            ids.iface_id = 0;
            ids.vap_id   = beerocks::IFACE_RADIO_ID;
            return ids;
        }
        else if (!iface.compare("rai0")){
            ids.iface_id = 1;
            ids.vap_id   = beerocks::IFACE_RADIO_ID;
            return ids;
        }

    return ids;
}

std::string utils::get_iface_string_from_iface_vap_ids(int8_t iface_id, int8_t vap_id)
{
    std::string ifname;

    if ((vap_id < beerocks::IFACE_VAP_ID_MIN) || (vap_id > beerocks::IFACE_VAP_ID_MAX)) {
        LOG(ERROR) << "function input is not valid! iface_id=" << int(iface_id)
                   << ", vap_id=" << int(vap_id);
    } else {
        ifname = "wlan" + std::to_string(iface_id) + "." + std::to_string(vap_id);
    }

    return ifname;
}

std::string utils::get_iface_string_from_iface_vap_ids(const std::string &iface, int8_t vap_id)
{
    if (vap_id == beerocks::IFACE_RADIO_ID) {
        return iface;
    }

    std::string ifname;
    if ((iface.find("wlan") == std::string::npos) || (vap_id < beerocks::IFACE_VAP_ID_MIN) ||
        (vap_id > beerocks::IFACE_VAP_ID_MAX)) {
        LOG(ERROR) << "function input is not valid! iface=" << iface << ", vap_id=" << int(vap_id);
    } else {
        ifname = iface + "." + std::to_string(vap_id);
    }

    return ifname;
}

beerocks::eWiFiBandwidth utils::convert_bandwidth_to_enum(int bandwidth_int)
{
    beerocks::eWiFiBandwidth bw;
    switch (bandwidth_int) {
    case 20:
        bw = beerocks::BANDWIDTH_20;
        break;
    case 40:
        bw = beerocks::BANDWIDTH_40;
        break;
    case 80:
        bw = beerocks::BANDWIDTH_80;
        break;
    case 160:
        bw = beerocks::BANDWIDTH_160;
        break;
    default:
        bw = beerocks::BANDWIDTH_80;
        break;
    }
    return bw;
}

int utils::convert_bandwidth_to_int(beerocks::eWiFiBandwidth bandwidth)
{
    int bandwidth_int;
    switch (bandwidth) {
    case beerocks::BANDWIDTH_20:
        bandwidth_int = 20;
        break;
    case beerocks::BANDWIDTH_40:
        bandwidth_int = 40;
        break;
    case beerocks::BANDWIDTH_80:
        bandwidth_int = 80;
        break;
    case beerocks::BANDWIDTH_160:
        bandwidth_int = 160;
        break;
    default:
        bandwidth_int = 80;
        break;
    }
    return bandwidth_int;
}

std::string utils::convert_channel_ext_above_to_string(bool channel_ext_above_secondary,
                                                       beerocks::eWiFiBandwidth bandwidth)
{
    switch (bandwidth) {
    case beerocks::BANDWIDTH_20:
        return std::string();
        break;
    case beerocks::BANDWIDTH_40:
    case beerocks::BANDWIDTH_80:
    case beerocks::BANDWIDTH_160:
        if (channel_ext_above_secondary) {
            return "H";
        } else {
            return "L";
        }
    default:
        return std::string();
    }
}

int utils::wifi_channel_to_freq(int channel)
{
    if (channel == 14)
        return 2484;

    if (channel < 14)
        return (channel * 5) + 2407;

    return (channel + 1000) * 5;
}

uint16_t utils::wifi_channel_to_vht_center_freq(int channel, int bandwidth,
                                                bool channel_ext_above_secondary)
{
    int freq = wifi_channel_to_freq(channel);
    uint16_t vht_center_freq;
    switch (bandwidth) {
    case 20:
        vht_center_freq = freq;
        break;
    case 40:
        vht_center_freq = freq + (channel_ext_above_secondary ? 10 : -10);
        break;
    case 80:
        vht_center_freq = freq + (channel_ext_above_secondary ? 30 : -30);
        break;
    case 160:
        vht_center_freq = freq + (channel_ext_above_secondary ? 70 : -70);
        break;
    default:
        LOG(ERROR) << "invalid bandwidth!";
        return -1;
    }
    return vht_center_freq;
}

int utils::wifi_freq_to_channel(int freq)
{
    if (freq == 2484)
        return 14;

    if (freq < 2484)
        return (freq - 2407) / 5;

    /* FIXME: dot11ChannelStartingFactor (802.11-2007 17.3.8.3.2) */
    return freq / 5 - 1000;
}

void utils::merge_list(std::vector<uint8_t> &ret, std::vector<uint8_t> &list)
{
    std::vector<uint8_t>::iterator it;
    for (it = list.begin(); it != list.end(); it++) {
        if ((*it != 0) && (std::find(ret.begin(), ret.end(), *it) == ret.end())) {
            ret.push_back(*it);
        }
    }
}

std::string utils::dump_buffer(const uint8_t *buffer, size_t len)
{
    std::ostringstream hexdump;
    for (size_t i = 0; i < len; i += 16) {
        for (size_t j = i; j < len && j < i + 16; j++)
            hexdump << std::hex << std::setw(2) << std::setfill('0') << (unsigned)buffer[j] << " ";
        hexdump << std::endl;
    }
    return hexdump.str();
}

void utils::hex_dump(const std::string &description, uint8_t *addr, int len,
                     const char *calling_file, int calling_line)
{
    int16_t i;
    char ascii_chars[17]        = {};
    uint8_t *pc                 = addr;
    auto calling_file_str       = std::string(calling_file);
    const auto caller_file_name = calling_file_str.substr(calling_file_str.rfind('/') + 1);
    std::stringstream print_stream;

    // Process every byte in the data.
    for (i = 0; i < len; i++) {
        // Multiple of 16 means new line (with line offset).

        if ((i % 16) == 0) {
            // Just don't print ASCII for the zeroth line.
            if (i != 0) {
                print_stream << "   " << std::string(ascii_chars) << std::endl;
            }

            print_stream << string_utils::int_to_hex_string(i, 4);
        }

        // Now the hex code for the specific character.
        print_stream << " " << string_utils::int_to_hex_string(i, 2);

        // And store a printable ASCII character for later.
        if ((pc[i] < 0x20) || (pc[i] > 0x7e)) {
            ascii_chars[i % 16] = '.';
        } else {
            ascii_chars[i % 16] = pc[i];
        }
        ascii_chars[(i % 16) + 1] = '\0';
    }

    // Pad out last line if not exactly 16 characters.
    while ((i % 16) != 0) {
        print_stream << "   ";
        i++;
    }

    // And print the final ASCII bit.
    print_stream << "   " << ascii_chars << std::endl;

    std::cout << description << std::endl << print_stream.str();

    LOG(DEBUG) << caller_file_name << "[" << (int)calling_line << "] " << description << std::endl
               << print_stream.str();
}
