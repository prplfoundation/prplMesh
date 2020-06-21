/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _BPL_DB_H_
#define _BPL_DB_H_

#include "bpl.h"
#include "bpl_err.h"
#include <stdint.h>
#include <string>
#include <unordered_map>

namespace beerocks {
namespace bpl {

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
 * @brief find if entry exists
 * 
 * @param[in] entry_type type of the requested entry.
 * @param[in] entry_name name of the requested entry.
 * @return true if entry exists in db, false otherwise
 */
bool db_has_entry(const std::string &entry_type, const std::string &entry_name);

/**
 * @brief Add new entry with initial value.
 * 
 * @param[in] entry_type type of the requested entry.
 * @param[in] entry_name name of the requested entry.
 * @param[in] params initial params of the entry.
 * @return true on success, false otherwise
 */
bool db_add_entry(const std::string &entry_type, const std::string &entry_name,
                  const std::unordered_map<std::string, std::string> &params);
/**
 * @brief Set values in entry, append attributes and override existing.
 * 
 * @param[in] entry_type type of the requested entry.
 * @param[in] entry_name name of the requested entry.
 * @param[in] params unordered_map containing the params the requested entry, to be set.
 * @return true on success, false otherwise.
 */
bool db_set_entry(const std::string &entry_type, const std::string &entry_name,
                  const std::unordered_map<std::string, std::string> &params);
/**
 * @brief Get values of entry, return all avaliable values.
 * 
 * @param[in] entry_type type of the requested entry.
 * @param[in] entry_name name of the requested entry.
 * @param [in/out] params unordered_map containing the params of the requested entry.
 * @return true on success, false otherwise.
 */
bool db_get_entry(const std::string &entry_type, const std::string &entry_name,
                  std::unordered_map<std::string, std::string> &params);

/**
 * @brief Get all entries by type as a nested map <Entry-name, <Attrebute-name, value>>.
 * 
 * @param[in] entry_type type of the requested entries.
 * @param[in/out] nested_params nested map of params (with entry's name as key).
 * @return true on success, false otherwise.
 */
bool db_get_entries_by_type(
    const std::string &entry_type,
    std::unordered_map<std::string, std::unordered_map<std::string, std::string>> &nested_params);

/**
 * @brief Remove entry by name.
 * 
 * @param[in] entry_type type of the requested entry.
 * @param[in] entry_name name of the requested entry.
 * @return true on success, false otherwise.
 */
bool db_remove_entry(const std::string &entry_type, const std::string &entry_name);

} // namespace bpl
} // namespace beerocks

#endif /* _BPL_DB_H_ */
