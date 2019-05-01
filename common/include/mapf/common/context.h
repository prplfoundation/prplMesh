/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef __MAPF_COMMON_CONTEXT_H__
#define __MAPF_COMMON_CONTEXT_H__

#include <mapf/common/logger.h>
#include <mapf/common/err.h>
#include <iostream>

namespace mapf
{

class Context
{
public:
	
	static Context& Instance();
	~Context();
	void* get();

private:
	Context();
	void Close();
	Context(const Context&) = delete;
	void operator = (const Context&) = delete;
	void *ctx_;
	
};

} /* namespace mapf */

#endif /* __MAPF_COMMON_CONTEXT_H__ */
