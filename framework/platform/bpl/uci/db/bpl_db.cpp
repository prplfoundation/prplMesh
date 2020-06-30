/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include "bpl/bpl_db.h"
#include "bpl/bpl_uci.h"

#include <mapf/common/logger.h>

//////////////////////////////////////////////////////////////////////////////
/////////////////////////////// Implementation ///////////////////////////////
//////////////////////////////////////////////////////////////////////////////

namespace beerocks {
namespace bpl {

#define DB_FILE "prplMesh_db"

bool db_has_entry(const std::string &entry_type, const std::string &entry_name)
{
    LOG(TRACE) << entry_type << ":" << entry_name;
    return uci_entry_exists(DB_FILE, entry_type, entry_name);
}

bool db_add_entry(const std::string &entry_type, const std::string &entry_name,
                  const std::unordered_map<std::string, std::string> &params)
{
    LOG(TRACE) << entry_type << ":" << entry_name;

    // Check if entry of the same name (and any type) exist.
    if (!uci_entry_exists(DB_FILE, "", entry_name)) {
        LOG(DEBUG) << "Entry " << entry_name << " does not exist! Create new one.";
        if (!uci_add_entry(DB_FILE, entry_type, entry_name)) {
            LOG(ERROR) << "Failed to create entry " << entry_name << "!";
            return false;
        }
    } else {
        // Entry exists, validate type match
        if (uci_entry_exists(DB_FILE, entry_type, entry_name)) {
            LOG(ERROR) << "Entry " << entry_name << " exists, but under diffrent type!";
            return false;
        }
    }
    // Update the new/existing entry
    LOG(DEBUG) << "Update entry " << entry_name << " with new values.";
    return uci_set_entry(DB_FILE, entry_type, entry_name, params);
}

bool db_set_entry(const std::string &entry_type, const std::string &entry_name,
                  const std::unordered_map<std::string, std::string> &params)
{
    LOG(TRACE) << entry_type << ":" << entry_name;

    if (!uci_entry_exists(DB_FILE, entry_type, entry_name)) {
        LOG(ERROR) << "Entry " << entry_name << " not found, or exists under a diffrent type!";
        return false;
    }
    // Update the new/existing entry
    LOG(DEBUG) << "Update entry " << entry_name << " with new values.";
    return uci_set_entry(DB_FILE, entry_type, entry_name, params);
}

bool db_get_entry(const std::string &entry_type, const std::string &entry_name,
                  std::unordered_map<std::string, std::string> &params)
{
    LOG(TRACE) << entry_type << ":" << entry_name;

    return uci_get_entry(DB_FILE, entry_type, entry_name, params);
}

bool db_get_entries_by_type(
    const std::string &entry_type,
    std::unordered_map<std::string, std::unordered_map<std::string, std::string>> &nested_params)
{
    LOG(TRACE) << entry_type;

    std::vector<std::string> entries;

    if (!uci_get_all_entries(DB_FILE, entry_type, entries)) {
        LOG(ERROR) << "Failed to get all entries";
        return false;
    }
    for (auto &entry_name : entries) {
        std::unordered_map<std::string, std::string> params;
        if (!uci_get_entry(DB_FILE, entry_type, entry_name, params)) {
            LOG(ERROR) << "Failed to get entry " << entry_name;
            return false;
        }
        nested_params.emplace(entry_name, params);
    }

    LOG(DEBUG) << "Found " << nested_params.count() << " entries!";
    return true;
}

bool db_remove_entry(const std::string &entry_type, const std::string &entry_name)
{
    LOG(TRACE) << entry_type << ":" << entry_name;

    if (!uci_entry_exists(DB_FILE, entry_type, entry_name)) {
        LOG(DEBUG) << "Entry " << entry_name << " not found!";
        return true;
    }
    return uci_delete_entry(DB_FILE, entry_type, entry_name, params);
}

} // namespace bpl
} // namespace beerocks
