/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include <bcl/beerocks_utils.h>
#include <bcl/beerocks_version.h>

#include <cstdio>

#include <easylogging++.h>

using namespace beerocks;

constexpr char version::INVALID_VERSION_STRING[];
constexpr char version::INVALID_TIMESTAMP_STRING[];

// Static variable declaration
beerocks::version::beerocks_version_map_t beerocks::version::s_beerocks_version_map;

beerocks::version::version(std::string ver, std::string build_date, std::string build_rev)
{
    set_module_version("__main__", ver, build_date, build_rev);
}

void beerocks::version::set_module_version(std::string so_name, std::string ver,
                                           std::string build_date, std::string build_rev)
{
    s_beerocks_version_map[so_name] = std::make_tuple(ver, build_date, build_rev);
}

std::string beerocks::version::get_module_version(const std::string &module_name)
{
    return std::get<0>(s_beerocks_version_map[module_name]);
}

std::string beerocks::version::get_module_timestamp(const std::string &module_name)
{
    return std::get<1>(s_beerocks_version_map[module_name]);
}

std::string beerocks::version::get_module_revision(const std::string &module_name)
{
    return std::get<2>(s_beerocks_version_map[module_name]);
}

std::string beerocks::version::version_to_string(const sBinaryVersion &version)
{
    std::stringstream ss;
    ss << static_cast<uint32_t>(version.major) << "." << static_cast<uint32_t>(version.minor) << "."
       << static_cast<uint32_t>(version.build_number);

    return ss.str();
}

sBinaryVersion beerocks::version::version_from_string(const std::string &version)
{
    sBinaryVersion ret = {};
    if (version.empty())
        return ret;
    char delim; // hold on '.' delimiter from version string "X.X.X"
    uint32_t arr[3] = {0, 0, 0};
    std::stringstream ss(version);
    ss >> arr[0] >> delim >> arr[1] >> delim >> arr[2];
    ret.major        = static_cast<uint8_t>(arr[0]);
    ret.minor        = static_cast<uint8_t>(arr[1]);
    ret.build_number = static_cast<uint16_t>(arr[2]);
    return ret;
}

static std::string get_last_path(std::string path)
{
    auto delim_index = path.find_last_of("/");
    if (std::string::npos == delim_index) {
        return path;
    }

    auto last_path = path.substr(delim_index + 1);
    if (0 < last_path.length()) {
        return last_path;
    }

    auto start_index = path.find_last_of("/", delim_index - 1);
    if (std::string::npos == start_index) {
        return path.substr(0, delim_index - 1);
    } else {
        return path.substr(start_index + 1, delim_index - 1);
    }
}

void beerocks::version::print_version(bool verbose, const std::string &name,
                                      const std::string &description)
{
    std::cout << name << " " << version::get_module_version() << " ("
              << version::get_module_timestamp() << ") [" << version::get_module_revision() << "]"
              << std::endl;

    if (description.length() > 0) {
        std::cout << description << std::endl;
    }
    std::cout << "Copyright (c) 2018 Intel Corporation, All Rights Reserved." << std::endl
              << std::endl;
    if (verbose) {
        if (s_beerocks_version_map.size() > 1) {
            std::cout << "Additional Modules:" << std::endl;
            for (auto &module_node : s_beerocks_version_map) {
                if (module_node.first != "__main__") {
                    std::cout << std::string("  ") << module_node.first << std::string(": ")
                              << std::get<0>(module_node.second) << " ("
                              << std::get<1>(module_node.second) << ") ["
                              << std::get<2>(module_node.second) << "]" << std::endl;
                }
            }
        }
    }
}

void beerocks::version::log_version(int, char **argv)
{
    std::string name(get_last_path(argv[0]));

    LOG(INFO) << name << " " << version::get_module_version() << " ("
              << version::get_module_timestamp() << ") [" << version::get_module_revision() << "]";

    if (s_beerocks_version_map.size() > 1) {
        LOG(INFO) << "Additional Modules:";
        for (auto &module_node : s_beerocks_version_map) {
            if (module_node.first != "__main__") {
                LOG(INFO) << std::string("  ") << module_node.first << std::string(": ")
                          << std::get<0>(module_node.second) << " ("
                          << std::get<1>(module_node.second) << ") ["
                          << std::get<2>(module_node.second) << "]";
            }
        }
    }
}

bool beerocks::version::handle_version_query(int argc, char **argv, const std::string &description)
{
    if (argc > 1) {
        bool version = false;
        bool all     = false;
        for (int i = 1; i < argc; ++i) {
            if ((std::string("-v") == argv[i]) || (std::string("--version") == argv[i])) {
                version = true;
            } else if ((std::string("-va") == argv[i]) ||
                       (std::string("--version-all") == argv[i])) {
                version = true;
                all     = true;
            }
        }

        if (version) {
            print_version(all, get_last_path(argv[0]), description);
            return true;
        }
    }
    return false;
}
