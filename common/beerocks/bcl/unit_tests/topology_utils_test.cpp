/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include <bcl/topology.h>
#include <bcl/son/son_wireless_utils.h>

#include <gtest/gtest.h>

using namespace beerocks;
namespace {

/// Auxiliary structures to set up the topology
/// @{
struct sTestAgentInfo {};
struct sTestRadioInfo {};
struct sTestVAPInfo {};
struct sTestStationInfo {};
using TestTopology = Topology<sTestAgentInfo, sTestRadioInfo, sTestVAPInfo, sTestStationInfo>;
/// @}

class topology_unit_test : ::testing::Test {
protected:
    topology_unit_test() {
//        std::vector<TestTopology::Agent> agents;
//        sMacAddr addr = tlvf::mac_from_string("11:22:33:44:55:00");
//        agents.emplace_back(topology, addr, dummy_agent_info);
        auto &agent1 = topology.create_agent(tlvf::mac_from_string("11:22:33:44:55:00"), dummy_agent_info);
    }

    TestTopology topology;
    static constexpr sTestAgentInfo dummy_agent_info = {};
};

TEST(topology_unit_test, find_vap_by_bssid)
{
}

} // namespace
