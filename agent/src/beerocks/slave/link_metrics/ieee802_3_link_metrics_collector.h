/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2020 MaxLinear
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */
#ifndef __IEEE802_3_LINK_METRICS_COLLECTOR_H__
#define __IEEE802_3_LINK_METRICS_COLLECTOR_H__

#include "link_metrics.h"

namespace beerocks {

class ieee802_3_link_metrics_collector : public link_metrics_collector {

public:
    virtual ~ieee802_3_link_metrics_collector();

    /**
     * @brief Gets link metrics information.
     *
     * Gets link metrics associated to the link between given local interface and a neighbor's
     * interface whose MAC address is 'neighbor_interface_address'.
     *
     * Note that metrics are associated to a link and not to an interface. For Ethernet interfaces
     * and in Linux though, it is not possible to obtain per link stats: in Linux is easy to check
     * how many packets were received by "eth0" *in total*, but it is not trivial to find out how
     * many packets were received by "eth0" *from each neighbor*. For the sake of simplicity this
     * implementation just reports the overall per-interface stats (thus ignoring the
     * 'neighbor_interface_address' parameter).
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
