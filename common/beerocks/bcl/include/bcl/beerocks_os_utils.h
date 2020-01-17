/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _BEEROCKS_OS_UTILS_H_
#define _BEEROCKS_OS_UTILS_H_

#ifdef __ANDROID__
////
#ifndef IS_ANDROID
#define IS_ANDROID
#endif
////
#else
////
#if __linux
#elif __unix
#elif __posix
#else
////
#ifndef IS_WINDOWS
#define IS_WINDOWS
#endif
////
#endif
////
#ifndef IS_WINDOWS
#ifndef IS_LINUX
#define IS_LINUX
#endif
#endif
////
#endif

#ifdef IS_WINDOWS
#ifndef snprintf
#define snprintf _snprintf
#endif
#define UTILS_SLEEP_MSEC(msec) Sleep(msec)
#else // Linux
#include <unistd.h>
#define UTILS_SLEEP_MSEC(msec) usleep(msec * 1000)
#endif

#include <map>
#include <set>
#include <string>

namespace beerocks {
#define SYSTEM_CALL os_utils::system_call

class os_utils {
public:
    ///
    /// @brief Function to get current process executable path
    ///
    /// @return string containing executable binary path location.
    ///     if information can't be acquired.
    ///
    static std::string get_process_path();

    ///
    /// @brief Function to get current process directory
    ///
    /// @return string containing the directory where the executable binary is located.
    ///
    static std::string get_process_dir();

    ///
    /// @brief Function that checks if file exists
    ///
    /// @return boolean that is true if the file exists and false otherwise.
    ///
    static bool file_exists(const std::string &fname);

    static std::string system_call(std::string cmd, int log_lvl = 0, bool detached = false);

    static void kill_pid(std::string path, std::string file_name);

    static bool is_pid_running(std::string path, std::string file_name, int *pid_out = nullptr);

    static bool write_pid_file(std::string path, std::string file_name);

    static bool touch_pid_file(std::string file_path);

    static int redirect_console_std(std::string log_file_name);

    static void close_file(int fd);
};
} // namespace beerocks

#endif //_BEEROCKS_OS_UTILS_H_
