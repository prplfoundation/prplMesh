/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2019-2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include <iostream>
#include <memory>
#include <mutex>

namespace beerocks {

/**
 * @brief AgentDB is a class used by all main Agent threads on the Agent process to store the Agent
 * common data.
 * 
 * The class is implemented using singleton design pattern.
 * It is thread safe and being locked and released automatically.
 * 
 * How to use:
 * @code
 * auto db = AgentDB::get();            // Get DB and automatically lock it. The lock will be
 *                                      // Released when 'db' will be destructed.
 * 
 * db->foo = 42;                        // Change database member 'foo' to value 42.
 * db->bar = "prpl"                     // Set another database member.
 * auto &my_foo = db->foo;              // Get a refernce to a member.
 * 
 * AgentDB::get()->foo = 44;            // Set database member directly.
 * 
 * 
 * Unsafe operation which should never be done:
 * auto my_foo = AgentDB::get()->foo;   // Unsafe! Don't do it! The database might be unlocked,
 *                                      // if used on ranged loop. So better not use it.
 * 
 * auto &foo = AgentDB::get()->foo;     // Unsafe! Don't do it! The database will be unlocked,
 *                                      // but the reference will remain.
 * 
 * std::string &get_foo() {             // Unsafe! Don't do it! Returning refernce to the database
 *     auto db = AgentDB::get();        // on a wrapper function is unsafe because the database will
 *     return db->foo;                  // be unlocked when the function ends, and the caller will
 * }                                    // hold a refernce to it.                                 // hold a refernce to it.
 * @endcode
 */
class AgentDB {
public:
    class SafeDB {
    public:
        SafeDB(AgentDB &db) : m_db(db) { m_db.db_lock(); }
        ~SafeDB() { m_db.db_unlock(); }
        AgentDB *operator->() { return &m_db; }

    private:
        AgentDB &m_db;
    };
    static SafeDB get()
    {
        // Guaranteed to be destroyed.
        // Instantiated on first use.
        static AgentDB instance;
        return SafeDB(instance);
    }
    AgentDB(const AgentDB &) = delete;
    void operator=(const AgentDB &) = delete;

private:
    // Private constructor so that no objects can be created.
    AgentDB() = default;
    std::recursive_mutex m_db_mutex;
    void db_lock() { m_db_mutex.lock(); }
    void db_unlock() { m_db_mutex.unlock(); }

public:
    /* Put here database members used by the Agent modules */
};

} // namespace beerocks
