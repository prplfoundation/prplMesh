#include "bpl_db_uci.h"

#include <mapf/common/logger.h>
#include <mapf/common/utils.h>

extern "C" {
#include <uci.h>
}

namespace beerocks {
namespace bpl {
namespace db {

constexpr ssize_t MAX_UCI_BUF_LEN = 64;

std::string uci_get_error(struct uci_context *ctx)
{
    char *err_buf;
    uci_get_errorstr(ctx, &err_buf, "");
    return std::string(err_buf);
}

std::shared_ptr<uci_context> alloc_context()
{
    auto ctx = std::shared_ptr<uci_context>(uci_alloc_context(), [](uci_context *ctx) {
        if (ctx) {
            uci_free_context(ctx);
        }
    });

    LOG_IF(!ctx, ERROR) << "Failed allocating UCI context!";
    return ctx;
}

bool compose_path(char *path, const std::string &config_file)
{
    return sprintf(path, "%s", config_file.c_str()) > 0;
}
bool compose_path(char *path, const std::string &config_file, const std::string &entry_name)
{
    return sprintf(path, "%s.%s", config_file.c_str(), entry_name.c_str()) > 0;
}
bool compose_path(char *path, const std::string &config_file, const std::string &entry_name,
                  const std::string &param_name)
{
    return sprintf(path, "%s.%s.%s", config_file.c_str(), entry_name.c_str(), param_name.c_str()) >
           0;
}
bool compose_path(char *path, const std::string &config_file, const std::string &entry_name,
                  const std::string &param_name, const std::string &param_value)
{
    return sprintf(path, "%s.%s.%s=%s", config_file.c_str(), entry_name.c_str(), param_name.c_str(),
                   param_value.c_str()) > 0;
}

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
