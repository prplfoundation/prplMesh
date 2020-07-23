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

    return true;
}

bool uci_set_section(const std::string &package_name, const std::string &section_type,
                     const std::string &section_name,
                     const std::unordered_map<std::string, std::string> &options)
{
    //package_name.(section_type)section_name
    LOG(TRACE) << "uci_set_section() " << package_name << ".(" << section_type << ")"
               << section_name;

    return true;
}

bool uci_get_section(const std::string &package_name, const std::string &section_type,
                     const std::string &section_name,
                     std::unordered_map<std::string, std::string> &options)
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
