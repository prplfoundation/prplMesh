
#include "expected_ap_metrics_response.h"

// mid
void ExpectedApMetricsResponse::reset_to_new_mid(uint16_t mid)
{
    _mid = mid;
    _expected_bssid_list.clear();
    _response.reset();
    memset(_response_buffer, 0, sizeof(_response_buffer));
}

uint16_t ExpectedApMetricsResponse::get_mid() const { return _mid; }

// bssid
void ExpectedApMetricsResponse::add_expected_bssid(const sMacAddr &bssid)
{
    _expected_bssid_list.insert(bssid);
}

void ExpectedApMetricsResponse::remove_expected_bssid(const sMacAddr &bssid)
{
    _expected_bssid_list.erase(bssid);
}

bool ExpectedApMetricsResponse::is_expected_bssid_empty() const
{
    return _expected_bssid_list.empty();
}

bool ExpectedApMetricsResponse::find_expected_bssid(const sMacAddr &bssid) const
{
    return _expected_bssid_list.find(bssid) != _expected_bssid_list.end();
}

// cmdu message
ieee1905_1::CmduMessageTx &ExpectedApMetricsResponse::create_tx_message()
{
    _response.create(_mid, ieee1905_1::eMessageType::AP_METRICS_RESPONSE_MESSAGE);

    return _response;
}
