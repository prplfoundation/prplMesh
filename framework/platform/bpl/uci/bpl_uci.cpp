#include "bpl/bpl_uci.h"

#include <mapf/common/logger.h>
#include <mapf/common/utils.h>

extern "C" {
#include <uci.h>
}

bool uci_entry_exists(const std::string &config_file, const std::string &entry_type,
                      const std::string &entry_name)
{
    LOG(TRACE) << " - NOT IMPLEMENTED!";

    return false;
}

bool uci_add_entry(const std::string &config_file, const std::string &entry_type,
                   const std::string &entry_name)
{
    LOG(TRACE) << " - NOT IMPLEMENTED!";

    return false;
}

bool uci_set_entry(const std::string &config_file, const std::string &entry_type,
                   const std::string &entry_name,
                   const std::unordered_map<std::string, std::string> &params)
{
    LOG(TRACE) << " - NOT IMPLEMENTED!";

    return false;
}

bool uci_get_entry(const std::string &config_file, const std::string &entry_type,
                   const std::string &entry_name,
                   std::unordered_map<std::string, std::string> &params)
{
    LOG(TRACE) << " - NOT IMPLEMENTED!";

    return false;
}

bool uci_get_entry_type(const std::string &config_file, const std::string &entry_name,
                        std::string &entry_type)
{
    LOG(TRACE) << " - NOT IMPLEMENTED!";

    return false;
}

bool uci_get_param(const std::string &config_file, const std::string &entry_type,
                   const std::string &entry_name, const std::string &param_name, std::string &param)
{
    LOG(TRACE) << " - NOT IMPLEMENTED!";

    return false;
}

bool uci_delete_entry(const std::string &config_file, const std::string &entry_type,
                      const std::string &entry_name)
{
    LOG(TRACE) << " - NOT IMPLEMENTED!";

    return false;
}

bool uci_get_all_entries(const std::string &config_file, const std::string &entry_type,
                         std::vector<std::string> entries)
{
    LOG(TRACE) << " - NOT IMPLEMENTED!";

    return false;
}
