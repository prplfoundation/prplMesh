/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include <mapf/common/context.h>

namespace mapf {

Context &Context::Instance()
{
    static Context instance;
    return instance;
}

Context::~Context() { Close(); }

Context::Context() {}

void Context::Close() {}

void *Context::get() { return ctx_; }

} /* namespace mapf */