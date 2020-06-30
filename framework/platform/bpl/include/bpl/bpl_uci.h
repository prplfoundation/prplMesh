/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _BPL_UCI_H_
#define _BPL_UCI_H_

#include "bpl.h"
#include "bpl_err.h"
#include <string>
#include <unordered_map>
#include <vector>

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

bool uci_entry_exists(const std::string &config_file, const std::string &entry_type,
                      const std::string &entry_name);

bool uci_add_entry(const std::string &config_file, const std::string &entry_type,
                   const std::string &entry_name);

bool uci_set_entry(const std::string &config_file, const std::string &entry_type,
                   const std::string &entry_name,
                   const std::unordered_map<std::string, std::string> &params);

bool uci_get_entry(const std::string &config_file, const std::string &entry_type,
                   const std::string &entry_name,
                   std::unordered_map<std::string, std::string> &params);

bool uci_get_entry_type(const std::string &config_file, const std::string &entry_name,
                        std::string &entry_type);

bool uci_get_param(const std::string &config_file, const std::string &entry_type,
                   const std::string &entry_name, const std::string &param_name,
                   std::string &param);

bool uci_delete_entry(const std::string &config_file, const std::string &entry_type,
                      const std::string &entry_name);

bool uci_get_all_entries(const std::string &config_file, const std::string &entry_type,
                         std::vector<std::string> entries);

} // namespace bpl
} // namespace beerocks

#endif /* _BPL_UCI_H_ */
