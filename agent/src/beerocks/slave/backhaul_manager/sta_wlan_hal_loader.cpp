/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

// Monitor HAL
#include <bwl/sta_wlan_hal.h>

#include <dlfcn.h> // Dynamic loading

#include <easylogging++.h>

//////////////////////////////////////////////////////////////////////////////
////////////////////////// Local Module Definitions //////////////////////////
//////////////////////////////////////////////////////////////////////////////

// The file name of the BWL shared library
#ifndef BWL_MODULE_NAME
#define BWL_MODULE_NAME "libbwl.so"
#endif

//////////////////////////////////////////////////////////////////////////////
/////////////////////////// Local Module Variables ///////////////////////////
//////////////////////////////////////////////////////////////////////////////

static std::shared_ptr<void> s_sharedLib;

static sta_wlan_hal_create_t s_obj_create   = nullptr;
static sta_wlan_hal_destroy_t s_obj_destroy = nullptr;

//////////////////////////////////////////////////////////////////////////////
/////////////////////////////// Implementation ///////////////////////////////
//////////////////////////////////////////////////////////////////////////////

extern "C" {

bwl::sta_wlan_hal *sta_wlan_hal_create(std::string iface_name,
                                       bwl::base_wlan_hal::hal_event_cb_t cb)
{
    // Load the shared library on demand
    if (!s_sharedLib) {

        // Open the shared library
        s_sharedLib = std::shared_ptr<void>(dlopen(BWL_MODULE_NAME, RTLD_LAZY),
                                            // dlopen(BWL_MODULE_NAME, RTLD_NOW | RTLD_GLOBAL),
                                            [](void *module) {
                                                if (module) {
                                                    dlclose(module);
                                                }
                                            });

        if (!s_sharedLib) {
            return nullptr;
        }

        // Reset errors
        dlerror();

        // Creator function
        s_obj_create     = (sta_wlan_hal_create_t)(dlsym(s_sharedLib.get(), "sta_wlan_hal_create"));
        auto dlsym_error = dlerror();
        if (!s_obj_create || dlsym_error) {
            LOG(ERROR) << "Failed loading 'sta_wlan_hal_create': " << dlsym_error;
            return nullptr;
        }

        // Destroy function
        s_obj_destroy = (sta_wlan_hal_destroy_t)(dlsym(s_sharedLib.get(), "sta_wlan_hal_destroy"));
        dlsym_error   = dlerror();
        if (!s_obj_destroy || dlsym_error) {
            LOG(ERROR) << "Failed loading 'sta_wlan_hal_destroy': " << dlsym_error;
            return nullptr;
        }
    }

    // Create and return a new instance of the HAL class
    return s_obj_create(iface_name, cb);
}

void sta_wlan_hal_destroy(bwl::sta_wlan_hal *obj)
{
    if (!s_obj_destroy) {
        LOG(FATAL) << "Attempted destroy of 'sta_wlan_hal' object without creation!";
    } else {
        // Destroy the object
        s_obj_destroy(obj);
    }
}
}
