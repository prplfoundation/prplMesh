/*
* INTEL CONFIDENTIAL
* Copyright 2016 Intel Corporation All Rights Reserved.
*
* The source code contained or described herein and all documents related to
* the source code ("Material") are owned by Intel Corporation or its
* suppliers or licensors.  Title to the Material remains with Intel
* Corporation or its suppliers and licensors.  The Material contains trade
* secrets and proprietary and confidential information of Intel or its
* suppliers and licensors.  The Material is protected by worldwide copyright
* and trade secret laws and treaty provisions. No part of the Material may
* be used, copied, reproduced, modified, published, uploaded, posted,
* transmitted, distributed, or disclosed in any way without Intel's prior
* express written permission.
*
* No license under any patent, copyright, trade secret or other intellectual
* property right is granted to or conferred upon you by disclosure or
* delivery of the Materials,  either expressly, by implication, inducement,
* estoppel or otherwise.  Any license under such intellectual property
* rights must be express and approved by Intel in writing.
*/

#ifndef _BEEROCKS_VERSION_H_
#define _BEEROCKS_VERSION_H_

#include "beerocks_defines.h"
#include <map>
#include <tuple>

namespace beerocks
{

// This will default the version to 0.0.0 if not supplied by the Makefile. This will never
// happen with our current Makefiles as the version gets baked into the command line in
// the base build/common.mk but if this code is ever repurposed this will prevent a compile
// error.
#ifndef COMMON_VERSION
#define COMMON_VERSION "0.0.0"
#endif

#ifndef COMMON_BUILD_DATE
#define COMMON_BUILD_DATE "00/00/00--00:00"
#endif

#ifndef COMMON_REVISION
#define COMMON_REVISION "INVALID"
#endif

// must be included once and only once in file with main()
#define BEEROCKS_INIT_BEEROCKS_VERSION                                          \
    static beerocks::version s_beerocks_version(                                \
        std::string(BEEROCKS_VERSION),                                          \
        std::string(BEEROCKS_BUILD_DATE),                                       \
        std::string(BEEROCKS_REVISION));

#define BEEROCKS_INIT_SO_VERSION(so_name)                                       \
    __attribute__((constructor)) static void beerocks_version_register_module() \
    {                                                                           \
        beerocks::g_beerocks_version_map[so_name] = std::make_tuple(            \
        beerocks::version::set_module_version(std::version(so_name),            \
            std::string(BEEROCKS_VERSION),                                      \
            std::string(BEEROCKS_BUILD_DATE),                                   \
            std::string(BEEROCKS_REVISION));                                    \
    }

typedef struct sBinaryVersion {
    uint8_t major;
    uint8_t minor;
    uint16_t build_number;
} sBinaryVersion;

class version
{
public:

    static constexpr sBinaryVersion INVALID_VERSION = {};
    static constexpr char INVALID_VERSION_STRING[] = "0.0.0";
    static constexpr char INVALID_TIMESTAMP_STRING[] = "00/00/00--00:00";

    // Constructor
    version(std::string ver, std::string build_date, std::string build_rev);

    static void print_version(
        bool verbose, const std::string& name, const std::string& description = std::string());
    static bool handle_version_query(
        int argc, char** argv, const std::string& description = std::string());
    static void log_version(int argc, char** argv);

    static void set_module_version(std::string so_name, std::string ver, std::string build_date, std::string build_rev);
    static std::string get_module_version(const std::string& module_name = std::string("__main__"));
    static std::string get_module_timestamp(const std::string& module_name = std::string("__main__"));
    static std::string get_module_revision(const std::string& module_name = std::string("__main__"));
    static std::string version_to_string(const sBinaryVersion& version);
    static sBinaryVersion version_from_string(const std::string& version);

    static std::string get_bootloader_version();
    static std::string get_ugw_version();
    static std::string get_cpuid();
    static std::string get_kernel_version();
    static std::string get_wave_version();

    static beerocks::ePlatform get_platform();

private:

    // Version, Build Date, GIT Revision
    typedef std::map<std::string, std::tuple<std::string, std::string, std::string>> beerocks_version_map_t;
    static  beerocks_version_map_t s_beerocks_version_map;
    
};

}

#endif  // _BEEROCKS_VERSION_H_
