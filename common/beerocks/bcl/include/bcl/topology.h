/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef TOPOLOGY_H
#define TOPOLOGY_H

#include <algorithm>
#include <memory>
#include <string>
#include <vector>

#include <tlvf/common/sMacAddr.h>

#include "beerocks_defines.h"

namespace beerocks {

/**
 * @brief Generic topology manager
 *
 * This template manages the relationships between agents, radios, VAPs and stations. It is intended
 * to be instantiated as follows:
 * @code
 * using MyTopology = public Topology<MyTopologyInfo, MyAgentInfo, MyRadioInfo, MyVAPInfo,
 *                                    MyStationInfo>;
 * @endcode
 *
 * The topology components (Agent, Radio, ...) are defined as inner classes of Topology, which
 * allows instantiating the template for all of them in one go.
 *
 * The Info classes must be copy-constructible.
 *
 * Except for Topology itself, none of the classes can be insantiated directly. Use the create
 * functions instead.
 */
template <class AgentInfo, class RadioInfo, class VAPInfo, class StationInfo>
class Topology {
public:
    class VirtualAP;
    class Radio;
    class Agent;

    /// Representation of a station (= a client).
    class Station {
    public:
        /// MAC address of the station.
        const sMacAddr m_mac;

        /// User-supplied extra information for the station.
        StationInfo m_info;

    private:
        Station(const sMacAddr &mac, const StationInfo &info) : m_mac(mac), m_info(info) {}
        Station()                = delete;
        Station(const Station &) = delete;
        void operator=(const Station &) = delete;
        ~Station()                      = default;
    };

    /// Representation of a BSS (= a VAP = a Virtual Access Point)
    class VirtualAP {
    public:
        /// Radio on which the VAP exists.
        Radio &m_radio;
        /// BSSID of the VAP.
        const sMacAddr m_bssid;
        /// Name of the VAP. This is not guaranteed to be unique or even exist.
        const std::string m_name;

        /// User-supplied extra information for VAP.
        VAPInfo m_info;

        /// Stations associated with this VAP.
        const std::vector<std::shared_ptr<Station>> associated_stations()
        {
            return m_associated_stations;
        };

    private:
        VirtualAP(Radio &radio, const sMacAddr &bssid, const VAPInfo &info,
                  const std::string name = "")
            : m_radio(radio), m_bssid(bssid), m_name(name), m_info(info)
        {
        }
        VirtualAP()                  = delete;
        VirtualAP(const VirtualAP &) = delete;
        void operator=(const VirtualAP &) = delete;
        ~VirtualAP()                      = default;

        std::vector<std::shared_ptr<Station>> m_associated_stations;
    };

    /// Representation of a radio
    class Radio {
    public:
        /// Agent on which the radio exists.
        Agent &m_agent;
        /// Radio UID.
        const sMacAddr m_uid;
        /// Name of the radio. This is not guaranteed to be unique or even exist.
        const std::string m_name;
        /// Frequency band
        const eFreqType m_freq_type;
        /// User-supplied extra information for Radio.
        RadioInfo m_info;

        const std::vector<VirtualAP> vaps() const { return m_vaps; };

        /// @brief Create a VAP.
        /// @pre VAP with same BSSID does not exist.
        VirtualAP &create_vap(const sMacAddr &bssid, const VAPInfo &info,
                              const std::string name = "")
        {
            m_vaps.emplace_back({*this, bssid, info, name});
            return m_vaps.back();
        }

    private:
        Radio(Agent &agent, const sMacAddr &uid, eFreqType freq_type, const RadioInfo &info,
              const std::string name = "")
            : m_agent(agent), m_uid(uid), m_name(name), m_freq_type(freq_type), m_info(info)
        {
        }
        Radio()              = delete;
        Radio(const Radio &) = delete;
        void operator=(const Radio &) = delete;
        ~Radio()                      = default;

        std::vector<VirtualAP> m_vaps;
    };

    /// Representation of an agent (= a 1905.1 device)
    class Agent {
    public:
        /// Topology in which the agent exists.
        Topology &m_topology;
        /// Agent AL MAC address
        const sMacAddr m_al_mac;
        /// User-supplied extra information for Agent.
        AgentInfo m_info;

        const std::vector<Radio> radios() const { return m_radios; };

        /// @brief Create a radio.
        /// @pre Radio with same UID does not exist on this agent.
        Radio &create_radio(const sMacAddr &uid, eFreqType freq_type, const RadioInfo &info,
                            const std::string name = "")
        {
            m_radios.emplace_back({*this, uid, freq_type, info, name});
            return m_radios.back();
        }

        Agent(Agent &&) = default;
    private:
        Agent(Topology &topology, sMacAddr al_mac, AgentInfo info)
            : m_topology(topology), m_al_mac(al_mac), m_info(info)
        {
        }
        Agent()              = delete;
        Agent(const Agent &) = delete;
        void operator=(const Agent &) = delete;

         std::vector<Radio> m_radios;
    };

    /// Retrieve all agents.
    const std::vector<Agent> agents() const { return m_agents; };

    /// @brief Create an agent.
    /// @pre Agent with same al_mac address does not exist.
    Agent &create_agent(const sMacAddr &al_mac, const AgentInfo &info)
    {
        m_agents.emplace_back(*this, al_mac, info);
        return m_agents.back();
    }

    /// Functions to look up object
    /// @{

    /// @brief Find agent with the given MAC address.
    /// @return nullptr if no such agent exists.
    Agent *find_agent_by_al_mac(const sMacAddr &al_mac)
    {
        auto it = std::find_if(m_agents.begin(), m_agents.end(), [&al_mac](const Agent &agent){
            return al_mac == agent.al_mac;
        });
        if (it == m_agents.end()) {
            return nullptr;
        } else {
            return it;
        }
    }

    ~Topology() = default;

private:
    std::vector<Agent> m_agents;
};

} // namespace beerocks

#endif // TOPOLOGY_H
