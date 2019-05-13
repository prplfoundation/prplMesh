/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include "../include/beerocks/bcl/beerocks_version.h"
#include "../include/beerocks/bcl/beerocks_utils.h"

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

#ifdef BEEROCKS_UGW
        std::cout << std::endl << "UGW Version: " << version::get_ugw_version() << std::endl;
        std::cout << "WAVE Version: " << version::get_wave_version() << std::endl;
        std::cout << "Bootloader Version: " << version::get_bootloader_version() << std::endl;
        std::cout << "Platform: " << version::get_cpuid() << std::endl;
#endif
        std::cout << "Kernel Version: " << version::get_kernel_version() << std::endl;
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

#ifdef BEEROCKS_UGW
    LOG(INFO) << "UGW Version: " << version::get_ugw_version();
    LOG(INFO) << "WAVE Version: " << version::get_wave_version();
    LOG(INFO) << "Bootloader Version: " << version::get_bootloader_version();
    LOG(INFO) << "Platform: " << version::get_cpuid();
#endif
    LOG(INFO) << "Kernel Version: " << version::get_kernel_version();
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

std::string beerocks::version::get_bootloader_version()
{
    std::string bootloader_version;

    std::string bootloader_file_name = "/proc/cmdline";
    std::ifstream bootloader_file(bootloader_file_name);

    if (bootloader_file.fail()) {
        LOG(INFO) << "get_bootloader_version failed reading /proc/cmdline.";
        return bootloader_version;
    }

    std::string bootloader_version_raw;
    std::getline(bootloader_file, bootloader_version_raw);

    std::string uboottok("ubootver=");
    auto start_ver = bootloader_version_raw.find(uboottok);
    if (std::string::npos == start_ver) {
        uboottok  = "BOOT_IMAGE=";
        start_ver = bootloader_version_raw.find(uboottok);
    }
    if (std::string::npos == start_ver) {
        LOG(ERROR) << "get_bootloader_version failed to find ubootver.";
        return bootloader_version;
    }

    auto end_ver = bootloader_version_raw.find_first_of(" \t\n", start_ver + uboottok.length());
    if (std::string::npos == end_ver) {
        bootloader_version = bootloader_version_raw.substr(start_ver + uboottok.length());
    } else {
        bootloader_version = bootloader_version_raw.substr(
            start_ver + uboottok.length(), end_ver - (start_ver + uboottok.length()));
    }

    return bootloader_version;
}

std::string beerocks::version::get_ugw_version()
{
    std::string ugw_version;

    std::string ugw_version_file_name   = "/etc/version";
    std::string ugw_timestamp_file_name = "/etc/timestamp";
    std::ifstream version_file(ugw_version_file_name);
    std::ifstream timestamp_file(ugw_timestamp_file_name);

    if (version_file.fail() || timestamp_file.fail()) {
        LOG(ERROR) << "get_ugw_version failed.";
        return ugw_version;
    }

    std::string version;
    std::string timestamp;
    version_file >> version;
    timestamp_file >> timestamp;
    ugw_version = version + "-" + timestamp;

    return ugw_version;
}

std::string beerocks::version::get_cpuid()
{
    std::string cpuid;
    std::string cpuid_file_name = "/proc/cpuinfo";
    std::ifstream cpuid_file(cpuid_file_name);

    if (cpuid_file.fail()) {
        LOG(ERROR) << "get_cpuid failed to access cpuinfo.";
        return cpuid;
    }

    std::string line;
    std::string model;
    while (std::getline(cpuid_file, line)) {
        auto machine_line = line.find("machine");
        if (std::string::npos != machine_line) {
            auto value_start = line.find(":", 7);
            if (std::string::npos != value_start) {
                cpuid = line.substr(value_start + 2);
                return cpuid;
            }
        }
        if (0 == model.length()) {
            auto model_line = line.find("model name");
            if (std::string::npos != model_line) {
                auto value_start = line.find(":");
                if (std::string::npos != value_start) {
                    model = line.substr(value_start + 2);
                }
            }
        }
    }

    if (0 < model.length()) {
        return model;
    }

    LOG(ERROR) << "failed to parse cpuinfo.";
    return cpuid;
}

std::string beerocks::version::get_wave_version()
{
    std::string wave_version;
    std::string wave_version_file_name = "/etc/wave_components.ver";
    std::ifstream wave_version_file(wave_version_file_name);

    if (wave_version_file.fail()) {
        LOG(ERROR) << "failed to access wave version";
        return wave_version;
    }

    std::string line;
    while (std::getline(wave_version_file, line)) {
        auto wave_version_line = line.find("wave_release_minor");
        if (std::string::npos != wave_version_line) {
            auto value_start = line.find("=");
            if (std::string::npos != value_start) {
                wave_version =
                    line.substr(value_start + 2, ((line.length() - 1) - (value_start + 2)));
                return wave_version;
            }
        }
    }

    return wave_version;
}

std::string beerocks::version::get_kernel_version()
{
    std::string kernel_version;
    std::string kernel_version_file_name = "/proc/sys/kernel/osrelease";
    std::ifstream kernel_version_file(kernel_version_file_name);

    if (kernel_version_file.fail()) {
        LOG(INFO) << "get_kernel_version failed.";
        return kernel_version;
    }

    kernel_version_file >> kernel_version;
    return kernel_version;
}

beerocks::ePlatform beerocks::version::get_platform()
{
    auto cpuid = get_cpuid();
    if (std::string::npos != cpuid.find("GRX350")) {
        return beerocks::PLATFORM_GRX_350;
    }

    auto bootloader = get_bootloader_version();
    if (std::string::npos != bootloader.find("vmlinuz")) {
        return beerocks::PLATFORM_LINUX;
    }

    // TBD: currently defaulting to PLATFORM_GRX_350 everywhere patch this in case
    // there are issues with the new heuristics.
    // return beerocks::PLATFORM_GRX_350;
    return beerocks::PLATFORM_UNSUPPORTED;
}
