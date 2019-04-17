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

#ifndef _BEEROCKS_STRING_UTILS_H_
#define _BEEROCKS_STRING_UTILS_H_

#include <stdint.h>
#include <string.h>

#include <cstddef>
#include <string>
#include <vector>

namespace beerocks
{
    class string_utils
    {
        public:
            ///
            /// @brief trim whitespace in place from the head of a string.
            ///
            static void ltrim(std::string&, std::string additional_chars = std::string());

            ///
            /// @brief trim whitespace in place from the tail of a string.
            ///
            static void rtrim(std::string&, std::string additional_chars = std::string());

            ///
            /// @brief trim whitespace in place from the head and tail of a string.
            ///
            static void trim(std::string&, std::string additional_chars = std::string());

            ///
            /// @brief get a head trimmed substring from provided string
            ///
            /// @return string the resulting trimmed substring
            ///
            static std::string ltrimmed_substr(const std::string&);

            ///
            /// @brief get a tail trimmed substring from provided string
            ///
            /// @return string the resulting trimmed substring
            ///
            static std::string rtrimmed_substr(const std::string&);

            ///
            /// @brief get a head and tail trimmed substring from provided string
            ///
            /// @return string the resulting trimmed substring
            ///
            static std::string trimmed_substr(const std::string&);

            ///
            /// @brief get a head and tail trimmed substring from provided string
            ///
            /// @return string representation of the boolean value ["true" | "false"]
            ///
            static std::string bool_str(bool val);

            static void copy_string(char *dst, const char *src, size_t dst_len);

            static std::vector<std::string> str_split(const std::string &s, char delim);

            static int64_t stoi(std::string str, const char* calling_file = __builtin_FILE() , int calling_line = __builtin_LINE());

            static std::string int_to_hex_string(const unsigned int integer, const uint8_t number_of_digits);
            
    };
} // namespace beerocks

#endif //_BEEROCKS_STRING_UTILS_H_