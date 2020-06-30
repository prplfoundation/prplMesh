#include "bpl/bpl_uci.h"

#include <mapf/common/logger.h>
#include <mapf/common/utils.h>

extern "C" {
#include <uci.h>
}

bool uci_entry_exists(const std::string &config_file, const std::string &entry_type,
                      const std::string &entry_name)
{
    //entry: config_file: entry_type(entry_name)
    LOG(TRACE) << "entry: " << config_file << ": " << entry_type << "(" << entry_name << ")";

    struct uci_context *ctx = nullptr;
    if (!(ctx = uci_alloc_context())) {
        LOG(ERROR) << "uci context allocate failed!";
        return false;
    }

    struct uci_package *pkg = nullptr;
    if (!(pkg = uci_lookup_package(ctx, config_file.c_str()))) {
        LOG(ERROR) << "uci lookup package failed!";
        uci_free_context(ctx);
        return false;
    }

    // Loop through the sections with a matching section name (entry_name)
    struct uci_element *elm = nullptr;
    while (uci_lookup_next(ctx, &elm, &pkg->sections, entry_name) == UCI_OK) {
        struct uci_section *sec = uci_to_section(elm);
        // if the entry_type is empty all matching sections are a valid match
        // str == char[] uses std::string::compare(char[])
        if (entry_type.empty() || entry_type == sec->type) {
            uci_free_context(ctx);
            return true;
        }
    }

    LOG(TRACE) << "No " << entry_name << " entry found" << entry_type.empty()
        ? "!"
        : " with type: " << entry_type;
    uci_free_context(ctx);
    return false;
}

bool uci_add_entry(const std::string &config_file, const std::string &entry_type,
                   const std::string &entry_name)
{
    //entry: config_file: entry_type(entry_name)
    LOG(TRACE) << "entry: " << config_file << ": " << entry_type << "(" << entry_name << ")";

    if (uci_entry_exists(config_file, entry_type, entry_name)) {
        LOG(TRACE) << "uci entry already exists!";
        return true;
    }

    struct uci_context *ctx = nullptr;
    if (!(ctx = uci_alloc_context())) {
        LOG(ERROR) << "uci context allocate failed!";
        return false;
    }

    struct uci_ptr ptr;
    ptr.package = config_file.c_str();
    ptr.section = entry_name.c_str();
    ptr.option  = nullptr;
    ptr.value   = entry_type.c_str();

    if (uci_set(ctx, &ptr) != UCI_OK) {
        LOG(ERROR) << "Failed to create entry!";
        uci_free_context(ctx);
        return false;
    }

    return true;
}

bool uci_set_entry(const std::string &config_file, const std::string &entry_type,
                   const std::string &entry_name,
                   const std::unordered_map<std::string, std::string> &params)
{
    //entry: config_file: entry_type(entry_name)
    LOG(TRACE) << "entry: " << config_file << ": " << entry_type << "(" << entry_name << ")";

    if (!uci_entry_exists(config_file, entry_type, entry_name)) {
        LOG(TRACE) << "uci entry does not exists!";
        return true;
    }

    struct uci_context *ctx = nullptr;
    if (!(ctx = uci_alloc_context())) {
        LOG(ERROR) << "uci context allocate failed!";
        return false;
    }

    for (auto &param : params) {
        struct uci_ptr ptr;
        ptr.package = config_file.c_str();
        ptr.section = entry_name.c_str();
        ptr.option  = param.first.c_str();  //option
        ptr.value   = param.second.c_str(); //value

        if (uci_set(ctx, &ptr) != UCI_OK) {
            LOG(ERROR) << "Failed to set option " << param.first << "!";
            uci_free_context(ctx);
            return false;
        }
    }

    return false;
}

bool uci_get_entry(const std::string &config_file, const std::string &entry_type,
                   const std::string &entry_name,
                   std::unordered_map<std::string, std::string> &params)
{
    //entry: config_file: entry_type(entry_name)
    LOG(TRACE) << "entry: " << config_file << ": " << entry_type << "(" << entry_name << ")";

    struct uci_context *ctx = nullptr;
    if (!(ctx = uci_alloc_context())) {
        LOG(ERROR) << "uci context allocate failed!";
        return false;
    }

    struct uci_ptr ptr = nullptr;
    auto &path         = std::format("{}.{}", config_file, entry_name);
    if (uci_lookup_ptr(cont, &ptr, path.c_str(), true) != UCI_OK || !ptr.s) {
        LOG(ERROR) << "uci lookup package failed!";
        uci_free_context(ctx);
        return false;
    }

    // Loop through the option within the found section
    struct uci_element *elm = nullptr;
    uci_foreach_element(&ptr->s->options, elm)
    {
        struct uci_option *opt = uci_to_option(elm);
        // Only UCI_TYPE_STRING is supported
        if (o->type == UCI_TYPE_STRING) {
            params.emplace(o->e.name, o->v.string);
        }
    }

    return true;
}

