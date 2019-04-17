/*
 * INTEL CONFIDENTIAL
 * Copyright 2016 Intel Corporation All Rights Reserved.
 *
 * The source code contained or described herein and all documents related to
 * the source code ("Material") are owned by Intel Corporation or its
 * suppliers or licensors.  Title to the Material remains with Intel
 * Corporation or its suppliers and licensors.  The Material contains trade
 * secrets and proprietary and confidential information of Intel or its
 * suppliers and licensors.  The Material is protected by worldwide copyright
 * and trade secret laws and treaty provisions. No part of the Material may
 * be used, copied, reproduced, modified, published, uploaded, posted,
 * transmitted, distributed, or disclosed in any way without Intel's prior
 * express written permission.
 *
 * No license under any patent, copyright, trade secret or other intellectual
 * property right is granted to or conferred upon you by disclosure or
 * delivery of the Materials,  either expressly, by implication, inducement,
 * estoppel or otherwise.  Any license under such intellectual property
 * rights must be express and approved by Intel in writing.
 */

#ifndef _BPL_H_
#define _BPL_H_

#ifdef __cplusplus
extern "C" {
#endif

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

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* _BPL_H_ */
