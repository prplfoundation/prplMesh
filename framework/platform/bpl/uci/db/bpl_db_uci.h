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
 * @brief Find if entry exists.
 * 
 * @param[in] config_file name of the requested configuration.
 * @param[in] entry_type type of the requested entry.
 * @param[in] entry_name name of the requested entry.
 * @return true if entry exists in db, false otherwise
 */
bool uci_entry_exists(const std::string &config_file, const std::string &entry_type,
                      const std::string &entry_name);

/**
 * @brief Add new named entry.
 * 
 * @param[in] config_file name of the requested configuration.
 * @param[in] entry_type type of the requested entry.
 * @param[in] entry_name name of the requested entry.
 * @return true if entry exists in db, false otherwise
 */
bool uci_add_entry(const std::string &config_file, const std::string &entry_type,
                   const std::string &entry_name);

/**
 * @brief Set values in entry, updating as needed.
 * A parameter set to an empty string will remove it from the entry.
 * A parameter that does not exist will be added to the entry.
 * A parameter that already exists will be overridden with the new value
 * 
 * @param[in] config_file name of the requested configuration.
 * @param[in] entry_type type of the requested entry.
 * @param[in] entry_name name of the requested entry.
 * @param[in] params unordered_map containing the params the requested entry, to be set.
 * @return true on success, false otherwise.
 */
bool uci_set_entry(const std::string &config_file, const std::string &entry_type,
                   const std::string &entry_name,
                   const std::unordered_map<std::string, std::string> &params);

/**
 * @brief Get values in entry.
 * 
 * @param[in] config_file name of the requested configuration.
 * @param[in] entry_type type of the requested entry.
 * @param[in] entry_name name of the requested entry.
 * @param[in/out] params unordered_map empty initalized map, that will contain found parameters. 
 * @return true on success, false otherwise.
 */
bool uci_get_entry(const std::string &config_file, const std::string &entry_type,
                   const std::string &entry_name,
                   std::unordered_map<std::string, std::string> &params);

/**
 * @brief Get type of entry by name.
 * 
 * @param[in] config_file name of the requested configuration.
 * @param[in] entry_name name of the requested entry.
 * @param[in/out] entry_type initalized string, will contain type of found entry.
 * @return true on success, false otherwise.
 */
bool uci_get_entry_type(const std::string &config_file, const std::string &entry_name,
                        std::string &entry_type);

/**
 * @brief Get specific value from entry.
 * 
 * @param[in] config_file name of the requested configuration.
 * @param[in] entry_type type of the requested entry.
 * @param[in] entry_name name of the requested entry.
 * @param[in] param_name name of the requested parameter.
 * @param[in/out] value initalized string, will contain type of found entry. 
 * @return true on success, false otherwise.
 */
bool uci_get_param(const std::string &config_file, const std::string &entry_type,
                   const std::string &entry_name, const std::string &param_name,
                   std::string &value);

/**
 * @brief Delete existing entry.
 * 
 * @param[in] config_file name of the requested configuration.
 * @param[in] entry_type type of the requested entry.
 * @param[in] entry_name name of the requested entry.
 * @return true if entry exists in db, false otherwise
 */
bool uci_delete_entry(const std::string &config_file, const std::string &entry_type,
                      const std::string &entry_name);

/**
 * @brief Get all entries with the same type.
 * 
 * @param[in] config_file name of the requested configuration.
 * @param[in] entry_type type of the requested entries.
 * @param[in/out] entries initialized vector, will be filled with all entries that match the type.
 * @return true if entry exists in db, false otherwise
 */
bool uci_get_all_entries(const std::string &config_file, const std::string &entry_type,
                         std::vector<std::string> &entries);

} // namespace db
} // namespace bpl
} // namespace beerocks

#endif /* _BPL_DB_UCI_H_ */
