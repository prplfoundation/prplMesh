/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _BPL_DB_UCI_H_
#define _BPL_DB_UCI_H_

#include <string>
#include <unordered_map>
#include <vector>

namespace beerocks {
namespace bpl {
namespace db {

/****************************************************************************/
/******************************* Definitions ********************************/
/****************************************************************************/

/****************************************************************************/
/******************************* Structures *********************************/
/****************************************************************************/

/****************************************************************************/
/******************************** Functions *********************************/
/****************************************************************************/

/**
 * @brief Find if section exists.
 * 
 * @param[in] package_name name of the requested configuration file.
 * @param[in] section_type type of the requested section.
 * @param[in] section_name name of the requested section.
 * @return true if section exists in db, false otherwise
 */
bool uci_section_exists(const std::string &package_name, const std::string &section_type,
                        const std::string &section_name);

/**
 * @brief Add new named section.
 * 
 * @param[in] package_name name of the requested configuration file.
 * @param[in] section_type type of the requested section.
 * @param[in] section_name name of the requested section.
 * @return true if section exists in db, false otherwise
 */
bool uci_add_section(const std::string &package_name, const std::string &section_type,
                     const std::string &section_name);

/**
 * @brief Set values in section, updating as needed.
 * An option set to an empty string will be removed from the section.
 * An option that does not exist will be added to the section.
 * An option that already exists will be overridden with the new value
 * Options that exist and are not mentioned in @a params will remain unchanged.
 * 
 * @param[in] package_name name of the requested configuration file.
 * @param[in] section_type type of the requested section.
 * @param[in] section_name name of the requested section.
 * @param[in] params unordered_map containing the params the requested section, to be set.
 * @return true on success, false otherwise.
 */
bool uci_set_section(const std::string &package_name, const std::string &section_type,
                     const std::string &section_name,
                     const std::unordered_map<std::string, std::string> &options);

/**
 * @brief Get values in section.
 * 
 * @param[in] package_name name of the requested configuration file.
 * @param[in] section_type type of the requested section.
 * @param[in] section_name name of the requested section.
 * @param[in/out] params unordered_map empty initalized map, that will contain found parameters. 
 * @return true on success, false otherwise.
 */
bool uci_get_section(const std::string &package_name, const std::string &section_type,
                     const std::string &section_name,
                     std::unordered_map<std::string, std::string> &options);

/**
 * @brief Get type of section by name.
 * 
 * @param[in] package_name name of the requested configuration file.
 * @param[in] section_name name of the requested section.
 * @param[in/out] section_type initalized string, will contain type of found section.
 * @return true on success, false otherwise.
 */
bool uci_get_section_type(const std::string &package_name, const std::string &section_name,
                          std::string &section_type);

/**
 * @brief Get specific value from section.
 * 
 * @param[in] package_name name of the requested configuration file.
 * @param[in] section_type type of the requested section.
 * @param[in] section_name name of the requested section.
 * @param[in] option_name name of the requested parameter.
 * @param[in/out] option_value initalized string, will contain type of found section. 
 * @return true on success, false otherwise.
 */
bool uci_get_option(const std::string &package_name, const std::string &section_type,
                    const std::string &section_name, const std::string &option_name,
                    std::string &option_value);

/**
 * @brief Delete existing section.
 * 
 * @param[in] package_name name of the requested configuration file.
 * @param[in] section_type type of the requested section.
 * @param[in] section_name name of the requested section.
 * @return true if section exists in db, false otherwise
 */
bool uci_delete_section(const std::string &package_name, const std::string &section_type,
                        const std::string &section_name);

/**
 * @brief Get all entries with the same type.
 * 
 * @param[in] package_name name of the requested configuration file.
 * @param[in] section_type type of the requested section. (If empty get sections of all types)
 * @param[in/out] sections initialized vector, will be filled with all matching sections.
 * @return true if section exists in db, false otherwise
 */
bool uci_get_all_sections(const std::string &package_name, const std::string &section_type,
                          std::vector<std::string> &sections);

} // namespace db
} // namespace bpl
} // namespace beerocks

#endif /* _BPL_DB_UCI_H_ */
