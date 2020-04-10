/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2019 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _BEEROCKS_STATE_MACHINE_H_
#define _BEEROCKS_STATE_MACHINE_H_

#include <functional>
#include <list>
#include <unordered_map>

#include <easylogging++.h>

namespace beerocks {

// Support for enum class
struct EnumClassHash {
    template <typename T> std::size_t operator()(T t) const { return static_cast<std::size_t>(t); }
};

template <typename Key>
using HashType =
    typename std::conditional<std::is_enum<Key>::value, EnumClassHash, std::hash<Key>>::type;

template <typename Key, typename T>
using TypedUnorderedMap = std::unordered_map<Key, T, HashType<Key>>;

/*!
 * Beerocks FSM Transition - Represents a transition of the state machine
 * from "Source" state to "Destination" state on "Evnet"
 */
template <typename TState, typename TEvent> class beerocks_fsm_transition {
    // Public methods:
public:
    // Constructor
    beerocks_fsm_transition(const TState &source, const TState &destination, const TEvent &event)
        : m_source(source), m_destination(destination), m_event(event)
    {
    }

    // Getter methods
    const TState &source() const { return m_source; }
    const TState &destination() const { return m_destination; }
    const TEvent &event() const { return m_event; }

    // Update the destination of the transition (not the default state)
    bool change_destination(const TState &dest)
    {
        LOG(INFO) << "'" << m_source << "': Overriding destination for '" << m_event << "' from '"
                  << m_destination << "' to '" << dest << "'";

        m_destination = dest;
        return true;
    }

    // Private data-members:
private:
    // Source state
    const TState m_source;

    // Destination state
    TState m_destination;

    // Event (trigger)
    const TEvent m_event;
};

/*!
 * Beerocks FSM Event Handler - Processes fired events
 */
template <typename TState, typename TEvent> class beerocks_fsm_event {
    // Public definitions:
public:
    // FSM transition
    using TTransition = beerocks_fsm_transition<TState, TEvent>;

    // FSM transition condition function
    using TCondition = std::function<bool(TTransition &, const void *)>;

    // Public methods:
public:
    // Constructor
    beerocks_fsm_event(const TEvent &event, const std::list<TState> &target_states,
                       const TCondition &cond)
        : m_event(event), m_allowed_states(target_states), m_cond(cond)
    {
    }

    // Getter methods
    const TEvent &event() const { return m_event; }
    const TState &target_state() const
    {
        // Return the default state
        return m_allowed_states.front();
    }

    // Check the event transition condition
    bool check_condition(TTransition &transition, const void *args = nullptr)
    {
        // Support empty conditions
        if (!m_cond)
            return true;

        TState org_dest = transition.destination();

        // Evaluate the condition
        if (!m_cond(transition, args))
            return false;

        // In case the destination state was modified by the condition function,
        // check the new destination is allowed...
        if (org_dest != transition.destination()) {
            if (std::find(m_allowed_states.begin(), m_allowed_states.end(),
                          transition.destination()) == m_allowed_states.end()) {
                LOG(ERROR) << "Invalid transition from state '" << transition.source()
                           << "' to state '" << transition.destination() << "'";

                return false;
            }
        }

        return true;
    }

    // Private data-members:
private:
    // Event
    TEvent m_event;

    // Allowed target states for the current event
    std::list<TState> m_allowed_states;

    // Event transition function
    TCondition m_cond;
};

/*!
 * Beerocks FSM State Handler - Implements state related operations:
 * State entry, state exit, event processing
 */
template <typename TState, typename TEvent> class beerocks_fsm_state {
    // Public definitions:
public:
    // FSM Event Handler
    typedef beerocks_fsm_event<TState, TEvent> TEventHandler;

    // Simple (non-transional) action
    typedef std::function<bool(const void *)> TAction;

    // Inherited types
    typedef typename TEventHandler::TTransition TTransition;
    typedef typename TEventHandler::TCondition TTransAction;

    // Public methods:
public:
    // Constructor
    beerocks_fsm_state(const TState &state) : m_state(state) {}

    // Set handler for state entry
    void set_entry_action(const TAction &action) { m_entry_action = action; }

    // Set handler for state exit
    void set_exit_action(const TAction &action) { m_exit_action = action; }

    // Add state event handler
    void add_event(const std::shared_ptr<TEventHandler> &event_handler)
    {
        if (!event_handler) {
            LOG(FATAL) << "Invalid event handler pointer!";
        }

        if (m_events.find(event_handler->event()) != m_events.end()) {
            LOG(FATAL) << "Event '" << event_handler->event() << "' for State '" << m_state
                       << "' already exists!";
        }

        // Add the event
        m_events[event_handler->event()] = event_handler;
    }

    // Handle state entry
    bool enter(const void *args = nullptr) const
    {
        // Allow empty entry actions
        if (!m_entry_action)
            return true;
        return (m_entry_action(args));
    }

    // Handle state exit
    bool exit(const void *args = nullptr) const
    {
        // Allow empty exit actions
        if (!m_exit_action)
            return true;
        return (m_exit_action(args));
    }

    // Check whether the current state can handle the given event
    bool can_handle(const TEvent &event) { return (m_events.find(event) != m_events.end()); }

    // Process an event and return the next state
    const TState &handle(const TEvent &event, const void *args = nullptr)
    {
        // Check if the given event was registered for this state
        auto iter = m_events.find(event);
        if (iter == m_events.end()) {
            LOG(WARNING) << "State '" << m_state << "' can't handle event '" << event << "'";
            return m_state;
        }

        // Construct a new transition object and execute the event condition function
        auto trigger_handler = iter->second;
        TTransition transition(m_state, trigger_handler->target_state(), event);
        if (trigger_handler->check_condition(transition, args)) {
            return transition.destination();
        }

        // Remain in the current state
        return m_state;
    }

    // Private data-members:
private:
    // State
    const TState m_state;

    // Entry/Exit functions
    TAction m_entry_action;
    TAction m_exit_action;

    // Map of allowed state events
    //std::unordered_map<TEvent, std::shared_ptr<TEventHandler>> m_events;
    TypedUnorderedMap<TEvent, std::shared_ptr<TEventHandler>> m_events;
};

/*!
 * Beerocks FSM Configuration
 */
template <typename TState, typename TEvent> class beerocks_fsm_config {
    // Public definitions
public:
    typedef beerocks_fsm_state<TState, TEvent> TStateHandler;

    // Inherited types
    typedef typename TStateHandler::TTransition TTransition;
    typedef typename TStateHandler::TTransAction TTransAction;
    typedef typename TStateHandler::TAction TAction;
    typedef typename TStateHandler::TEventHandler TEventHandler;

    // Pointer to state handler class
    typedef std::shared_ptr<TStateHandler> TStateHandlerPtr;

    // Configuration states structure
    //typedef std::unordered_map<TState, std::shared_ptr<TStateHandler>> TConfStates;
    typedef TypedUnorderedMap<TState, std::shared_ptr<TStateHandler>> TConfStates;

    // Public methods
public:
    // Configure current state
    beerocks_fsm_config &state(const TState &state)
    {
        // Make sure the state doesn't exists in the configuration
        if (m_states.find(state) != m_states.end()) {
            LOG(FATAL) << "State '" << state << "' already exists in the configuration!";
        }

        // Create new state
        m_curr_state    = std::make_shared<TStateHandler>(state);
        m_states[state] = m_curr_state;

        return *this;
    };

    // Set entry function
    beerocks_fsm_config &entry(const TAction &action)
    {
        if (!m_curr_state) {
            LOG(FATAL) << "Invalid state handler!";
        }

        m_curr_state->set_entry_action(action);
        return *this;
    };

    // Set exit function
    beerocks_fsm_config &exit(const TAction &action)
    {
        if (!m_curr_state) {
            LOG(FATAL) << "Invalid state handler!";
        }

        m_curr_state->set_exit_action(action);
        return *this;
    };

    // Configure event handling logic
    beerocks_fsm_config &on(const TEvent &event, const std::list<TState> &states,
                            const TTransAction &cond = {})
    {
        if (!m_curr_state) {
            LOG(FATAL) << "Invalid state handler!";
        }

        if (!states.size()) {
            LOG(FATAL) << "At least 1 target state should be defined for event '" << event << "'";
        }

        // Create an event handler and add it to the state
        auto event_handler = std::make_shared<TEventHandler>(event, states, cond);
        m_curr_state->add_event(event_handler);

        return *this;
    };

    beerocks_fsm_config &on(const TEvent &trigger, const TState &state,
                            const TTransAction &cond = {})
    {
        return on(trigger, std::list<TState>({state}), cond);
    };

    // Return a list of configured states
    const TConfStates &states() const { return m_states; }

private:
    // States
    TConfStates m_states;

    // Current state (used during configuration)
    TStateHandlerPtr m_curr_state;
};

/*!
 * Beerocks FSM Implementation
 */
template <typename TState, typename TEvent> class beerocks_fsm {
    // Public definitions
public:
    // FSM Configuration
    typedef beerocks_fsm_config<TState, TEvent> TConfiguration;
    typedef typename TConfiguration::TTransition TTransition;

    // Public methods
public:
    // Constructor
    beerocks_fsm(TState init_state)
        : m_init_state(init_state), m_curr_state(init_state), m_started(false)
    {
    }

    // FSM Configuration
    TConfiguration &config() { return m_config; }

    // Return current FSM state
    const TState &state() const { return m_curr_state; }

    // Initialize the FSM
    bool start(const void *args = nullptr)
    {
        if (m_started) {
            LOG(WARNING) << "FSM already started";
            return true;
        }

        // Make sure that the initial state is configured
        auto iter = m_config.states().find(m_init_state);
        if (iter == m_config.states().end()) {
            LOG(FATAL) << "Invalid initial state '" << m_init_state << "'";
            return false;
        }

        auto state_handler = iter->second;
        m_started          = true;

        // Enter the initial state
        return state_handler->enter(args);
    }

    // Handle event
    bool fire(const TEvent &event, const void *args = nullptr)
    {
        if (!m_started) {
            LOG(ERROR) << "FSM not started!";
            return false;
        }

        // Find the current state
        auto iter = m_config.states().find(m_curr_state);
        if (iter == m_config.states().end()) {
            LOG(FATAL) << "Invalid state '" << m_curr_state << "'";
            return false;
        }

        // Check if the state is able to handle the given event
        auto state_handler = iter->second;
        if (!state_handler->can_handle(event)) {
            LOG(WARNING) << "Current state '" << m_curr_state << "' unable to handle event '"
                         << event << "'";
            return false;
        }

        // Handle the event
        auto new_state = state_handler->handle(event, args);

        // Check if the FSM should switch to a new state
        if (new_state != m_curr_state) {

            // Exit current state
            state_handler->exit(args);

            // Check if the new state is configured
            iter = m_config.states().find(new_state);
            if (iter == m_config.states().end()) {
                LOG(ERROR) << "Unable to transition to undefined state '" << new_state << "'";
                return false;
            }
            state_handler = iter->second;

            // Enter the new state
            state_handler->enter(args);
            m_curr_state = new_state;
        }

        return true;
    }

    // Private data-members:
private:
    // Initial state
    const TState m_init_state;

    // Current state
    TState m_curr_state;

    // FSM Started
    bool m_started = false;

    // FSM Configuration
    TConfiguration m_config;
};

} // namespace beerocks

#endif // _BEEROCKS_STATE_MACHINE_H_