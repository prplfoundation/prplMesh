/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include "../../common/utils/utils.h"
#include <bpl/bpl_cfg.h>

#include "bpl_cfg_helper.h"
#include "bpl_cfg_uci.h"

#include "mapf/common/logger.h"

using namespace mapf;

using namespace beerocks::bpl;

#ifndef BPL_PASSIVE_MODE

//============================================ STATIC START ============================================

//============================================ STATIC END ============================================

int bpl_cfg_set_onboarding(int enable) { return 0; }

#else //PASSIVE_MODE is on

int bpl_cfg_set_onboarding(int enable) { return 0; }

#endif
