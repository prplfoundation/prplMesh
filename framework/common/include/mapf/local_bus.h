/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef __MAPF_LOCAL_BUS_API_H__
#define __MAPF_LOCAL_BUS_API_H__

#include <mapf/broker/broker_interface.h>
#include <mapf/common/config.h>

namespace mapf {

static const std::string kLocalBusConf(std::string(MAPF_ROOT) + "/share/local_bus.conf");

class LocalBusInterface : public BrokerInterface {
public:
    explicit LocalBusInterface(Context &ctx) : BrokerInterface(ctx, kLocalBusConf) {}
    ~LocalBusInterface() {}
};
} // namespace mapf

#endif /* __MAPF_LOCAL_BUS_API_H__ */
