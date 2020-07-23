/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include <bpl/bpl_db.h>

#include "bpl_db_uci.h"

#include <mapf/common/logger.h>

//////////////////////////////////////////////////////////////////////////////
/////////////////////////////// Implementation ///////////////////////////////
//////////////////////////////////////////////////////////////////////////////

namespace beerocks {
namespace bpl {

constexpr char DB_FILE[] = "prplmesh_db";

bool db_has_entry(const std::string &entry_type, const std::string &entry_name)
{
    LOG(TRACE) << entry_type << ":" << entry_name;

    if (entry_name.empty()) {
        LOG(ERROR) << "Entry name must be provided";
        return false;
    }
    return db::uci_section_exists(DB_FILE, entry_type, entry_name);
}

bool db_add_entry(const std::string &entry_type, const std::string &entry_name,
                  const std::unordered_map<std::string, std::string> &params)
{
    LOG(TRACE) << entry_type << ":" << entry_name;

    if (entry_name.empty() || entry_type.empty()) {
        LOG(ERROR) << "Entry name & type must be set";
        return false;
    }
    // Check if entry of the same name exists.
    if (db::uci_section_exists(DB_FILE, "", entry_name)) {
        LOG(ERROR) << "Entry " << entry_name << " already exists";
        return false;
    }
    if (!db::uci_add_section(DB_FILE, entry_type, entry_name)) {
        LOG(ERROR) << "Failed to create entry " << entry_name << "!";
        return false;
    }
    LOG(DEBUG) << "Update entry " << entry_name << " with new values.";
    return db::uci_set_section(DB_FILE, entry_type, entry_name, params);
}

bool db_set_entry(const std::string &entry_type, const std::string &entry_name,
                  const std::unordered_map<std::string, std::string> &params)
{
    LOG(TRACE) << entry_type << ":" << entry_name;

    if (entry_name.empty()) {
        LOG(ERROR) << "Entry name must be provided";
        return false;
    }
    if (!db::uci_section_exists(DB_FILE, entry_type, entry_name)) {
        LOG(DEBUG) << "Entry " << entry_name
                   << (!entry_type.empty() ? std::string(" of type ") + entry_type : "")
                   << " not found!";
        return false;
    }
    // Update the new/existing entry
    LOG(DEBUG) << "Update entry " << entry_name << " with new values.";
    return db::uci_set_section(DB_FILE, entry_type, entry_name, params);
}

bool db_get_entry(const std::string &entry_type, const std::string &entry_name,
                  std::unordered_map<std::string, std::string> &params)
{
    LOG(TRACE) << entry_type << ":" << entry_name;

    if (entry_name.empty()) {
        LOG(ERROR) << "Entry name must be provided";
        return false;
    }
    if (params.size() == 0) {
        // params map is empty, getting all parameters
        return db::uci_get_section(DB_FILE, entry_type, entry_name, params);
    }
    for (auto &param : params) {
        // params map is not empty, getting selected parameters
        if (!db::uci_get_option(DB_FILE, entry_type, entry_name, param.first, param.second)) {
            LOG(ERROR) << "Failed to get " << param.first;
            // TODO:
            return false;
        }
    }
    return true;
}

bool db_get_entries_by_type(
    const std::string &entry_type,
    std::unordered_map<std::string, std::unordered_map<std::string, std::string>> &nested_params)
{
    LOG(TRACE) << entry_type;

    std::vector<std::string> entries;

    if (!db::uci_get_all_sections(DB_FILE, entry_type, entries)) {
        LOG(ERROR) << "Failed to get all entries";
        return false;
    }
    for (auto &entry_name : entries) {
        std::unordered_map<std::string, std::string> params;
        if (!db::uci_get_section(DB_FILE, entry_type, entry_name, params)) {
            LOG(ERROR) << "Failed to get entry " << entry_name;
            return false;
        }
        // If the returning parameters are empty, there is not need to set.
        if (!params.empty()) {
            nested_params[entry_name] = std::move(params);
        }
    }

    LOG(DEBUG) << "Found " << nested_params.size() << " entries!";
    return true;
}

bool db_remove_entry(const std::string &entry_type, const std::string &entry_name)
{
    LOG(TRACE) << entry_type << ":" << entry_name;

    if (entry_name.empty()) {
        LOG(ERROR) << "Entry name must be provided";
        return false;
    }
    if (!db::uci_section_exists(DB_FILE, entry_type, entry_name)) {
        LOG(DEBUG) << "Entry " << entry_name
                   << (!entry_type.empty() ? std::string(" of type ") + entry_type : "")
                   << " not found!";
        return true;
    }
    return db::uci_delete_section(DB_FILE, entry_type, entry_name);
}

} // namespace bpl
} // namespace beerocks
