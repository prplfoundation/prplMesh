#include "bpl_db_uci.h"

#include <mapf/common/logger.h>
#include <mapf/common/utils.h>

extern "C" {
#include <uci.h>
}

namespace beerocks {
namespace bpl {
namespace db {

constexpr ssize_t MAX_UCI_BUF_LEN  = 64;
constexpr char package_path[]      = "%s";
constexpr char section_path[]      = "%s.%s";
constexpr char section_type_path[] = "%s.%s=%s";
constexpr char option_path[]       = "%s.%s.%s";
constexpr char option_value_path[] = "%s.%s.%s=%s";

std::string uci_get_error(uci_context *ctx)
{
    char *err_buf = nullptr;
    uci_get_errorstr(ctx, &err_buf, "");
    return std::string(err_buf == nullptr ? "" : err_buf);
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

bool uci_section_exists(const std::string &package_name, const std::string &section_type,
                        const std::string &section_name)
{
    //package_name.(section_type)section_name
    LOG(TRACE) << "uci_section_exists() " << package_name << ".(" << section_type << ")"
               << section_name;

    auto ctx = alloc_context();
    if (!ctx) {
        return false;
    }

    char path[MAX_UCI_BUF_LEN] = {0};
    if (snprintf(path, MAX_UCI_BUF_LEN, package_path, package_name.c_str()) <= 0) {
        LOG(ERROR) << "Failed to compose path";
        return false;
    }

    uci_package *pkg = nullptr;
    if (!(pkg = uci_lookup_package(ctx.get(), path))) {
        // Try and load the package file, in case it wasn't loaded before
        uci_load(ctx.get(), path, &pkg);
        if (!pkg) {
            LOG(ERROR) << "uci lookup package failed!" << std::endl << uci_get_error(ctx.get());
            return false;
        }
    }

    bool found = false;
    // Loop through the sections with a matching section name (section_name)
    uci_element *elm = nullptr;
    uci_foreach_element(&pkg->sections, elm)
    {
        found            = true;
        uci_section *sec = uci_to_section(elm);
        // if the section_type is empty all matching sections are a valid match
        auto name_match = section_name.compare(sec->e.name) == 0;
        auto type_match = section_type.empty() || section_type.compare(sec->type) == 0;
        if (name_match && type_match) {
            LOG(TRACE) << "Found match: " << sec->type << "(" << sec->e.name << ")";
            return true;
        }
    }

    LOG(TRACE) << "No " << section_name << " section found"
               << (section_type.empty() || !found ? "!"
                                                  : std::string(" with type: ") + section_type);

    return false;
}

bool uci_add_section(const std::string &package_name, const std::string &section_type,
                     const std::string &section_name)
{
    //package_name.(section_type)section_name
    LOG(TRACE) << "uci_add_section() " << package_name << ".(" << section_type << ")"
               << section_name;

    auto ctx = alloc_context();
    if (!ctx) {
        return false;
    }

    char sec_path[MAX_UCI_BUF_LEN] = {0};
    if (snprintf(sec_path, MAX_UCI_BUF_LEN, section_type_path, package_name.c_str(),
                 section_name.c_str(), section_type.c_str()) <= 0) {
        LOG(ERROR) << "Failed to compose path";
        return false;
    }

    uci_ptr ptr;
    // Get pointer to value
    if (uci_lookup_ptr(ctx.get(), &ptr, sec_path, true) != UCI_OK) {
        LOG(ERROR) << "UCI failed to lookup ptr for path: " << sec_path << std::endl
                   << uci_get_error(ctx.get());
        return false;
    }

    // If section does not exists, create new one, otherwise change the "type" of the section
    if (uci_set(ctx.get(), &ptr) != UCI_OK) {
        LOG(ERROR) << "Failed to add new section";
        return false;
    }

    // Create delta from changes, this does not change the persistent file.
    if (uci_save(ctx.get(), ptr.p) != UCI_OK) {
        LOG(ERROR) << "Failed to save changes!" << std::endl << uci_get_error(ctx.get());
        return false;
    }

    // Commit changes to file
    if (uci_commit(ctx.get(), &ptr.p, false) != UCI_OK) {
        LOG(ERROR) << "Failed to commit changes!" << std::endl << uci_get_error(ctx.get());
        return false;
    }

    return true;
}

bool uci_set_section(const std::string &package_name, const std::string &section_type,
                     const std::string &section_name,
                     const std::unordered_map<std::string, std::string> &options)
{
    //package_name.(section_type)section_name
    LOG(TRACE) << "uci_set_section() " << package_name << ".(" << section_type << ")"
               << section_name;

    auto ctx = alloc_context();
    if (!ctx) {
        return false;
    }

    for (auto &option : options) {
        char opt_path[MAX_UCI_BUF_LEN] = {0};
        if (snprintf(opt_path, MAX_UCI_BUF_LEN, option_path, package_name.c_str(),
                     section_name.c_str(), option.first.c_str()) <= 0) {
            LOG(ERROR) << "Failed to compose path";
            return false;
        }

        LOG(TRACE) << "Trying to lookup: " << opt_path;

        uci_ptr opt_ptr;
        // Get pointer to value
        if (uci_lookup_ptr(ctx.get(), &opt_ptr, opt_path, true) != UCI_OK) {
            LOG(ERROR) << "UCI failed to lookup ptr for path: " << opt_path << std::endl
                       << uci_get_error(ctx.get());
            return false;
        }

        //Set the value to the pointer
        LOG(TRACE) << "Setting " << option.first << "=" << option.second;
        opt_ptr.value = strdup(option.second.c_str());

        /*
            If the value is set to empty the option will be removed
            If the option does not exist it will be created
            If the option does exist the value will be overridden
        */
        if (uci_set(ctx.get(), &opt_ptr) != UCI_OK) {
            LOG(ERROR) << "Failed to set option " << option.first;
            return false;
        }

        // Create delta from changes, this does not change the persistent file.
        if (uci_save(ctx.get(), opt_ptr.p) != UCI_OK) {
            LOG(ERROR) << "Failed to save changes!" << std::endl << uci_get_error(ctx.get());
            return false;
        }
    }
    LOG(TRACE) << "Finished setting parameters. about to commit changes";

    char pkg_path[MAX_UCI_BUF_LEN] = {0};
    if (snprintf(pkg_path, MAX_UCI_BUF_LEN, package_path, package_name.c_str()) <= 0) {
        LOG(ERROR) << "Failed to compose path";
        return false;
    }
    uci_ptr pkg_ptr;
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

bool uci_get_section(const std::string &package_name, const std::string &section_type,
                     const std::string &section_name,
                     std::unordered_map<std::string, std::string> &options)
{
    //package_name.(section_type)section_name
    LOG(TRACE) << "uci_get_section() " << package_name << ".(" << section_type << ")"
               << section_name;

    auto ctx = alloc_context();
    if (!ctx) {
        return false;
    }

    char path[MAX_UCI_BUF_LEN] = {0};
    if (snprintf(path, MAX_UCI_BUF_LEN, section_path, package_name.c_str(), section_name.c_str()) <=
        0) {
        LOG(ERROR) << "Failed to compose path";
        return false;
    }

    uci_ptr ptr;
    if (uci_lookup_ptr(ctx.get(), &ptr, path, true) != UCI_OK || !ptr.s) {
        LOG(ERROR) << "UCI failed to lookup ptr for path: " << path << std::endl
                   << uci_get_error(ctx.get());
        return false;
    }

    // Loop through the option within the found section
    uci_element *elm = nullptr;
    uci_foreach_element(&ptr.s->options, elm)
    {
        uci_option *opt = uci_to_option(elm);
        // Only UCI_TYPE_STRING is supported
        if (opt->type == UCI_TYPE_STRING) {
            options[std::string(opt->e.name)] = opt->v.string;
        }
    }

    return true;
}

bool uci_get_section_type(const std::string &package_name, const std::string &section_name,
                          std::string &section_type)
{
    //package_name.section_name
    LOG(TRACE) << "uci_get_section_type() " << package_name << "." << section_name;

    auto ctx = alloc_context();
    if (!ctx) {
        return false;
    }

    char path[MAX_UCI_BUF_LEN] = {0};
    if (snprintf(path, MAX_UCI_BUF_LEN, section_path, package_name.c_str(), section_name.c_str()) <=
        0) {
        LOG(ERROR) << "Failed to compose path";
        return false;
    }

    uci_ptr ptr;
    if (uci_lookup_ptr(ctx.get(), &ptr, path, true) != UCI_OK || !ptr.s) {
        LOG(ERROR) << "UCI failed to lookup ptr for path: " << path << std::endl
                   << uci_get_error(ctx.get());
        return false;
    }

    section_type = ptr.s->type;
    return true;
}

bool uci_get_option(const std::string &package_name, const std::string &section_type,
                    const std::string &section_name, const std::string &option_name,
                    std::string &option_value)
{
    //package_name.(section_type)section_name.option_name
    LOG(TRACE) << "uci_get_param() " << package_name << ".(" << section_type << ")" << section_name
               << "." << option_name;

    if (!uci_section_exists(package_name, section_type, section_name)) {
        LOG(ERROR) << "section " << section_name << " of type " << section_type
                   << " was not found!";
        return false;
    }

    auto ctx = alloc_context();
    if (!ctx) {
        return false;
    }

    char path[MAX_UCI_BUF_LEN] = {0};
    if (snprintf(path, MAX_UCI_BUF_LEN, option_path, package_name.c_str(), section_name.c_str(),
                 option_name.c_str()) <= 0) {
        LOG(ERROR) << "Failed to compose path";
        return false;
    }

    uci_ptr ptr;
    if (uci_lookup_ptr(ctx.get(), &ptr, path, true) != UCI_OK || !ptr.o) {
        LOG(ERROR) << "UCI failed to lookup ptr for path: " << path << std::endl
                   << uci_get_error(ctx.get());
        return false;
    }

    option_value = ptr.o->v.string;
    return true;
}

bool uci_delete_section(const std::string &package_name, const std::string &section_type,
                        const std::string &section_name)
{
    //package_name.(section_type)section_name
    LOG(TRACE) << "uci_delete_section() " << package_name << ".(" << section_type << ")"
               << section_name;

    if (!uci_section_exists(package_name, section_type, section_name)) {
        LOG(ERROR) << "section " << section_name << " of type " << section_type
                   << " was not found!";
        return false;
    }

    auto ctx = alloc_context();
    if (!ctx) {
        return false;
    }

    char sec_path[MAX_UCI_BUF_LEN] = {0};
    if (snprintf(sec_path, MAX_UCI_BUF_LEN, section_path, package_name.c_str(),
                 section_name.c_str()) <= 0) {
        LOG(ERROR) << "Failed to compose path";
        return false;
    }

    uci_ptr sec_ptr;
    if (uci_lookup_ptr(ctx.get(), &sec_ptr, sec_path, true) != UCI_OK || !sec_ptr.s) {
        LOG(ERROR) << "UCI failed to lookup ptr for path: " << sec_path << std::endl
                   << uci_get_error(ctx.get());
        return false;
    }

    if (uci_delete(ctx.get(), &sec_ptr) != UCI_OK) {
        LOG(ERROR) << "UCI failed to delete ptr: " << sec_path << std::endl
                   << uci_get_error(ctx.get());
        return false;
    }

    char pkg_path[MAX_UCI_BUF_LEN] = {0};
    if (snprintf(pkg_path, MAX_UCI_BUF_LEN, package_path, package_name.c_str()) <= 0) {
        LOG(ERROR) << "Failed to compose path";
        return false;
    }

    uci_ptr pkg_ptr;
    if (uci_lookup_ptr(ctx.get(), &pkg_ptr, pkg_path, true) != UCI_OK || !pkg_ptr.p) {
        LOG(ERROR) << "UCI failed to lookup ptr for path: " << pkg_path << std::endl
                   << uci_get_error(ctx.get());
        return false;
    }

    // Create delta from changes, this does not change the persistent file.
    if (uci_save(ctx.get(), pkg_ptr.p) != UCI_OK) {
        LOG(ERROR) << "Failed to save changes!" << std::endl << uci_get_error(ctx.get());
        return false;
    }

    // Commit changes to file
    if (uci_commit(ctx.get(), &pkg_ptr.p, false) != UCI_OK) {
        LOG(ERROR) << "Failed to commit changes!" << std::endl << uci_get_error(ctx.get());
        return false;
    }

    return true;
}

bool uci_get_all_sections(const std::string &package_name, const std::string &section_type,
                          std::vector<std::string> &sections)
{
    //package_name.(section_type)
    LOG(TRACE) << "uci_get_all_sections() " << package_name << ".(" << section_type << ")";

    return true;
}

} // namespace db
} // namespace bpl
} // namespace beerocks
