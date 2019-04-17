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

#ifndef _BEEROCKS_LOGGING_H_
#define _BEEROCKS_LOGGING_H_

#include <string>
#include <set>
#include <map>

#include "beerocks_string_utils.h"
#include "beerocks_config_file.h"
#include "beerocks_defines.h"

namespace beerocks
{
  #define CONSOLE_MSG(a) do { std::cout << a << "\r\n" << std::flush; } while(0)
  #define CONSOLE_MSG_INPLACE(a) do { std::cout << "\r" << a << std::flush; } while(0)

  extern const std::string BEEROCKS_LOGGING_MODULE_NAME;
  #define BEEROCKS_INIT_LOGGING(module_name) \
      const std::string beerocks::BEEROCKS_LOGGING_MODULE_NAME = (module_name);

  class log_levels
  {
    public:
      typedef std::set<std::string> set_t;

      log_levels() = default;
      ~log_levels() = default;
      log_levels(const std::set<std::string>& log_levels);
      log_levels(const std::string& log_level_str);
      log_levels& operator=(const log_levels& rhs);
      log_levels& operator=(const std::string& log_level_str);
      log_levels operator&(const log_levels& rhs);

      std::string to_string();
      void parse_string(const std::string& log_level_str);

      bool is_all();
      bool is_off();

      bool fatal_enabled();
      bool error_enabled();
      bool warning_enabled();
      bool info_enabled();
      bool debug_enabled();
      bool trace_enabled();

      void set_log_level_state(const eLogLevel& log_level, const bool &new_state);

    private:
      std::set<std::string> m_level_set;
  };

  extern const log_levels LOG_LEVELS_ALL;
  extern const log_levels LOG_LEVELS_OFF;
  extern const log_levels LOG_LEVELS_GLOBAL_DEFAULT;
  extern const log_levels LOG_LEVELS_MODULE_DEFAULT;
  extern const log_levels LOG_LEVELS_SYSLOG_DEFAULT;

  class logging
  {
    public:
      typedef std::map<std::string, std::string> settings_t;

      logging(const std::string config_path = std::string(),
          std::string module_name = BEEROCKS_LOGGING_MODULE_NAME);
      logging(const settings_t& settings, bool cache_settings = false,
          std::string module_name = BEEROCKS_LOGGING_MODULE_NAME);
      logging(const beerocks::config_file::SConfigLog& settings, std::string module_name, 
              bool cache_settings = false);
      ~logging() = default;

      void apply_settings();

      std::string get_module_name();
      std::string get_log_path();
      std::string get_log_filepath();
      std::string get_log_filename();
      std::string get_log_max_size_setting();
      log_levels get_log_levels();
      log_levels get_syslog_levels();

      void set_log_level_state(const eLogLevel& log_level, const bool& new_state);

      // TBD: Can/Should these be removed?
      size_t get_log_rollover_size();
      size_t get_log_max_size();

    protected:
      bool load_settings(const std::string& config_file_path);
      std::string get_config_path(std::string config_path);
      std::string get_cache_path();
      bool save_settings(const std::string& config_file_path);

      void set_log_path(std::string log_path);
      void eval_settings();

      static void handle_logging_rollover(const char*, std::size_t);

    private:
      static const std::string format;

      std::string m_module_name;

      size_t m_logfile_size;
      std::string m_log_path;
      std::string m_log_filename;
      log_levels m_levels;
      log_levels m_syslog_levels;
      std::string m_netlog_host;
      uint16_t m_netlog_port;

      settings_t m_settings_map;
  };

}  // namespace beerocks

#endif  // _BEEROCKS_LOGGING_H_
