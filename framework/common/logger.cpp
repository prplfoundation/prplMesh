/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include <json-c/json.h>
#include <mapf/common/logger.h>
#include <pthread.h>
#include <sstream>
#include <sys/inotify.h>
#include <unistd.h>

//SHARE_EASYLOGGINGPP(
//    el::Helpers::
//        storage()) //use easylogging resources of the main process (assuming this process use INITIALIZE_EASYLOGGINGPP)

#define DEFAULT_LOGGER_NAME "default"

namespace mapf {
static const char *get_name(const el::LogMessage *message) //for easylogging use
{
    return mapf::Logger::Instance().logger_name();
}

static inline std::string const bool_to_string(bool b)
{
    return b ? std::string("TRUE") : std::string("FALSE");
}

#ifdef USE_INOTIFY
static void *watch_log_file(void *args)
{
    const char *logger_name = (const char *)args;
    int fd = -1, length = -1, i = 0, ret = 0;
    int event_buf_len = (sizeof(struct inotify_event) + 16) * 1024;
    char buffer[event_buf_len];
    struct inotify_event *fileEvent;

    fd = inotify_init();
    if (fd < 0) {
        return nullptr;
    }

    ret = inotify_add_watch(fd, std::string(CONF_FILE_PATH).c_str(),
                            IN_MODIFY | IN_CLOSE_WRITE | IN_IGNORED | IN_DELETE_SELF);
    if (ret < 0) {
        return nullptr;
    }

    while (1) {
        length = read(fd, buffer, event_buf_len);
        if (length < 0) {
            MAPF_ERR("can't watch logger configuration file");
        } else {
            while (i < length) {
                fileEvent = (struct inotify_event *)&buffer[i];
                if (fileEvent->mask) {
                    if ((fileEvent->mask & IN_IGNORED) || (fileEvent->mask & IN_DELETE_SELF)) {
                        inotify_add_watch(fd, std::string(CONF_FILE_PATH).c_str(),
                                          IN_MODIFY | IN_CLOSE_WRITE | IN_IGNORED | IN_DELETE_SELF);
                    }
                    sleep(1);
                    Logger::Instance().LoggerConfig(logger_name);
                }
                i += sizeof(struct inotify_event) + fileEvent->len;
            }
            i = 0;
        }
    }
}
#endif //#ifdef USE_INOTIFY

Logger::Logger()
{
    el::Configurations conf;
    conf.set(el::Level::Global, el::ConfigurationType::Enabled, "true");
    conf.set(el::Level::Global, el::ConfigurationType::ToStandardOutput, "true");
    conf.set(el::Level::Global, el::ConfigurationType::Format,
             "%datetime{%H:%m:%s} %fbase[%line]: %msg");
    el::Loggers::reconfigureLogger(DEFAULT_LOGGER_NAME, conf);
}

void Logger::LoggerInit() { LoggerInit(DEFAULT_LOGGER_NAME); }

//for initial configuration and for default configuring all loggers
void Logger::LoggerInit(const char *logger_name)
{
    if (std::string(logger_name).compare(DEFAULT_LOGGER_NAME) != 0) //string are not equal
        logger_name_ = logger_name;
    else
        logger_name_ = std::string(__progname);

    LoggerConfig(logger_name);
    static bool init_performed = false;
    if (!init_performed) {
        el::Helpers::installCustomFormatSpecifier(el::CustomFormatSpecifier("%proc", get_name));
#ifdef USE_INOTIFY
        init_performed = true;
        pthread_t watchThread;
        int rc = pthread_create(&watchThread, NULL, watch_log_file, (void *)logger_name);
        if (rc)
            MAPF_ERR("Unable to create watcher thread");
#endif //#ifdef USE_INOTIFY

#ifdef ELPP_SYSLOG
        el::Configurations syslogConf;
        syslogConf.setToDefault();
        syslogConf.parseFromText(
            std::string(std::string("*GLOBAL:\n FORMAT = ") + std::string(kSyslogMessageFormat))
                .c_str());
        el::Loggers::reconfigureLogger("syslog", syslogConf);
#endif //#ifdef ELPP_SYSLOG
    }
}

void Logger::LoggerConfig() { LoggerConfig(DEFAULT_LOGGER_NAME); }

//apply config file configuration all loggers
void Logger::LoggerConfig(const char *logger_name)
{
    Logger::Config cfg;
    std::string conf_path_tmp = CONF_FILE_TMP_PATH;
    std::string conf_path     = CONF_FILE_PATH;

    // first look for conf in tmp, since some systems are read only
    if (cfg.SetValuesFromJson(conf_path_tmp, logger_name) < 0) {
        cfg.SetValuesFromJson(conf_path, logger_name);
    }

    LoggerConfig(cfg);
}

//for configuring a specific logger (or a new one) with a custom configuration
void Logger::LoggerConfig(Logger::Config &cfg)
{
    el::Configurations conf;
    conf.parseFromText(cfg.ToEasyLoggingString().c_str());
    el::Loggers::reconfigureLogger(DEFAULT_LOGGER_NAME, conf);
}

int Logger::Config::SetValuesFromJson(std::string file_path, std::string logger_name)
{
    struct json_object *jobj = json_object_from_file(file_path.c_str());
    if (!jobj) {
        MAPF_DBG("Configuration file does not exist");
        return -1;
    }
    struct json_object *jlogger;
    if (json_object_object_get_ex(jobj, "logging", &jobj)) {
        if (json_object_object_get_ex(jobj, std::string(DEFAULT_LOGGER_NAME).c_str(), &jlogger)) {
            SetValuesFromJson(jlogger, logger_name);
        }

        if (std::string(logger_name).compare(DEFAULT_LOGGER_NAME) != 0) //string are not equal
        {
            if (json_object_object_get_ex(jobj, logger_name.c_str(), &jlogger)) {
                SetValuesFromJson(jlogger, logger_name);
            }
        }
    }
    return 0;
}

void Logger::Config::SetValuesFromJson(struct json_object *jlogger, std::string logger_name)
{
    struct json_object *jfile, *jtmp;
    if (json_object_object_get_ex(jlogger, "level", &jtmp)) {
        level_ = json_object_get_string(jtmp);
    }
    if (json_object_object_get_ex(jlogger, "syslog", &jtmp)) {
        write_to_syslog_ = json_object_get_boolean(jtmp);
    }
    if (json_object_object_get_ex(jlogger, "console", &jtmp)) {
        write_to_console_ = json_object_get_boolean(jtmp);
    }
    if (json_object_object_get_ex(jlogger, "flushthreshold", &jtmp)) {
        log_flush_threshold_ = json_object_get_int(jtmp);
    }
    if (json_object_object_get_ex(jlogger, "file", &jfile)) {
        if (json_object_object_get_ex(jfile, "enable", &jtmp)) {
            write_to_file_ = json_object_get_boolean(jtmp);
        }
        if (json_object_object_get_ex(jfile, "path", &jtmp)) {
            file_path_ = json_object_get_string(jtmp);
            file_path_ += logger_name;
            file_path_ += ".log";
        }
        if (json_object_object_get_ex(jfile, "size", &jtmp)) {
            max_file_size_ = json_object_get_int(jtmp);
        }
    }
}

std::string Logger::Config::ToEasyLoggingString()
{
    std::string settings("*GLOBAL:\n");
    settings += std::string("FORMAT = ") + kMessageFormat + "\n";
    settings += "FILENAME = " + file_path_ + "\n";
    settings += "ENABLED = " + bool_to_string(false) + "\n";
    settings += "TO_FILE = " + bool_to_string(write_to_file_) + "\n";
    settings += "TO_STANDARD_OUTPUT = " + bool_to_string(write_to_console_) + "\n";
    settings += "MAX_LOG_FILE_SIZE = " + std::to_string(max_file_size_) + "\n";
    settings += "LOG_FLUSH_THRESHOLD = " + std::to_string(log_flush_threshold_) + "\n";
#ifdef ELPP_SYSLOG //if TO_SYSLOG isn't configured
    settings += "TO_SYSLOG = " + bool_to_string(write_to_syslog_) + "\n";
#endif

    if (level_.compare("DEBUG") == 0) {
        settings += "*DEBUG:\nENABLED = " + bool_to_string(true) + "\n";
        settings += "*INFO:\nENABLED = " + bool_to_string(true) + "\n";
        settings += "*WARNING:\nENABLED = " + bool_to_string(true) + "\n";
        settings += "*ERROR:\nENABLED = " + bool_to_string(true) + "\n";
    } else if (level_.compare("INFO") == 0) {
        settings += "*INFO:\nENABLED = " + bool_to_string(true) + "\n";
        settings += "*WARNING:\nENABLED = " + bool_to_string(true) + "\n";
        settings += "*ERROR:\nENABLED = " + bool_to_string(true) + "\n";
    } else if (level_.compare("WARNING") == 0) {
        settings += "*WARNING:\nENABLED = " + bool_to_string(true) + "\n";
        settings += "*ERROR:\nENABLED = " + bool_to_string(true) + "\n";
    } else if (level_.compare("ERROR") == 0) {
        settings += "*ERROR:\nENABLED = " + bool_to_string(true) + "\n";
    } else // if invalid level, allow everything
    {
        settings += "*DEBUG:\nENABLED = " + bool_to_string(true) + "\n";
        settings += "*INFO:\nENABLED = " + bool_to_string(true) + "\n";
        settings += "*WARNING:\nENABLED = " + bool_to_string(true) + "\n";
        settings += "*ERROR:\nENABLED = " + bool_to_string(true) + "\n";
    }

    return settings;
}
} // namespace mapf
