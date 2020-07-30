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
    // Generate a uci path to the package we wish to lookup
    if (snprintf(path, MAX_UCI_BUF_LEN, package_path, package_name.c_str()) <= 0) {
        LOG(ERROR) << "Failed to compose path";
        return false;
    }

    uci_package *pkg = uci_lookup_package(ctx.get(), path);
    if (!pkg) {
        // Try and load the package file, in case it wasn't loaded before
        uci_load(ctx.get(), path, &pkg);
        if (!pkg) {
            LOG(ERROR) << "uci lookup package failed!" << std::endl << uci_get_error(ctx.get());
            return false;
        }
    }

    bool section_found = false;
    // Loop through all sections
    uci_section *sec = nullptr;
    uci_element *elm = nullptr;
    uci_foreach_element(&pkg->sections, elm)
    {
        sec = uci_to_section(elm);
        // uci enforces sections must have unique name
        if (section_name.compare(sec->e.name) == 0) {
            LOG(TRACE) << "Found section: " << sec->type << "(" << sec->e.name << ")";
            section_found = true;
            break;
        }
    }

    if (!section_found) {
        LOG(DEBUG) << "No matching section found for " << section_name;
        return false;
    }

    // If the section_type is empty all matching sections are a valid match
    if (!section_type.empty() && section_type.compare(sec->type) != 0) {
        LOG(DEBUG) << "Section with matching name found, but types don't match" << std::endl
                   << "Searched type: " << section_type << ", Found type: " << sec->type;
        return false;
    }

    return true;
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
    // Generate a uci path to the section we wish to lookup
    if (snprintf(sec_path, MAX_UCI_BUF_LEN, section_type_path, package_name.c_str(),
                 section_name.c_str(), section_type.c_str()) <= 0) {
        LOG(ERROR) << "Failed to compose path";
        return false;
    }

    uci_ptr sec_ptr;
    // Initialize section pointer from path.
    if (uci_lookup_ptr(ctx.get(), &sec_ptr, sec_path, true) != UCI_OK) {
        LOG(ERROR) << "UCI failed to lookup ptr for path: " << sec_path << std::endl
                   << uci_get_error(ctx.get());
        return false;
    }

    // Check if section already exists
    if (sec_ptr.s) {
        LOG(ERROR) << "Section already exists!";
        return false;
    }

    // Add a new named section.
    if (uci_set(ctx.get(), &sec_ptr) != UCI_OK) {
        LOG(ERROR) << "Failed to add new section";
        return false;
    }

    // Create delta from changes, this does not change the persistent file.
    if (uci_save(ctx.get(), sec_ptr.p) != UCI_OK) {
        LOG(ERROR) << "Failed to save changes!" << std::endl << uci_get_error(ctx.get());
        return false;
    }

    // Commit changes to file
    if (uci_commit(ctx.get(), &sec_ptr.p, false) != UCI_OK) {
        LOG(ERROR) << "Failed to commit changes!" << std::endl << uci_get_error(ctx.get());
        return false;
    }

    return true;
}

bool uci_set_section(const std::string &package_name, const std::string &section_type,
                     const std::string &section_name, const OptionsUnorderedMap &options)
{
    //package_name.(section_type)section_name
    LOG(TRACE) << "uci_set_section() " << package_name << ".(" << section_type << ")"
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

    for (auto &option : options) {
        char opt_path[MAX_UCI_BUF_LEN] = {0};
        // Generate a uci path to the option we wish to set
        if (snprintf(opt_path, MAX_UCI_BUF_LEN, option_path, package_name.c_str(),
                     section_name.c_str(), option.first.c_str()) <= 0) {
            LOG(ERROR) << "Failed to compose path";
            return false;
        }

        uci_ptr opt_ptr;
        // Initialize option pointer from path.
        if (uci_lookup_ptr(ctx.get(), &opt_ptr, opt_path, true) != UCI_OK) {
            LOG(ERROR) << "UCI failed to lookup ptr for path: " << opt_path << std::endl
                       << uci_get_error(ctx.get());
            return false;
        }
        // Set the value of option in uci_ptr
        // when using uci_set, it will store the change in the context
        auto value_ptr = strdup(option.second.c_str());
        if (!value_ptr) {
            LOG(ERROR) << "Failed to duplicate value buffer";
            return false;
        }
        opt_ptr.value = value_ptr;

        /*
            If the value is set to empty the option will be removed
            If the option does not exist it will be created
            If the option does exist the value will be overridden
        */
        if (uci_set(ctx.get(), &opt_ptr) != UCI_OK) {
            LOG(ERROR) << "Failed to set option " << option.first << std::endl
                       << uci_get_error(ctx.get());
            return false;
        }

        // Create delta from changes in the coxtext, this does not change the persistent file.
        if (uci_save(ctx.get(), opt_ptr.p) != UCI_OK) {
            LOG(ERROR) << "Failed to save changes!" << std::endl << uci_get_error(ctx.get());
            return false;
        }
    }

    // To save the changes made we need to call "uci_commit" with the package
    // Loading a uci_ptr will also load the changes saved in the context's delta
    char pkg_path[MAX_UCI_BUF_LEN] = {0};
    // Generate a uci path to the package we wish to lookup
    if (snprintf(pkg_path, MAX_UCI_BUF_LEN, package_path, package_name.c_str()) <= 0) {
        LOG(ERROR) << "Failed to compose path";
        return false;
    }

    uci_ptr pkg_ptr;
    // Initialize package pointer from path & validate package existace.
    if (uci_lookup_ptr(ctx.get(), &pkg_ptr, pkg_path, true) != UCI_OK || !pkg_ptr.p) {
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
                     const std::string &section_name, OptionsUnorderedMap &options)
{
    //package_name.(section_type)section_name
    LOG(TRACE) << "uci_get_section() " << package_name << ".(" << section_type << ")"
               << section_name;

    return true;
}

bool uci_get_section_type(const std::string &package_name, const std::string &section_name,
                          std::string &section_type)
{
    //package_name.section_name
    LOG(TRACE) << "uci_get_section_type() " << package_name << "." << section_name;

    return true;
}

bool uci_get_option(const std::string &package_name, const std::string &section_type,
                    const std::string &section_name, const std::string &option_name,
                    std::string &option_value)
{
    //package_name.(section_type)section_name.option_name
    LOG(TRACE) << "uci_get_param() " << package_name << ".(" << section_type << ")" << section_name
               << "." << option_name;

    return true;
}

bool uci_delete_section(const std::string &package_name, const std::string &section_type,
                        const std::string &section_name)
{
    //package_name.(section_type)section_name
    LOG(TRACE) << "uci_delete_section() " << package_name << ".(" << section_type << ")"
               << section_name;

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
