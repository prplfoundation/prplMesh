/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _BWL_BASE_WLAN_HAL_DUMMY_H_
#define _BWL_BASE_WLAN_HAL_DUMMY_H_

#include <bwl/base_wlan_hal.h>

#include <bcl/beerocks_state_machine.h>

#include <chrono>
#include <memory>

#define DUMMY_EVENT_KEYLESS_PARAM_TYPE "_type"
#define DUMMY_EVENT_KEYLESS_PARAM_OPCODE "_opcode"
#define DUMMY_EVENT_KEYLESS_PARAM_MAC "_mac"
#define DUMMY_EVENT_KEYLESS_PARAM_IFACE "_iface"

namespace bwl {
namespace dummy {

enum class dummy_fsm_state { Delay, Init, GetRadioInfo, Attach, Operational, Detach };

enum class dummy_fsm_event { Attach, Detach };

typedef std::unordered_map<std::string, std::string> parsed_obj_map_t;
typedef std::list<parsed_obj_map_t> parsed_obj_listed_map_t;

/*!
 * Base class for the dummy abstraction layer.
 * Read more about virtual inheritance: https://en.wikipedia.org/wiki/Virtual_inheritance
 */
class base_wlan_hal_dummy : public virtual base_wlan_hal,
                            protected beerocks::beerocks_fsm<dummy_fsm_state, dummy_fsm_event> {

    // Public methods:
public:
    virtual ~base_wlan_hal_dummy();

    virtual HALState attach(bool block = false) override;
    virtual bool detach() override;
    virtual bool ping() override;
    virtual bool refresh_radio_info() override;
    virtual bool refresh_vaps_info(int id) override;
    virtual bool process_ext_events() override;
    virtual bool process_nl_events() override;
    virtual std::string get_radio_mac() override;

    /**
     * @brief Gets channel utilization.
     *
     * @see base_wlan_hal::get_channel_utilization
     *
     * Returns a fake channel utilization value, varying from 0 to UINT8_MAX on each call.
     *
     * @param[out] channel_utilization Channel utilization value.
     *
     * @return True on success and false otherwise.
     */
    bool get_channel_utilization(uint8_t &channel_utilization) override;

    // Protected methods
protected:
    base_wlan_hal_dummy(HALType type, const std::string &iface_name, hal_event_cb_t callback,
                        const hal_conf_t &hal_conf = {});

    void parsed_obj_debug(parsed_obj_map_t &obj);
    void parsed_obj_debug(parsed_obj_listed_map_t &obj);

    bool dummy_obj_read_int(const std::string &key, parsed_obj_map_t &obj, int64_t &value,
                            bool ignore_unknown = false);
    bool dummy_obj_read_str(const std::string &key, parsed_obj_map_t &obj, char **value);

    // Process dummy event
    virtual bool process_dummy_event(parsed_obj_map_t &parsed_obj) = 0;

    // Process dummy data
    virtual bool process_dummy_data(parsed_obj_map_t &parsed_obj) = 0;

    virtual bool set(const std::string &param, const std::string &value,
                     int vap_id = beerocks::IFACE_RADIO_ID) override;

    /** @brief Write/update a dummy status file.
     *
     * Write the @a value to a status file with name @a filename. This is used by test scripts to
     * verify if the proper bwl commands were given.
     */
    bool write_status_file(const std::string &filename, const std::string &value) const;

    /** @brief Get the status directory.
     *
     * The dummy implementation interacts with the test framework through files in the test
     * directory. This function returns the path to that directory.
     */
    std::string get_status_dir() const
    {
        return std::string(BEEROCKS_TMP_PATH) + "/" + get_iface_name();
    }

    static const int predefined_vaps_num = 4;

    // Private data-members:
private:
    const uint32_t AP_ENABELED_TIMEOUT_SEC           = 15;
    const uint32_t AP_ENABELED_FIXED_DFS_TIMEOUT_SEC = 660;

    bool fsm_setup();
    bool refresh_vap_info(int vap_id);

    dummy_fsm_state m_last_attach_state = dummy_fsm_state::Detach;

    std::chrono::steady_clock::time_point m_state_timeout;
};

} // namespace dummy
} // namespace bwl

#endif // _BWL_BASE_WLAN_HAL_DUMMY_H_
