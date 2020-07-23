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

    auto ctx = alloc_context();
    if (!ctx) {
        return false;
    }

    char path[MAX_UCI_BUF_LEN] = {0};
    if (!compose_path(path, config_file)) {
        LOG(ERROR) << "Failed to compose path";
        return false;
    }

    struct uci_package *pkg = nullptr;
    if (!(pkg = uci_lookup_package(ctx.get(), path))) {
        // Try and load the package file, in case it wasn't loaded before
        uci_load(ctx.get(), path, &pkg);
        if (!pkg) {
            LOG(ERROR) << "uci lookup package failed!" << std::endl << uci_get_error(ctx.get());
            return false;
        }
    }

    bool found = false;
    // Loop through the sections with a matching section name (entry_name)
    struct uci_element *elm = nullptr;
    uci_foreach_element(&pkg->sections, elm)
    {
        found                   = true;
        struct uci_section *sec = uci_to_section(elm);
        // if the entry_type is empty all matching sections are a valid match
        auto name_match = entry_name.compare(sec->e.name) == 0;
        auto type_match = entry_type.empty() || entry_type.compare(sec->type) == 0;
        if (name_match && type_match) {
            LOG(TRACE) << "Found match: " << sec->type << "(" << sec->e.name << ")";
            return true;
        }
    }

    LOG(TRACE) << "No " << entry_name << " entry found"
               << (entry_type.empty() || !found ? "!" : std::string(" with type: ") + entry_type);

    return false;
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
