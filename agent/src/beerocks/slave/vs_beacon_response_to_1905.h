/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _VS_BEACON_RESPNSE_TO_1905_H_
#define _VS_BEACON_RESPNSE_TO_1905_H_

#include <beerocks/tlvf/beerocks_message_monitor.h>
#include <tlvf/CmduMessageTx.h>

namespace gate {

// loads lhs with rhs. as if it was written: lhs = rhs; or lhs << rhs;
// however load() was chosen over operator=() or operator<<() as it permits returning success/fail with the bool return value,
// and also it opens the possibilites for adding additional parameters in the future if needed
bool load(ieee1905_1::CmduMessageTx &lhs,
          std::shared_ptr<beerocks_message::cACTION_MONITOR_CLIENT_BEACON_11K_RESPONSE> rhs);

} // namespace gate

#endif
