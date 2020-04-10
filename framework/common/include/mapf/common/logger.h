/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2019 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef __MAPF_COMMON_LOGGER_H__
#define __MAPF_COMMON_LOGGER_H__

#include <easylogging++.h>
#include <iomanip> //for resetiosflags
#include <mapf/common/config.h>

#define MAPF_ERR(msg) LOG(ERROR) << msg << resetiosflags((std::ios_base::fmtflags)0xFFFF)
#define MAPF_WARN(msg) LOG(WARNING) << msg << resetiosflags((std::ios_base::fmtflags)0xFFFF)
#define MAPF_INFO(msg) LOG(INFO) << msg << resetiosflags((std::ios_base::fmtflags)0xFFFF)
#define MAPF_DBG(msg) LOG(DEBUG) << msg << resetiosflags((std::ios_base::fmtflags)0xFFFF)

#define MAPF_ERR_IF(cond, msg)                                                                     \
    LOG_IF(cond, ERROR) << msg << resetiosflags((std::ios_base::fmtflags)0xFFFF)
#define MAPF_WARN_IF(cond, msg)                                                                    \
    LOG_IF(cond, WARNING) << msg << resetiosflags((std::ios_base::fmtflags)0xFFFF)
#define MAPF_INFO_IF(cond, msg)                                                                    \
    LOG_IF(cond, INFO) << msg << resetiosflags((std::ios_base::fmtflags)0xFFFF)
#define MAPF_DBG_IF(cond, msg)                                                                     \
    LOG_IF(cond, DEBUG) << msg << resetiosflags((std::ios_base::fmtflags)0xFFFF)

#define CONF_FILE_PATH std::string(MAPF_ROOT) + "/share/logging.conf"
#define CONF_FILE_TMP_PATH std::string("/tmp/share/logging.conf")

extern char *__progname;
struct json_object;
namespace mapf {
class Logger {
public:
    class Config {
    public:
        Config() {}
        void set_level(std::string param) { level_ = param; }
        void set_file_path(std::string param) { file_path_ = param; }
        void set_write_to_syslog(bool param) { write_to_syslog_ = param; }
        void set_write_to_console(bool param) { write_to_console_ = param; }
        void set_write_to_file(bool param) { write_to_file_ = param; }
        void set_max_file_size(size_t param) { max_file_size_ = param; }
        void set_log_flush_threshold(size_t param) { log_flush_threshold_ = param; }

        std::string level() { return level_; }
        std::string file_path() { return file_path_; }
        bool write_to_syslog() { return write_to_syslog_; }
        bool write_to_console() { return write_to_console_; }
        bool write_to_file() { return write_to_file_; }
        size_t max_file_size() { return max_file_size_; }
        size_t log_flush_threshold() { return log_flush_threshold_; }
        int SetValuesFromJson(std::string file_path, std::string logger_name);
        std::string ToEasyLoggingString();

    private:
        std::string level_ = "DEBUG", file_path_ = "logs.log";
        bool write_to_syslog_ = false, write_to_console_ = true, write_to_file_ = false;
        size_t max_file_size_ = 1024, log_flush_threshold_ = 100;
        const char *kMessageFormat = "%datetime{%H:%m:%s} [%proc] [%level] %fbase[%line]: %msg";
        void SetValuesFromJson(struct json_object *jlogger, std::string logger_name);
    };

public:
    static Logger &Instance()
    {
        static Logger instance;
        return instance;
    }
    void LoggerInit();
    void LoggerInit(const char *logger_name);
    void LoggerConfig();
    void LoggerConfig(const char *logger_name);
    void LoggerConfig(Logger::Config &cfg);
    const char *logger_name() { return logger_name_.c_str(); } //for all API users

private:
    std::string logger_name_         = "";
    const char *kSyslogMessageFormat = "[mapf] [%proc] %fbase[%line]: %msg";
    Logger();
    Logger(const Logger &) = delete;
    void operator=(const Logger &) = delete;
};
} // namespace mapf

#endif /* #ifndef __MAPF_COMMON_LOGGER_H__ */
