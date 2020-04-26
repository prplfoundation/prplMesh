/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _1905_BEACONB_QUERY_TO_VS_H_
#define _1905_BEACONB_QUERY_TO_VS_H_

#include <beerocks/tlvf/beerocks_message_monitor.h>
#include <tlvf/CmduMessageRx.h>
#include <tlvf/wfa_map/tlvBeaconMetricsQuery.h>

namespace gate {

// loads lhs with rhs. as if it was written: lhs = rhs;
// however load() was chosen over operator=() or operator<<() as it permits returning success/fail with the bool return value
// and also it opens the possibilites for adding additional parameters in the future if needed
bool load(std::shared_ptr<beerocks_message::cACTION_MONITOR_CLIENT_BEACON_11K_REQUEST> lhs,
          const ieee1905_1::CmduMessageRx &rhs);

} // namespace gate

#endif
