#include "bpl_db_uci.h"


namespace beerocks {
namespace bpl {
namespace db {

bool uci_entry_exists(const std::string &config_file, const std::string &entry_type,
                      const std::string &entry_name)
{
    //entry: config_file: entry_type(entry_name)
    LOG(TRACE) << "uci_entry_exists() "
               << "entry: " << config_file << ": " << entry_type << "(" << entry_name << ")";

    return true;
}

bool uci_add_entry(const std::string &config_file, const std::string &entry_type,
                   const std::string &entry_name)
{
    //entry: config_file: entry_type(entry_name)
    LOG(TRACE) << "uci_add_entry() "
               << "entry: " << config_file << ": " << entry_type << "(" << entry_name << ")";

    return true;
}

bool uci_set_entry(const std::string &config_file, const std::string &entry_type,
                   const std::string &entry_name,
                   const std::unordered_map<std::string, std::string> &params)
{
    //entry: config_file: entry_type(entry_name)
    LOG(TRACE) << "uci_set_entry() "
               << "entry: " << config_file << ": " << entry_type << "(" << entry_name << ")";

    return true;
}

bool uci_get_entry(const std::string &config_file, const std::string &entry_type,
                   const std::string &entry_name,
                   std::unordered_map<std::string, std::string> &params)
{
    //entry: config_file: entry_type(entry_name)
    LOG(TRACE) << "uci_get_entry() "
               << "entry: " << config_file << ": " << entry_type << "(" << entry_name << ")";

    return true;
}

bool uci_get_entry_type(const std::string &config_file, const std::string &entry_name,
                        std::string &entry_type)
{
    //entry: config_file: entry_name
    LOG(TRACE) << "uci_get_entry_type() "
               << "entry: " << config_file << ": " << entry_name;

    return true;
}

bool uci_get_param(const std::string &config_file, const std::string &entry_type,
                   const std::string &entry_name, const std::string &param_name, std::string &value)
{
    //entry: config_file: entry_type(entry_name), param
    LOG(TRACE) << "uci_get_param() "
               << "entry: " << config_file << ": " << entry_type << "(" << entry_name << ")"
               << ", " << param_name;

    return true;
}

bool uci_delete_entry(const std::string &config_file, const std::string &entry_type,
                      const std::string &entry_name)
{
    //entry: config_file: entry_type(entry_name)
    LOG(TRACE) << "uci_delete_entry() "
               << "entry: " << config_file << ": " << entry_type << "(" << entry_name << ")";

    return true;
}

bool uci_get_all_entries(const std::string &config_file, const std::string &entry_type,
                         std::vector<std::string> &entries)
{
    //entry: config_file: entry_type
    LOG(TRACE) << "uci_get_all_entries() "
               << "entry: " << config_file << ": " << entry_type;

    return true;
}

} // namespace db
} // namespace bpl
} // namespace beerocks
