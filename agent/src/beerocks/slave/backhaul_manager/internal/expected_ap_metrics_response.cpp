
#include "expected_ap_metrics_response.h"

namespace beerocks {

// mid
void ExpectedApMetricsResponse::reset_to_new_mid(uint16_t mid)
{
    m_mid = mid;
    m_expected_bssid_list.clear();
    m_response.reset();
    memset(m_response_buffer, 0, sizeof(m_response_buffer));
}

uint16_t ExpectedApMetricsResponse::get_mid() const { return m_mid; }

// bssid
void ExpectedApMetricsResponse::add_expected_bssid(const sMacAddr &bssid)
{
    m_expected_bssid_list.insert(bssid);
}

void ExpectedApMetricsResponse::remove_expected_bssid(const sMacAddr &bssid)
{
    m_expected_bssid_list.erase(bssid);
}

bool ExpectedApMetricsResponse::is_expected_bssid_empty() const
{
    return m_expected_bssid_list.empty();
}

bool ExpectedApMetricsResponse::find_expected_bssid(const sMacAddr &bssid) const
{
    return m_expected_bssid_list.find(bssid) != m_expected_bssid_list.end();
}

// cmdu message
ieee1905_1::CmduMessageTx &ExpectedApMetricsResponse::create_tx_message()
{
    m_response.create(m_mid, ieee1905_1::eMessageType::AP_METRICS_RESPONSE_MESSAGE);

    return m_response;
}

} // namespace beerocks
