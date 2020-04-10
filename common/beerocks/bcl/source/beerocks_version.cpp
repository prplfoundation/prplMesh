/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2020 the prplMesh contributors (see AUTHORS.md)
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

const beerocks::version *beerocks::version::s_version = nullptr;

beerocks::version::version(std::string ver, std::string build_date, std::string build_rev)
    : m_ver(ver), m_build_date(build_date), m_build_rev(build_rev)
{
    s_version = this;
}

std::string beerocks::version::get_module_version()
{
    if (s_version)
        return s_version->m_ver;
    else
        return INVALID_VERSION_STRING;
}

std::string beerocks::version::get_module_timestamp()
{
    if (s_version)
        return s_version->m_build_date;
    else
        return INVALID_TIMESTAMP_STRING;
}

std::string beerocks::version::get_module_revision()
{
    if (s_version)
        return s_version->m_build_rev;
    else
        return "";
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
}

void beerocks::version::log_version(int, char **argv)
{
    std::string name(get_last_path(argv[0]));

    LOG(INFO) << name << " " << version::get_module_version() << " ("
              << version::get_module_timestamp() << ") [" << version::get_module_revision() << "]";
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
