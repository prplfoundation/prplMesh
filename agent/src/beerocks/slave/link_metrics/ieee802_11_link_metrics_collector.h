/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */
#ifndef __IEEE802_11_LINK_METRICS_COLLECTOR_H__
#define __IEEE802_11_LINK_METRICS_COLLECTOR_H__

#include "link_metrics.h"

namespace beerocks {

class ieee802_11_link_metrics_collector : public link_metrics_collector {

public:
    /**
     * @brief Gets link metrics information.
     *
     * Gets link metrics associated to the link between given local interface and a neighbor's
     * interface whose MAC address is 'neighbor_interface_address'.
     *
     * @param[in] local_interface_name Name of the local interface.
     * @param[in] neighbor_interface_address MAC address at the other end of the link (this MAC
     * address belongs to a neighbor's interface.
     * @param[out] link_metrics Link metrics information.
     *
     * @return True on success and false otherwise.
     */
    virtual bool get_link_metrics(const std::string &local_interface_name,
                                  const sMacAddr &neighbor_interface_address,
                                  sLinkMetrics &link_metrics) override;
};

} // namespace beerocks

#endif
