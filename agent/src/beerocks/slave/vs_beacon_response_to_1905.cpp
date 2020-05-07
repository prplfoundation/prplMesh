/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include "vs_beacon_response_to_1905.h"

namespace gate {

bool load(ieee1905_1::CmduMessageTx &lhs,
          std::shared_ptr<beerocks_message::cACTION_MONITOR_CLIENT_BEACON_11K_RESPONSE> rhs)
{
    return true;
}

} // namespace gate


