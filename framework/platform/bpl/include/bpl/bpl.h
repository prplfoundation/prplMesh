/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2019 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _BPL_H_
#define _BPL_H_

namespace beerocks {
namespace bpl {

/**
 * Initialize the BPL (Beerocks Platform Library).
 * This functions loads the platform specific shared object.
 *
 * @return 0 on success or a negative value on error.
 */
int bpl_init();

/**
 * Un-initialize the BPL.
 */
void bpl_close();

} // namespace bpl
} // namespace beerocks

#endif /* _BPL_H_ */
