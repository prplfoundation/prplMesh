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

    auto ctx = alloc_context();
    if (!ctx) {
        return false;
    }

    char pkg_path[MAX_UCI_BUF_LEN] = {0};
    if (!compose_path(pkg_path, config_file)) {
        LOG(ERROR) << "Failed to compose path";
        return false;
    }

    struct uci_package *pkg = nullptr;
    if (!(pkg = uci_lookup_package(ctx.get(), pkg_path))) {
        // Try and load the package file, in case it wasn't loaded before
        uci_load(ctx.get(), pkg_path, &pkg);
        if (!pkg) {
            LOG(ERROR) << "uci lookup package failed!" << std::endl << uci_get_error(ctx.get());
            return false;
        }
    }

    //Create new unnamed section
    struct uci_section *sec = nullptr;
    if (uci_add_section(ctx.get(), pkg, entry_type.c_str(), &sec) != UCI_OK) {
        LOG(ERROR) << "UCI failed to create new entry of type: " << entry_type << std::endl
                   << uci_get_error(ctx.get());
        return false;
    }

    char sec_path[MAX_UCI_BUF_LEN] = {0};
    LOG(TRACE) << "Testing: " << config_file.c_str() << "." << sec->e.name << "="
               << entry_name.c_str();

    if (sprintf(sec_path, "%s.%s=%s", config_file.c_str(), sec->e.name, entry_name.c_str()) <= 0) {
        LOG(ERROR) << "Failed to compose path";
        return false;
    }

    struct uci_ptr sec_ptr;
    // Lookup package to save delta.
    if (uci_lookup_ptr(ctx.get(), &sec_ptr, sec_path, true) != UCI_OK) {
        LOG(ERROR) << "UCI failed to lookup ptr for path: " << sec_path << std::endl
                   << uci_get_error(ctx.get());
        return false;
    }

    // Rename section
    if (uci_rename(ctx.get(), &sec_ptr) != UCI_OK) {
        LOG(ERROR) << "UCI failed to rename section!" << std::endl << uci_get_error(ctx.get());
        return false;
    }

    // Create delta from changes, this does not change the persistent file.
    if (uci_save(ctx.get(), pkg) != UCI_OK) {
        LOG(ERROR) << "Failed to save changes!" << std::endl << uci_get_error(ctx.get());
        return false;
    }

    // Commit changes to file
    if (uci_commit(ctx.get(), &pkg, false) != UCI_OK) {
        LOG(ERROR) << "Failed to commit changes!" << std::endl << uci_get_error(ctx.get());
        return false;
    }

    return true;
}

bool uci_set_entry(const std::string &config_file, const std::string &entry_type,
                   const std::string &entry_name,
                   const std::unordered_map<std::string, std::string> &params)
{
    //entry: config_file: entry_type(entry_name)
    LOG(TRACE) << "uci_set_entry() "
               << "entry: " << config_file << ": " << entry_type << "(" << entry_name << ")";

    auto ctx = alloc_context();
    if (!ctx) {
        return false;
    }

    for (auto &param : params) {
        char opt_path[MAX_UCI_BUF_LEN] = {0};
        if (!compose_path(opt_path, config_file, entry_name, param.first)) {
            LOG(ERROR) << "Failed to compose path";
            return false;
        }

        LOG(TRACE) << "Trying to lookup: " << opt_path;

        struct uci_ptr opt_ptr;
        // Get pointer to value
        if (uci_lookup_ptr(ctx.get(), &opt_ptr, opt_path, true) != UCI_OK) {
            LOG(ERROR) << "UCI failed to lookup ptr for path: " << opt_path << std::endl
                       << uci_get_error(ctx.get());
            return false;
        }

        //Set the value to the pointer
        LOG(TRACE) << "Setting " << param.first << "=" << param.second;
        opt_ptr.value = strdup(param.second.c_str());

        /*
            If the value is set to empty the option will be removed
            If the option does not exist it will be created
            If the option does exist the value will be overridden
        */
        if (uci_set(ctx.get(), &opt_ptr) != UCI_OK) {
            LOG(ERROR) << "Failed to set option " << param.first;
            return false;
        } else {
            // Create delta from changes, this does not change the persistent file.
            if (uci_save(ctx.get(), opt_ptr.p) != UCI_OK) {
                LOG(ERROR) << "Failed to save changes!" << std::endl << uci_get_error(ctx.get());
                return false;
            }
        }
    }
    LOG(TRACE) << "Finished setting parameters. about to commit changes";

    char pkg_path[MAX_UCI_BUF_LEN] = {0};
    if (!compose_path(pkg_path, config_file)) {
        LOG(ERROR) << "Failed to compose path";
        return false;
    }
    struct uci_ptr pkg_ptr;
    // Lookup package to save delta.
    if (uci_lookup_ptr(ctx.get(), &pkg_ptr, pkg_path, true) != UCI_OK) {
        LOG(ERROR) << "UCI failed to lookup ptr for path: " << pkg_path << std::endl
                   << uci_get_error(ctx.get());
        return false;
    }

    // Commit changes to file
    if (uci_commit(ctx.get(), &pkg_ptr.p, false) != UCI_OK) {
        LOG(ERROR) << "Failed to commit changes!" << std::endl << uci_get_error(ctx.get());
        return false;
    }
    return true;
}

