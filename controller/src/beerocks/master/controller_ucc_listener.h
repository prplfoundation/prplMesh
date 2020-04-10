/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2019 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef __CONTROLLER_UCC_LISTENER_H__
#define __CONTROLLER_UCC_LISTENER_H__

#include <bcl/beerocks_ucc_listener.h>

#include "db/db.h"

using namespace son;

namespace beerocks {
class controller_ucc_listener : public beerocks_ucc_listener {
public:
    controller_ucc_listener(db &database, ieee1905_1::CmduMessageTx &cmdu_tx);
    ~controller_ucc_listener(){};

private:
    void lock() override { m_database.lock(); }
    void unlock() override { m_database.unlock(); }
    std::string fill_version_reply_string() override;
    void clear_configuration() override;
    bool validate_destination_alid(const std::string &dest_alid) override;
    bool send_cmdu_to_destination(ieee1905_1::CmduMessageTx &cmdu_tx,
                                  const std::string &dest_mac = std::string()) override;
    bool handle_dev_set_config(std::unordered_map<std::string, std::string> &params,
                               std::string &err_string) override;
    bool handle_dev_get_param(std::unordered_map<std::string, std::string> &params,
                              std::string &value) override;
    static std::string parse_bss_info(const std::string &bss_info_str,
                                      son::wireless_utils::sBssInfoConf &bss_info_conf,
                                      std::string &err_string);

    db &m_database;
};

} // namespace beerocks

#endif // __CONTROLLER_UCC_LISTENER_H__
