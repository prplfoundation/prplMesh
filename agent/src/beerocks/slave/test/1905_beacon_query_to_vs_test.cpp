
#include "../1905_beacon_query_to_vs.h"
#include <bcl/network/network_utils.h>
#include <tlvf/CmduMessageTx.h>

//bool load( std::shared_ptr<beerocks_message::cACTION_MONITOR_CLIENT_BEACON_11K_REQUEST> lhs, const ieee1905_1::CmduMessageRx &rhs );
//

constexpr const int RET_OK          = 0;
constexpr const int RET_NO_BEEROCKS = -1;
constexpr const int RET_NO_1905     = -2;

std::ostream &operator<<(std::ostream &o, const beerocks_message::sBeaconRequest11k &request)
{
    /*
    uint8_t measurement_mode;
    uint8_t channel;
    int16_t op_class;
    uint16_t repeats;
    uint16_t rand_ival;
    uint16_t duration;
    sMacAddr sta_mac;
    sMacAddr bssid;
    uint8_t parallel;
    uint8_t enable;
    uint8_t request;
    uint8_t report;
    uint8_t mandatory_duration;
    uint8_t expected_reports_count;
    uint8_t use_optional_ssid;
    char ssid[beerocks::message::WIFI_SSID_MAX_LENGTH];
    uint8_t use_optional_ap_ch_report;
    uint8_t ap_ch_report[237];
    uint8_t use_optional_req_elements;
    uint8_t req_elements[13];
    uint8_t use_optional_wide_band_ch_switch;
    uint32_t new_ch_width;
    uint32_t new_ch_center_freq_seg_0;
    uint32_t new_ch_center_freq_seg_1;
    */

    o << "\n"
      << "measurement_mode:         " << +request.measurement_mode << "\n"
      << "channel:                  "
      << "0x" << std::hex << +request.channel << "\n"
      << "op_class:                 "
      << "0x" << std::hex << request.op_class << "\n"
      << "repeats:                  " << request.repeats << "\n"
      << "rand_ival:                " << request.rand_ival << "\n"
      << "duration:                 " << request.duration << "\n"
      << "sta mac:                  " << request.sta_mac << "\n"
      << "bssid:                    " << request.bssid << "\n"
      << "parallel:                 " << +request.parallel << "\n"
      << "enable:                   " << +request.enable << "\n"
      << "request:                  " << +request.request << "\n"
      << "report:                   " << +request.report << "\n"
      << "mandatory_duration:       " << +request.mandatory_duration << "\n"
      << "expected_reports_count:   " << +request.expected_reports_count << "\n"
      << "use_optional_ssid:        " << +request.use_optional_ssid << "\n"
      << "ssid:                     " << request.ssid << "\n"
      << "use_optional_ap_ch_report:" << +request.use_optional_ap_ch_report << "\n";

    for (int idx = 0; idx < request.use_optional_ap_ch_report; ++idx) {
        o << "channel[" << idx << "]:    " << std::dec << +request.ap_ch_report[idx] << "\n";
    }

    return o;
}

std::ostream &operator<<(std::ostream &o,
                         beerocks_message::cACTION_MONITOR_CLIENT_BEACON_11K_REQUEST &msg)
{
    o << "ACTION_MONITOR_CLIENT_BEACON_11K_REQUEST:"
      << "\n-----------------------------------------\n"
      << msg.params() << "\n-----------------------------------------\n";

    return o;
}

void fillTxValues(ieee1905_1::CmduMessageTx &tx)
{
    auto tlv = tx.getClass<wfa_map::tlvBeaconMetricsQuery>();
    if (!tlv) {
        LOG(ERROR) << "this Tx message does not contain tlv beacon metrics query";
    }

    tlv->associated_sta_mac() = sMacAddr{0x01, 0x02, 0x03, 0x04, 0x05, 0x06};
    tlv->operating_class()    = 0x73;
    tlv->channel_number()     = 0xff;

    auto ap_channel_list = tlv->create_ap_channel_reports_list();
    if (!ap_channel_list) {
        LOG(ERROR) << "unable to create channel report list";
        return;
    }
    ap_channel_list->alloc_ap_channel_report_list(3);
    *ap_channel_list->ap_channel_report_list(0) = 0x73;
    *ap_channel_list->ap_channel_report_list(1) = 46;
    *ap_channel_list->ap_channel_report_list(2) = 31;

    tlv->add_ap_channel_reports_list(ap_channel_list);

    // verify
    for (int idx = 0; idx < 3; ++idx) {
        std::cout << "DEBUG: " << idx << ":" << std::hex
                  << +*(std::get<1>(tlv->ap_channel_reports_list(0)).ap_channel_report_list(idx))
                  << "\n";
    }
}

int main()
{
    // preperation for beerocks

    // class list
    constexpr const uint16_t BUF_SIZE_BEEROCKS = 10000;
    uint8_t buff_beerocks[BUF_SIZE_BEEROCKS];
    ClassList cl(buff_beerocks, BUF_SIZE_BEEROCKS);

    // beerocks
    cl.addClass<beerocks_message::cACTION_MONITOR_CLIENT_BEACON_11K_REQUEST>();

    auto beerocks = cl.getClass<beerocks_message::cACTION_MONITOR_CLIENT_BEACON_11K_REQUEST>();

    if (!beerocks) {
        LOG(ERROR) << "no beerocks";
        return RET_NO_BEEROCKS;
    }

    // build Rx message from Tx message
    //
    // Tx with beacon metrics query
    constexpr const uint16_t BUF_SIZE_1905_TX = 10000;
    uint8_t buff_1905_tx[BUF_SIZE_1905_TX];
    ieee1905_1::CmduMessageTx tx(buff_1905_tx, BUF_SIZE_1905_TX);
    tx.create(0x1, ieee1905_1::eMessageType::BEACON_METRICS_QUERY_MESSAGE);
    tx.addClass<wfa_map::tlvBeaconMetricsQuery>();

    fillTxValues(tx);

    // Rx - built from the tx message above
    ieee1905_1::CmduMessageRx rx(tx.getMessageBuff(), tx.getMessageBuffLength());
    rx.parse();
    auto tlvRx = rx.getClass<wfa_map::tlvBeaconMetricsQuery>();

    if (!tlvRx) {
        LOG(ERROR) << "no 1905 rx";
        return RET_NO_1905;
    }

    gate::load(/*dst*/ beerocks, /*src*/ rx);

    beerocks_message::cACTION_MONITOR_CLIENT_BEACON_11K_REQUEST &msg = *beerocks;

    std::cout << msg;

    return RET_OK;
}
