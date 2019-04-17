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

#include <beerocks/bcl/beerocks_state_machine.h>

#include "../common/base_wlan_hal_types.h"

#include <chrono>

// Forward declarations
// namespace bwl {
// struct RadioInfo;
// }

struct wpa_ctrl;

namespace bwl {
namespace wav_fapi {

enum class fapi_fsm_state
{
    Delay,
    Init,
    GetRadioInfo,
    Attach,
    Operational,
    Detach
};

enum class fapi_fsm_event
{
    Attach,
    Detach
};

class fapi_attach_fsm : protected beerocks::beerocks_fsm<fapi_fsm_state, fapi_fsm_event>
{

// Public definitions
public:
    
    // FAPI event callback
    typedef std::function<bool(const std::string& opcode, std::shared_ptr<void> obj)> fapi_event_cb_t;

// Public methods:
public:

    fapi_attach_fsm(HALType type, RadioInfo& radio_info, fapi_event_cb_t cb);
    ~fapi_attach_fsm() = default;

    bool setup();
    fapi_fsm_state attach();
    bool detach();

    // Read the radio information from the hardware
    bool refresh_radio_info();

    // Refresh the VAPs information from the hardware
    bool refresh_vaps_info();

    //Read the interface information ,indication for interface and hostapd up.
    bool get_interface_info( std::string iface_name );
    
#ifdef USE_FAPI_DAEMON

    int get_wpa_ctrl_events_fd() const
    { return (m_fapi_event_fd); }

#else

    struct wpa_ctrl* get_wpa_ctrl_socket() const
    { return (m_wpa_ctrl); }

#endif

    // Inject FAPI event (from internal FAPI callback)
    void inject_fapi_event(const std::string opcode, void* obj);

    std::string state_enum_to_string(fapi_fsm_state state);

private:
    
    std::chrono::steady_clock::time_point   m_state_timeout;

    HALType                                 m_hal_type;
    RadioInfo&                              m_radio_info;

#ifdef USE_FAPI_DAEMON

    int                                     m_fapi_event_fd = -1;

#else

    struct wpa_ctrl*                        m_wpa_ctrl = nullptr;

#endif

    std::shared_ptr<char>                   m_temp_fapi_value;
    
    // FAPI events callback
    fapi_event_cb_t                         m_fapi_event_cb;
};

} // namespace bwl
} // namespace wav_fapi