bool uci_get_entry(const std::string &config_file, const std::string &entry_type,
                   const std::string &entry_name,
                   std::unordered_map<std::string, std::string> &params)
{
    //entry: config_file: entry_type(entry_name)
    LOG(TRACE) << "uci_get_entry() "
               << "entry: " << config_file << ": " << entry_type << "(" << entry_name << ")";

    auto ctx = alloc_context();
    if (!ctx) {
        return false;
    }

    char path[MAX_UCI_BUF_LEN] = {0};
    if (!compose_path(path, config_file, entry_name)) {
        LOG(ERROR) << "Failed to compose path";
        return false;
    }

    struct uci_ptr ptr;
    if (uci_lookup_ptr(ctx.get(), &ptr, path, true) != UCI_OK || !ptr.s) {
        LOG(ERROR) << "UCI failed to lookup ptr for path: " << path << std::endl
                   << uci_get_error(ctx.get());
        return false;
    }

    // Loop through the option within the found section
    struct uci_element *elm = nullptr;
    uci_foreach_element(&ptr.s->options, elm)
    {
        struct uci_option *opt = uci_to_option(elm);
        // Only UCI_TYPE_STRING is supported
        if (opt->type == UCI_TYPE_STRING) {
            params[std::string(opt->e.name)] = opt->v.string;
        }
    }

    return true;
}

bool uci_get_entry_type(const std::string &config_file, const std::string &entry_name,
                        std::string &entry_type)
{
    //entry: config_file: entry_name
    LOG(TRACE) << "uci_get_entry_type() "
               << "entry: " << config_file << ": " << entry_name;

    auto ctx = alloc_context();
    if (!ctx) {
        return false;
    }

    char path[MAX_UCI_BUF_LEN] = {0};
    if (!compose_path(path, config_file, entry_name)) {
        LOG(ERROR) << "Failed to compose path";
        return false;
    }

    struct uci_ptr ptr;
    if (uci_lookup_ptr(ctx.get(), &ptr, path, true) != UCI_OK || !ptr.s) {
        LOG(ERROR) << "UCI failed to lookup ptr for path: " << path << std::endl
                   << uci_get_error(ctx.get());
        return false;
    }

    entry_type = ptr.s->type;
    return true;
}

bool uci_get_param(const std::string &config_file, const std::string &entry_type,
                   const std::string &entry_name, const std::string &param_name, std::string &value)
{
    //entry: config_file: entry_type(entry_name), param
    LOG(TRACE) << "uci_get_param() "
               << "entry: " << config_file << ": " << entry_type << "(" << entry_name << ")"
               << ", " << param_name;

    if (!uci_entry_exists(config_file, entry_type, entry_name)) {
        LOG(ERROR) << "Entry " << entry_name << " of type " << entry_type << " was not found!";
        return false;
    }

    auto ctx = alloc_context();
    if (!ctx) {
        return false;
    }

    char path[MAX_UCI_BUF_LEN] = {0};
    if (!compose_path(path, config_file, entry_name, param_name)) {
        LOG(ERROR) << "Failed to compose path";
        return false;
    }

    struct uci_ptr ptr;
    if (uci_lookup_ptr(ctx.get(), &ptr, path, true) != UCI_OK || !ptr.o) {
        LOG(ERROR) << "UCI failed to lookup ptr for path: " << path << std::endl
                   << uci_get_error(ctx.get());
        return false;
    }

    value = ptr.o->v.string;
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
