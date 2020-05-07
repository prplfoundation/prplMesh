
#include "../vs_beacon_response_to_1905.h"
#include <bcl/network/network_utils.h>
#include <tlvf/CmduMessageTx.h>
#include <tlvf/wfa_map/tlvBeaconMetricsResponse.h>

constexpr const int RET_OK          = 0;
constexpr const int RET_NO_BEEROCKS = -1;
constexpr const int RET_NO_1905     = -2;

/*
typedef struct sBeaconResponse11k {
    uint8_t channel;
    uint8_t op_class;
    uint8_t dialog_token;
    uint8_t measurement_token;
    uint8_t rep_mode;
    uint8_t phy_type;
    uint8_t frame_type;
    int8_t rcpi;
    uint8_t rsni;
    uint8_t ant_id;
    uint16_t duration;
    uint32_t parent_tsf;
    uint64_t start_time;
    sMacAddr sta_mac;
    sMacAddr bssid;
    uint32_t new_ch_width;
    uint32_t new_ch_center_freq_seg_0;
    uint32_t new_ch_center_freq_seg_1;
    uint8_t use_optional_wide_band_ch_switch;
    }
*/

std::ostream &operator<<(std::ostream &o, const beerocks_message::sBeaconResponse11k &response)
{
    // NOTE: not all fields are printed
    // TODO: complete all fields

    o << "\n"
      << "channel:                          "
      << "0x" << std::hex << +response.channel << "\n"
      << "op_class:                         "   
      << "0x" << std::hex << +response.op_class << "\n"
      << "dialog_token:                     " << +response.dialog_token << "\n"
      << "measurement_token:                " << +response.measurement_token << "\n"
      << "rep_mode:                         " << +response.rep_mode << "\n"
      << "phy_type:                         " << +response.phy_type << "\n"
      << "frame_type:                       " << +response.frame_type << "\n"
      << "rcpi:                             " << response.rcpi << "\n"
      << "rsni:                             " << +response.rsni << "\n"
      << "ant_id:                           " << +response.ant_id << "\n"
      << "duration:                         " << response.duration << "\n"
      << "parent_tsf:                       " << response.parent_tsf << "\n"
      << "start_time:                       " << response.start_time << "\n"
      << "sta mac:                          " << response.sta_mac << "\n"
      << "bssid:                            " << response.bssid << "\n"
      << "new_ch_width:                     " << response.new_ch_width << "\n"
      << "new_ch_center_freq_seg_0:         " << response.new_ch_center_freq_seg_0 << "\n"
      << "new_ch_center_freq_seg_1:         " << response.new_ch_center_freq_seg_1 << "\n"
      << "use_optional_wide_band_ch_switch: " << response.use_optional_wide_band_ch_switch << "\n";

    return o;
}


std::ostream &operator<<(std::ostream &o,
                         beerocks_message::cACTION_MONITOR_CLIENT_BEACON_11K_RESPONSE &msg)
{
    // currently we don't print anything except for the params field
    // we might want to print / verify the action op (currently hard coded in the class)
    o << "ACTION_MONITOR_CLIENT_BEACON_11K_RESPONSE:"
      << "\n-----------------------------------------\n"
      << msg.params() << "\n-----------------------------------------\n";

    return o;
}

void fillVsValues(std::shared_ptr<beerocks_message::cACTION_MONITOR_CLIENT_BEACON_11K_RESPONSE> vs)
{
}

int main()
{
    // preperation for beerocks

    // class list
    constexpr const uint16_t BUF_SIZE_BEEROCKS = 10000;
    uint8_t buff_beerocks[BUF_SIZE_BEEROCKS];
    ClassList cl(buff_beerocks, BUF_SIZE_BEEROCKS);

    // beerocks
    cl.addClass<beerocks_message::cACTION_MONITOR_CLIENT_BEACON_11K_RESPONSE>();

    auto beerocks = cl.getClass<beerocks_message::cACTION_MONITOR_CLIENT_BEACON_11K_RESPONSE>();

    if (!beerocks) {
        LOG(ERROR) << "no beerocks";
        return RET_NO_BEEROCKS;
    }

    // fill the vs message with values
    fillVsValues(beerocks);

    // build tx message from vs message
    //
    // construct a Tx message with beacon metrics query
    constexpr const uint16_t BUF_SIZE_1905_TX = 10000;
    uint8_t buff_1905_tx[BUF_SIZE_1905_TX];
    ieee1905_1::CmduMessageTx tx(buff_1905_tx, BUF_SIZE_1905_TX);
    tx.create(0x1, ieee1905_1::eMessageType::BEACON_METRICS_QUERY_MESSAGE);
    tx.addClass<wfa_map::tlvBeaconMetricsResponse>();

    // DUT -
    gate::load(/*dst*/tx, /*src*/beerocks);

    // verification (hunman readable output)
    beerocks_message::cACTION_MONITOR_CLIENT_BEACON_11K_RESPONSE &msg = *beerocks;
    std::cout << msg;

    return RET_OK;
}
