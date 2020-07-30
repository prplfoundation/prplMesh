/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include "bpl/bpl_db.h"

#include <mapf/common/logger.h>

//////////////////////////////////////////////////////////////////////////////
/////////////////////////////// Implementation ///////////////////////////////
//////////////////////////////////////////////////////////////////////////////

namespace beerocks {
namespace bpl {

bool db_has_entry(const std::string &entry_type, const std::string &entry_name)
{
    LOG(TRACE) << " - NOT IMPLEMENTED!";
    return false;
}

bool db_add_entry(const std::string &entry_type, const std::string &entry_name,
                  const std::unordered_map<std::string, std::string> &params)
{
    LOG(TRACE) << " - NOT IMPLEMENTED!";
    return true;
}

bool db_set_entry(const std::string &entry_type, const std::string &entry_name,
                  const std::unordered_map<std::string, std::string> &params)
{
    LOG(TRACE) << " - NOT IMPLEMENTED!";
    return true;
}

bool db_get_entry(const std::string &entry_type, const std::string &entry_name,
                  std::unordered_map<std::string, std::string> &params)
{
    LOG(TRACE) << " - NOT IMPLEMENTED!";
    return true;
}

bool db_get_entries_by_type(
    const std::string &entry_type,
    std::unordered_map<std::string, std::unordered_map<std::string, std::string>> &nested_params)
{
    LOG(TRACE) << " - NOT IMPLEMENTED!";
    return true;
}

bool db_remove_entry(const std::string &entry_type, const std::string &entry_name)
{
    LOG(TRACE) << " - NOT IMPLEMENTED!";
    return true;
}

} // namespace bpl
} // namespace beerocks
