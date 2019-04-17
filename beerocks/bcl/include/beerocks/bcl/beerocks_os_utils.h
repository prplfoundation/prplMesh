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
#define UTILS_SLEEP_MSEC(msec)  Sleep(msec)
#else // Linux
#include <unistd.h>
#define UTILS_SLEEP_MSEC(msec)  usleep(msec*1000)
#endif

#include <string>
#include <set>
#include <map>

namespace beerocks
{
    #define SYSTEM_CALL os_utils::system_call

    class os_utils
    {
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

            static std::string system_call(std::string cmd,  int log_lvl = 0, bool detached=false);

            static void kill_pid(std::string path, std::string file_name);

            static bool is_pid_running(std::string path, std::string file_name, int* pid_out = nullptr);
            
            static bool write_pid_file(std::string path, std::string file_name);

            static bool touch_pid_file(std::string file_path);

            static int redirect_console_std(std::string log_file_name);

            static void close_file(int fd);
    };
} // namespace beerocks


#endif //_BEEROCKS_OS_UTILS_H_
