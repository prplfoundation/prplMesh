/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef __MAPF_BROKER_CONFIG_H__
#define __MAPF_BROKER_CONFIG_H__

#include <iostream>
#include <set>

namespace mapf {

struct BrokerSecurity {
public:
    BrokerSecurity() {}

    BrokerSecurity(bool enable, unsigned int key_len, std::string xsub_secret,
                   std::string xsub_public, std::string pub_secret, std::string pub_public,
                   std::string xpub_secret, std::string xpub_public, std::string sub_secret,
                   std::string sub_public)
        : enable(enable), key_len(key_len), xsub_secret(xsub_secret), xsub_public(xsub_public),
          pub_secret(pub_secret), pub_public(pub_public), xpub_secret(xpub_secret),
          xpub_public(xpub_public), sub_secret(sub_secret), sub_public(sub_public)
    {
    }

    ~BrokerSecurity() {}

    bool enable          = false;
    unsigned int key_len = 41;
    // PUB <--> broker (XSUB) set of keys
    std::string xsub_secret;
    std::string xsub_public;
    std::string pub_secret;
    std::string pub_public;
    // SUB <--> to broker (XPUB) set of keys
    std::string xpub_secret;
    std::string xpub_public;
    std::string sub_secret;
    std::string sub_public;
};

struct BrokerConfig {
    BrokerConfig() {}
    BrokerConfig(const std::string &path) : path(path) {}
    ~BrokerConfig(){};
    int Parse();
    void Print();

    bool verbose = false;
    bool monitor = false;
    std::string path;
    std::string name;
    std::set<std::string> frontend, backend, capture; // endpoints
    BrokerSecurity security;
};

} /* namespace mapf */
#endif /* __MAPF_BROKER_CONFIG_H__ */
