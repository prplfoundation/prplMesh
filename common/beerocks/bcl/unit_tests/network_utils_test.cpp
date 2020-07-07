/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include <bcl/network/network_utils.h>

#include <dirent.h>

#include <gtest/gtest.h>

namespace {

std::vector<std::string> get_iface_names()
{

    std::vector<std::string> iface_names;

    const char *path = "/sys/class/net";

    DIR *dir = opendir(path);
    if (dir) {
        struct dirent *entry;
        while ((entry = readdir(dir)) != NULL) {
            std::string iface_name = entry->d_name;
            if (iface_name == "." || iface_name == "..") {
                continue;
            }
            iface_names.push_back(iface_name);
        }
        closedir(dir);
    }

    return iface_names;
}

TEST(NetworkUtilsTest, get_iface_stats_should_succeed)
{
    beerocks::net::sInterfaceStats iface_stats;

    for (const auto &iface_name : get_iface_names()) {
        ASSERT_TRUE(beerocks::net::network_utils::get_iface_stats(iface_name, iface_stats));

        std::cout << iface_name << ":" << std::endl;
        std::cout << "\ttx_bytes: " << std::to_string(iface_stats.tx_bytes) << std::endl;
        std::cout << "\ttx_packets: " << std::to_string(iface_stats.tx_packets) << std::endl;
        std::cout << "\ttx_errors: " << std::to_string(iface_stats.tx_errors) << std::endl;
        std::cout << "\trx_bytes: " << std::to_string(iface_stats.rx_bytes) << std::endl;
        std::cout << "\trx_packets: " << std::to_string(iface_stats.rx_packets) << std::endl;
        std::cout << "\trx_errors: " << std::to_string(iface_stats.rx_errors) << std::endl;
    }
}

} // namespace
