#include "bpl_db_uci.h"

#include <mapf/common/logger.h>
#include <mapf/common/utils.h>

namespace beerocks {
namespace bpl {
namespace db {

bool uci_section_exists(const std::string &package_name, const std::string &section_type,
                        const std::string &section_name)
{
    //package_name.(section_type)section_name
    LOG(TRACE) << "uci_section_exists() " << package_name << ".(" << section_type << ")"
               << section_name;

    return true;
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
