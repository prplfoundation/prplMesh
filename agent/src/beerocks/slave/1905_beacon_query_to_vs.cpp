
#include "1905_beacon_query_to_vs.h"
#include <bcl/beerocks_defines.h>
#include <mapf/common/utils.h>

namespace gate {

bool load(std::shared_ptr<beerocks_message::cACTION_MONITOR_CLIENT_BEACON_11K_REQUEST> lhs,
          const ieee1905_1::CmduMessageRx &rhs)
{
    // escape I
    if (!lhs) {
        LOG(ERROR) << "can't fill empty pointer. lhs is nullptr. returning";
        return false;
    }

    // escape II
    // get the correct type of the message
    auto beacon_metrics_query = rhs.getClass<wfa_map::tlvBeaconMetricsQuery>();
    if (!beacon_metrics_query) {
        LOG(ERROR) << "tlvBeaconMetricsQuery is missing - can't read from invalid input";
        return false;
    }

    auto &lhs_params = lhs->params();

    lhs_params.bssid   = beacon_metrics_query->bssid();
    lhs_params.channel = beacon_metrics_query->channel_number();

    lhs_params.measurement_mode = beerocks::MEASURE_MODE_ACTIVE;
    lhs_params.duration         = beerocks::BEACON_MEASURE_DEFAULT_ACTIVE_DURATION;

    lhs_params.expected_reports_count = 1;

    lhs_params.rand_ival = beerocks::BEACON_MEASURE_DEFAULT_RANDOMIZATION_INTERVAL;
    lhs_params.sta_mac   = beacon_metrics_query->associated_sta_mac();

    lhs_params.op_class = beacon_metrics_query->operating_class();

    // FixMe: not sure about this
    lhs_params.report = beacon_metrics_query->reporting_detail_value();

    // values based on https://github.com/prplfoundation/prplMesh/pull/1114#discussion_r406326546
    lhs_params.repeats            = 0;
    lhs_params.parallel           = 0;
    lhs_params.enable             = 0;
    lhs_params.request            = 1;
    lhs_params.mandatory_duration = 0;
    lhs_params.use_optional_ssid  = 0;

    mapf::utils::copy_string(lhs_params.ssid, beacon_metrics_query->ssid_str().c_str(),
                             beerocks::message::WIFI_SSID_MAX_LENGTH);

    // how many elements in the list of channels, we support only 1 at the moment
    auto ap_ch_report_length = beacon_metrics_query->ap_channel_reports_list_length();

    // escape III - data validation
    // ap_ch_report_length != 0 ==> channel_number == 255
    // we verify with the already extracted values (we could do it withthe original as well)
    if (ap_ch_report_length != 0 && lhs_params.channel != 255) {
        LOG(ERROR) << "inconsistency between channel report length and channel number. please take "
                      "look at the specification. v1 17.2.27";
        return false;
    }

    // copy m_ap_channel_reports_list from the 1905 beacon_metrics_query
    // into ap_ch_report char array of the vs message (lhs_params)
    if (ap_ch_report_length > 1) {
        // the reason we support only 1 channel list is that the hostapd does not support more
        // there is no way to tell it for which operating class which channels to use
        LOG(ERROR)
            << "too many channles requested (the maximum is 237, current support is only for 1): "
            << ap_ch_report_length;
        return false;
    }

    // we support only one
    if (1 == ap_ch_report_length) {

        // get the first (and currently only) structure from the list
        auto channel = beacon_metrics_query->ap_channel_reports_list(0);
        if (!std::get<0>(channel)) {
            LOG(ERROR) << "there should be a structure at index 0, but it wasn't found";
            return false;
        }

        // the number of channels, excluding the first operating class
        lhs_params.use_optional_ap_ch_report =
            std::get<1>(channel).ap_channel_report_list_length() - 1;

        // the first index in the tlv's channel-report-list is the operating class,
        // we skip it. using pointer arithmethics and copying one element less from the source (we skip the operation class)
        for (int idx = 0; idx < lhs_params.use_optional_ap_ch_report; ++idx) {
            lhs_params.ap_ch_report[idx] = *std::get<1>(channel).ap_channel_report_list(idx + 1);
        }
    }

    // FixMe:
    // the same is true for use_optional_req_elements and req_elements
    // end FixMe

    // FixMe:
    // lhs_params.use_optional_wide_band_ch_switch = ??
    // lhs_params.new_ch_width = ??
    // lhs_params.new_ch_center_freq_seg_0 = ??
    // lhs_params.new_ch_center_freq_seg_1 = ??
    // end FixMe
    //
    //
    return true;
}

} // namespace gate