bool uci_get_entry_type(const std::string &config_file, const std::string &entry_name,
                        std::string &entry_type)
{
    //entry: config_file: entry_name
    LOG(TRACE) << "entry: " << config_file << ": " << entry_name;

    struct uci_context *ctx = nullptr;
    if (!(ctx = uci_alloc_context())) {
        LOG(ERROR) << "uci context allocate failed!";
        return false;
    }

    struct uci_ptr ptr = nullptr;
    auto &path         = std::format("{}.{}", config_file, entry_name);
    if (uci_lookup_ptr(cont, &ptr, path.c_str(), true) != UCI_OK || !ptr.s) {
        LOG(ERROR) << "uci lookup package failed!";
        uci_free_context(ctx);
        return false;
    }

    entry_type = ptr->s.type;
    uci_free_context(ctx);
    return false;
}

bool uci_get_param(const std::string &config_file, const std::string &entry_type,
                   const std::string &entry_name, const std::string &param_name, std::string &param)
{
    //entry: config_file: entry_type(entry_name), param
    LOG(TRACE) << "entry: " << config_file << ": " << entry_type << "(" << entry_name << ")"
               << ", " << param_name;

    struct uci_context *ctx = nullptr;
    if (!(ctx = uci_alloc_context())) {
        LOG(ERROR) << "uci context allocate failed!";
        return false;
    }

    struct uci_ptr ptr = nullptr;
    auto &path         = std::format("{}.{}.{}", config_file, entry_name, param_name);
    if (uci_lookup_ptr(cont, &ptr, path.c_str(), false) != UCI_OK || !ptr.o) {
        LOG(ERROR) << "uci lookup package failed!";
        uci_free_context(ctx);
        return false;
    }

    if (ptr.o->type != UCI_TYPE_STRING) {
        LOG(ERROR) << "list values are not supported!";
        uci_free_context(ctx);
        return false;
    }

    param = ptr.o->v.string;
    uci_free_context(ctx);
    return true;
}

bool uci_delete_entry(const std::string &config_file, const std::string &entry_type,
                      const std::string &entry_name)
{
    //entry: config_file: entry_type(entry_name)
    LOG(TRACE) << "entry: " << config_file << ": " << entry_type << "(" << entry_name << ")";

    struct uci_context *ctx = nullptr;
    if (!(ctx = uci_alloc_context())) {
        LOG(ERROR) << "uci context allocate failed!";
        return false;
    }

    struct uci_ptr ptr = nullptr;
    auto &path         = std::format("{}.{}.{}", config_file, entry_name, param_name);
    if (uci_lookup_ptr(ctx, &ptr, path.c_str(), false) != UCI_OK || !ptr.s) {
        LOG(ERROR) << "uci lookup package failed!";
        uci_free_context(ctx);
        return false;
    }

    uci_delete(ctx, ptr);

    return false;
}

bool uci_get_all_entries(const std::string &config_file, const std::string &entry_type,
                         std::vector<std::string> entries)
{
    //entry: config_file: entry_type
    LOG(TRACE) << "entry: " << config_file << ": " << entry_type;

    struct uci_context *ctx = nullptr;
    if (!(ctx = uci_alloc_context())) {
        LOG(ERROR) << "uci context allocate failed!";
        return false;
    }

    struct uci_package *pkg = nullptr;
    if (!(pkg = uci_lookup_package(ctx, config_file.c_str()))) {
        LOG(ERROR) << "uci lookup package failed!";
        uci_free_context(ctx);
        return false;
    }

    // Loop through the sections with a matching section name (entry_name)
    struct uci_element *elm = nullptr;
    uci_foreach_element(&pkg->sections, elm)
    {
        struct uci_section *sec = uci_to_section(elm);
        if (entry_type.empty() || entry_type == sec->type) {
            entries.emplace_back(sec->e.name);
        }
    }
    LOG(DEBUG) << "Found " << entries.count uci_free_context(ctx);

    return true;
}
