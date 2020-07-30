/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include "1905_beacon_query_to_vs.h"
#include "vs_beacon_response_to_1905.h"
#include <bcl/network/network_utils.h>
#include <tlvf/CmduMessageTx.h>
#include <tlvf/wfa_map/tlvBeaconMetricsResponse.h>

#include <gtest/gtest.h>

namespace {

/////// 1905 --> vs   ////////////////////////////////////////////////////////////////////////////////////

/**
* const values to be used for both assignment and test
*/

const auto associated_sta_mac_1 = sMacAddr{0x01, 0x02, 0x03, 0x04, 0x05, 0x06};
const uint8_t operating_class   = 0x73;
const uint8_t channel_number    = 255;
const uint8_t channel_1         = 46;
const uint8_t channel_2         = 31;

/**
* helper function to fill 1905 values
*/
static bool set_1905_values(ieee1905_1::CmduMessageTx &cmdu1905)
{
    auto beacon_metrics_query = cmdu1905.getClass<wfa_map::tlvBeaconMetricsQuery>();
    if (!beacon_metrics_query) {
        return false;
    }

    // fill some values
    beacon_metrics_query->associated_sta_mac() = associated_sta_mac_1;
    beacon_metrics_query->operating_class()    = operating_class;
    // for ap channel report list, this must be set to 255
    beacon_metrics_query->channel_number() = channel_number;

    auto ap_channel_list = beacon_metrics_query->create_ap_channel_reports_list();
    if (!ap_channel_list) {
        return false;
    }

    ap_channel_list->alloc_ap_channel_report_list(3);
    // the first is operating class, which is currently ignored system wide
    *ap_channel_list->ap_channel_report_list(0) = operating_class;
    *ap_channel_list->ap_channel_report_list(1) = channel_1;
    *ap_channel_list->ap_channel_report_list(2) = channel_2;

    beacon_metrics_query->add_ap_channel_reports_list(ap_channel_list);

    return true;
}

TEST(gate_beacon_query_test, loading_vs_from_1905_beacon_query)
{
    // preperation for beerocks

    // class list

    constexpr uint16_t buf_size_beerocks = 10000; // just an arbitrary size
    uint8_t buff_beerocks[buf_size_beerocks];
    ClassList cl(buff_beerocks, buf_size_beerocks);

    // beerocks
    auto added_class_vs =
        cl.addClass<beerocks_message::cACTION_MONITOR_CLIENT_BEACON_11K_REQUEST>();

    EXPECT_NE(added_class_vs, nullptr);

    auto beerocks = cl.getClass<beerocks_message::cACTION_MONITOR_CLIENT_BEACON_11K_REQUEST>();

    EXPECT_NE(beerocks, nullptr);

    // build 1905 message whthin a Tx message

    // construct a Tx message with beacon metrics query
    constexpr uint16_t buf_size_1905_tx = 10000; // just an arbitrary size
    uint8_t buff_1905_tx[buf_size_1905_tx];
    ieee1905_1::CmduMessageTx cmdu_1905(buff_1905_tx, buf_size_1905_tx);

    auto metrics_query =
        cmdu_1905.create(0x1, ieee1905_1::eMessageType::BEACON_METRICS_QUERY_MESSAGE);
    EXPECT_NE(metrics_query, nullptr);

    auto added_class_1905 = cmdu_1905.addClass<wfa_map::tlvBeaconMetricsQuery>();
    EXPECT_NE(added_class_1905, nullptr);

    // fill 1905 message with values
    bool filled = set_1905_values(cmdu_1905);
    EXPECT_TRUE(filled);

    // Rx - built from the tx message above
    ieee1905_1::CmduMessageRx rx(cmdu_1905.getMessageBuff(), cmdu_1905.getMessageBuffLength());

    bool parse_ok = rx.parse();
    EXPECT_EQ(parse_ok, true);

    auto tlv_becon_metrics_query = rx.getClass<wfa_map::tlvBeaconMetricsQuery>();
    EXPECT_NE(tlv_becon_metrics_query, nullptr);

    // DUT -
    bool load_ok = beerocks::gate::load(/*dst*/ beerocks, /*src*/ rx);
    EXPECT_EQ(load_ok, true);

    auto &beerocks_params = beerocks->params();

    // variables that were set in the test
    EXPECT_EQ(beerocks_params.channel, channel_number);
    EXPECT_EQ(beerocks_params.sta_mac, associated_sta_mac_1);
    EXPECT_EQ(beerocks_params.op_class, operating_class);

    // default values
    EXPECT_EQ(beerocks_params.bssid, beerocks::net::network_utils::ZERO_MAC);
    EXPECT_EQ(beerocks_params.measurement_mode, beerocks::MEASURE_MODE_ACTIVE);
    EXPECT_EQ(beerocks_params.duration, beerocks::BEACON_MEASURE_DEFAULT_ACTIVE_DURATION);
    EXPECT_EQ(beerocks_params.expected_reports_count, 1);
    EXPECT_EQ(beerocks_params.rand_ival, beerocks::BEACON_MEASURE_DEFAULT_RANDOMIZATION_INTERVAL);
    EXPECT_EQ(beerocks_params.report, 0);
    EXPECT_EQ(beerocks_params.repeats, 0);
    EXPECT_EQ(beerocks_params.parallel, 0);
    EXPECT_EQ(beerocks_params.enable, 0);
    EXPECT_EQ(beerocks_params.request, 1);
    EXPECT_EQ(beerocks_params.mandatory_duration, 0);
    EXPECT_EQ(beerocks_params.use_optional_ssid, 0);
}

/////// vs --> 1905 ////////////////////////////////////////////////////////////////////////////////////

/**
* const values to be used for both assignment and test
*/
const auto associated_sta_mac_2 = sMacAddr{0xba, 0xd0, 0xde, 0xad, 0xbe, 0xef};

void fill_vs_values(beerocks_message::cACTION_MONITOR_CLIENT_BEACON_11K_RESPONSE &vs)
{
    vs.params().sta_mac = associated_sta_mac_2;
}

TEST(gate_beacon_response_test, loading_1905_from_vs_response)
{
    // preperation for beerocks

    // class list
    constexpr uint16_t buf_size_beerocks = 10000;
    uint8_t buff_beerocks[buf_size_beerocks];
    ClassList cl(buff_beerocks, buf_size_beerocks);

    // beerocks
    auto added_class_vs =
        cl.addClass<beerocks_message::cACTION_MONITOR_CLIENT_BEACON_11K_RESPONSE>();
    EXPECT_NE(added_class_vs, nullptr);

    auto beerocks = cl.getClass<beerocks_message::cACTION_MONITOR_CLIENT_BEACON_11K_RESPONSE>();
    EXPECT_NE(beerocks, nullptr);

    // fill the vs message with values
    fill_vs_values(*beerocks);

    // build tx message from vs message

    // construct a Tx message with beacon metrics query
    constexpr uint16_t buf_size_1905_tx = 10000;
    uint8_t buff_1905_response[buf_size_1905_tx];
    ieee1905_1::CmduMessageTx response(buff_1905_response, buf_size_1905_tx);

    auto metrics_response =
        response.create(0x1, ieee1905_1::eMessageType::BEACON_METRICS_QUERY_MESSAGE);
    EXPECT_NE(metrics_response, nullptr);

    auto added_class_1905 = response.addClass<wfa_map::tlvBeaconMetricsResponse>();
    EXPECT_NE(added_class_1905, nullptr);

    // DUT -
    bool load_ok = beerocks::gate::load(/*dst*/ response, /*src*/ beerocks);
    EXPECT_EQ(load_ok, true);

    auto check_response = response.getClass<wfa_map::tlvBeaconMetricsResponse>();
    EXPECT_NE(check_response, nullptr);
    EXPECT_EQ(check_response->associated_sta_mac(), associated_sta_mac_2);
}

} // namespace
