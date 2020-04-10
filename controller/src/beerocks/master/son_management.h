/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2019 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _SON_MANAGEMENT_H_
#define _SON_MANAGEMENT_H_

#include "son_master_thread.h"

namespace son {

class son_management {
public:
    static void handle_cli_message(Socket *sd,
                                   std::shared_ptr<beerocks::beerocks_header> beerocks_header,
                                   ieee1905_1::CmduMessageTx &cmdu_tx, db &database,
                                   task_pool &tasks);
    static void handle_bml_message(Socket *sd,
                                   std::shared_ptr<beerocks::beerocks_header> beerocks_header,
                                   ieee1905_1::CmduMessageTx &cmdu_tx, db &database,
                                   task_pool &tasks);
};

} // namespace son
#endif
