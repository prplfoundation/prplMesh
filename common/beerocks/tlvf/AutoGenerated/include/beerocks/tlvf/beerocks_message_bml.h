///////////////////////////////////////
// AUTO GENERATED FILE - DO NOT EDIT //
///////////////////////////////////////

/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _BEEROCKS_TLVF_BEEROCKS_MESSAGE_BML_H_
#define _BEEROCKS_TLVF_BEEROCKS_MESSAGE_BML_H_

#include <cstddef>
#include <stdint.h>
#include <tlvf/swap.h>
#include <string.h>
#include <memory>
#include <tlvf/BaseClass.h>
#include <tuple>
#include <tlvf/tlvfutils.h>
#include "beerocks/tlvf/beerocks_message_common.h"

namespace beerocks_message {


class cACTION_BML_PING_REQUEST : public BaseClass
{
    public:
        cACTION_BML_PING_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_BML_PING_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_BML_PING_REQUEST();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_PING_REQUEST);
        }
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
};

class cACTION_BML_PING_RESPONSE : public BaseClass
{
    public:
        cACTION_BML_PING_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_BML_PING_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_BML_PING_RESPONSE();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_PING_RESPONSE);
        }
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
};

class cACTION_BML_NW_MAP_REQUEST : public BaseClass
{
    public:
        cACTION_BML_NW_MAP_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_BML_NW_MAP_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_BML_NW_MAP_REQUEST();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_NW_MAP_REQUEST);
        }
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
};

class cACTION_BML_NW_MAP_RESPONSE : public BaseClass
{
    public:
        cACTION_BML_NW_MAP_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_BML_NW_MAP_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_BML_NW_MAP_RESPONSE();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_NW_MAP_RESPONSE);
        }
        uint32_t& node_num();
        uint32_t& buffer_size();
        char* buffer(size_t length = 0);
        bool set_buffer(std::string& str);
        bool set_buffer(const std::string& str);
        bool set_buffer(const char buffer[], size_t size);
        bool alloc_buffer(size_t count = 1);
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
        uint32_t* m_node_num = nullptr;
        uint32_t* m_buffer_size = nullptr;
        char* m_buffer = nullptr;
        size_t m_buffer_idx__ = 0;
        int m_lock_order_counter__ = 0;
};

class cACTION_BML_NW_MAP_UPDATE : public BaseClass
{
    public:
        cACTION_BML_NW_MAP_UPDATE(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_BML_NW_MAP_UPDATE(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_BML_NW_MAP_UPDATE();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_NW_MAP_UPDATE);
        }
        uint32_t& node_num();
        uint32_t& buffer_size();
        char* buffer(size_t length = 0);
        bool set_buffer(std::string& str);
        bool set_buffer(const std::string& str);
        bool set_buffer(const char buffer[], size_t size);
        bool alloc_buffer(size_t count = 1);
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
        uint32_t* m_node_num = nullptr;
        uint32_t* m_buffer_size = nullptr;
        char* m_buffer = nullptr;
        size_t m_buffer_idx__ = 0;
        int m_lock_order_counter__ = 0;
};

class cACTION_BML_STATS_UPDATE : public BaseClass
{
    public:
        cACTION_BML_STATS_UPDATE(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_BML_STATS_UPDATE(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_BML_STATS_UPDATE();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_STATS_UPDATE);
        }
        uint32_t& num_of_stats_bulks();
        uint32_t& buffer_size();
        char* buffer(size_t length = 0);
        bool set_buffer(std::string& str);
        bool set_buffer(const std::string& str);
        bool set_buffer(const char buffer[], size_t size);
        bool alloc_buffer(size_t count = 1);
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
        uint32_t* m_num_of_stats_bulks = nullptr;
        uint32_t* m_buffer_size = nullptr;
        char* m_buffer = nullptr;
        size_t m_buffer_idx__ = 0;
        int m_lock_order_counter__ = 0;
};

class cACTION_BML_EVENTS_UPDATE : public BaseClass
{
    public:
        cACTION_BML_EVENTS_UPDATE(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_BML_EVENTS_UPDATE(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_BML_EVENTS_UPDATE();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_EVENTS_UPDATE);
        }
        uint32_t& buffer_size();
        char* buffer(size_t length = 0);
        bool set_buffer(std::string& str);
        bool set_buffer(const std::string& str);
        bool set_buffer(const char buffer[], size_t size);
        bool alloc_buffer(size_t count = 1);
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
        uint32_t* m_buffer_size = nullptr;
        char* m_buffer = nullptr;
        size_t m_buffer_idx__ = 0;
        int m_lock_order_counter__ = 0;
};

class cACTION_BML_REGISTER_TO_NW_MAP_UPDATES_REQUEST : public BaseClass
{
    public:
        cACTION_BML_REGISTER_TO_NW_MAP_UPDATES_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_BML_REGISTER_TO_NW_MAP_UPDATES_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_BML_REGISTER_TO_NW_MAP_UPDATES_REQUEST();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_REGISTER_TO_NW_MAP_UPDATES_REQUEST);
        }
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
};

class cACTION_BML_REGISTER_TO_NW_MAP_UPDATES_RESPONSE : public BaseClass
{
    public:
        cACTION_BML_REGISTER_TO_NW_MAP_UPDATES_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_BML_REGISTER_TO_NW_MAP_UPDATES_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_BML_REGISTER_TO_NW_MAP_UPDATES_RESPONSE();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_REGISTER_TO_NW_MAP_UPDATES_RESPONSE);
        }
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
};

class cACTION_BML_UNREGISTER_FROM_NW_MAP_UPDATES_REQUEST : public BaseClass
{
    public:
        cACTION_BML_UNREGISTER_FROM_NW_MAP_UPDATES_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_BML_UNREGISTER_FROM_NW_MAP_UPDATES_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_BML_UNREGISTER_FROM_NW_MAP_UPDATES_REQUEST();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_UNREGISTER_FROM_NW_MAP_UPDATES_REQUEST);
        }
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
};

class cACTION_BML_UNREGISTER_FROM_NW_MAP_UPDATES_RESPONSE : public BaseClass
{
    public:
        cACTION_BML_UNREGISTER_FROM_NW_MAP_UPDATES_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_BML_UNREGISTER_FROM_NW_MAP_UPDATES_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_BML_UNREGISTER_FROM_NW_MAP_UPDATES_RESPONSE();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_UNREGISTER_FROM_NW_MAP_UPDATES_RESPONSE);
        }
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
};

class cACTION_BML_SET_LEGACY_CLIENT_ROAMING_RESPONSE : public BaseClass
{
    public:
        cACTION_BML_SET_LEGACY_CLIENT_ROAMING_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_BML_SET_LEGACY_CLIENT_ROAMING_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_BML_SET_LEGACY_CLIENT_ROAMING_RESPONSE();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_SET_LEGACY_CLIENT_ROAMING_RESPONSE);
        }
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
};

class cACTION_BML_GET_LEGACY_CLIENT_ROAMING_REQUEST : public BaseClass
{
    public:
        cACTION_BML_GET_LEGACY_CLIENT_ROAMING_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_BML_GET_LEGACY_CLIENT_ROAMING_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_BML_GET_LEGACY_CLIENT_ROAMING_REQUEST();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_GET_LEGACY_CLIENT_ROAMING_REQUEST);
        }
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
};

class cACTION_BML_REGISTER_TO_EVENTS_UPDATES_REQUEST : public BaseClass
{
    public:
        cACTION_BML_REGISTER_TO_EVENTS_UPDATES_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_BML_REGISTER_TO_EVENTS_UPDATES_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_BML_REGISTER_TO_EVENTS_UPDATES_REQUEST();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_REGISTER_TO_EVENTS_UPDATES_REQUEST);
        }
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
};

class cACTION_BML_REGISTER_TO_EVENTS_UPDATES_RESPONSE : public BaseClass
{
    public:
        cACTION_BML_REGISTER_TO_EVENTS_UPDATES_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_BML_REGISTER_TO_EVENTS_UPDATES_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_BML_REGISTER_TO_EVENTS_UPDATES_RESPONSE();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_REGISTER_TO_EVENTS_UPDATES_RESPONSE);
        }
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
};

class cACTION_BML_UNREGISTER_FROM_EVENTS_UPDATES_REQUEST : public BaseClass
{
    public:
        cACTION_BML_UNREGISTER_FROM_EVENTS_UPDATES_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_BML_UNREGISTER_FROM_EVENTS_UPDATES_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_BML_UNREGISTER_FROM_EVENTS_UPDATES_REQUEST();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_UNREGISTER_FROM_EVENTS_UPDATES_REQUEST);
        }
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
};

class cACTION_BML_UNREGISTER_FROM_EVENTS_UPDATES_RESPONSE : public BaseClass
{
    public:
        cACTION_BML_UNREGISTER_FROM_EVENTS_UPDATES_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_BML_UNREGISTER_FROM_EVENTS_UPDATES_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_BML_UNREGISTER_FROM_EVENTS_UPDATES_RESPONSE();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_UNREGISTER_FROM_EVENTS_UPDATES_RESPONSE);
        }
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
};

class cACTION_BML_REGISTER_TO_STATS_UPDATES_REQUEST : public BaseClass
{
    public:
        cACTION_BML_REGISTER_TO_STATS_UPDATES_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_BML_REGISTER_TO_STATS_UPDATES_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_BML_REGISTER_TO_STATS_UPDATES_REQUEST();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_REGISTER_TO_STATS_UPDATES_REQUEST);
        }
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
};

class cACTION_BML_REGISTER_TO_STATS_UPDATES_RESPONSE : public BaseClass
{
    public:
        cACTION_BML_REGISTER_TO_STATS_UPDATES_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_BML_REGISTER_TO_STATS_UPDATES_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_BML_REGISTER_TO_STATS_UPDATES_RESPONSE();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_REGISTER_TO_STATS_UPDATES_RESPONSE);
        }
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
};

class cACTION_BML_UNREGISTER_FROM_STATS_UPDATES_REQUEST : public BaseClass
{
    public:
        cACTION_BML_UNREGISTER_FROM_STATS_UPDATES_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_BML_UNREGISTER_FROM_STATS_UPDATES_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_BML_UNREGISTER_FROM_STATS_UPDATES_REQUEST();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_UNREGISTER_FROM_STATS_UPDATES_REQUEST);
        }
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
};

class cACTION_BML_UNREGISTER_FROM_STATS_UPDATES_RESPONSE : public BaseClass
{
    public:
        cACTION_BML_UNREGISTER_FROM_STATS_UPDATES_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_BML_UNREGISTER_FROM_STATS_UPDATES_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_BML_UNREGISTER_FROM_STATS_UPDATES_RESPONSE();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_UNREGISTER_FROM_STATS_UPDATES_RESPONSE);
        }
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
};

class cACTION_BML_SET_LEGACY_CLIENT_ROAMING_REQUEST : public BaseClass
{
    public:
        cACTION_BML_SET_LEGACY_CLIENT_ROAMING_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_BML_SET_LEGACY_CLIENT_ROAMING_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_BML_SET_LEGACY_CLIENT_ROAMING_REQUEST();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_SET_LEGACY_CLIENT_ROAMING_REQUEST);
        }
        uint8_t& isEnable();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
        uint8_t* m_isEnable = nullptr;
};

class cACTION_BML_GET_LEGACY_CLIENT_ROAMING_RESPONSE : public BaseClass
{
    public:
        cACTION_BML_GET_LEGACY_CLIENT_ROAMING_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_BML_GET_LEGACY_CLIENT_ROAMING_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_BML_GET_LEGACY_CLIENT_ROAMING_RESPONSE();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_GET_LEGACY_CLIENT_ROAMING_RESPONSE);
        }
        uint8_t& isEnable();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
        uint8_t* m_isEnable = nullptr;
};

class cACTION_BML_SET_CLIENT_ROAMING_REQUEST : public BaseClass
{
    public:
        cACTION_BML_SET_CLIENT_ROAMING_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_BML_SET_CLIENT_ROAMING_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_BML_SET_CLIENT_ROAMING_REQUEST();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_SET_CLIENT_ROAMING_REQUEST);
        }
        uint8_t& isEnable();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
        uint8_t* m_isEnable = nullptr;
};

class cACTION_BML_SET_CLIENT_ROAMING_RESPONSE : public BaseClass
{
    public:
        cACTION_BML_SET_CLIENT_ROAMING_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_BML_SET_CLIENT_ROAMING_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_BML_SET_CLIENT_ROAMING_RESPONSE();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_SET_CLIENT_ROAMING_RESPONSE);
        }
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
};

class cACTION_BML_GET_CLIENT_ROAMING_REQUEST : public BaseClass
{
    public:
        cACTION_BML_GET_CLIENT_ROAMING_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_BML_GET_CLIENT_ROAMING_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_BML_GET_CLIENT_ROAMING_REQUEST();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_GET_CLIENT_ROAMING_REQUEST);
        }
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
};

class cACTION_BML_GET_CLIENT_ROAMING_RESPONSE : public BaseClass
{
    public:
        cACTION_BML_GET_CLIENT_ROAMING_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_BML_GET_CLIENT_ROAMING_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_BML_GET_CLIENT_ROAMING_RESPONSE();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_GET_CLIENT_ROAMING_RESPONSE);
        }
        uint8_t& isEnable();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
        uint8_t* m_isEnable = nullptr;
};

class cACTION_BML_SET_DFS_REENTRY_REQUEST : public BaseClass
{
    public:
        cACTION_BML_SET_DFS_REENTRY_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_BML_SET_DFS_REENTRY_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_BML_SET_DFS_REENTRY_REQUEST();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_SET_DFS_REENTRY_REQUEST);
        }
        uint8_t& isEnable();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
        uint8_t* m_isEnable = nullptr;
};

class cACTION_BML_SET_DFS_REENTRY_RESPONSE : public BaseClass
{
    public:
        cACTION_BML_SET_DFS_REENTRY_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_BML_SET_DFS_REENTRY_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_BML_SET_DFS_REENTRY_RESPONSE();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_SET_DFS_REENTRY_RESPONSE);
        }
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
};

class cACTION_BML_GET_DFS_REENTRY_REQUEST : public BaseClass
{
    public:
        cACTION_BML_GET_DFS_REENTRY_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_BML_GET_DFS_REENTRY_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_BML_GET_DFS_REENTRY_REQUEST();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_GET_DFS_REENTRY_REQUEST);
        }
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
};

class cACTION_BML_GET_DFS_REENTRY_RESPONSE : public BaseClass
{
    public:
        cACTION_BML_GET_DFS_REENTRY_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_BML_GET_DFS_REENTRY_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_BML_GET_DFS_REENTRY_RESPONSE();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_GET_DFS_REENTRY_RESPONSE);
        }
        uint8_t& isEnable();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
        uint8_t* m_isEnable = nullptr;
};

class cACTION_BML_SET_CLIENT_ROAMING_PREFER_SIGNAL_STRENGTH_REQUEST : public BaseClass
{
    public:
        cACTION_BML_SET_CLIENT_ROAMING_PREFER_SIGNAL_STRENGTH_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_BML_SET_CLIENT_ROAMING_PREFER_SIGNAL_STRENGTH_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_BML_SET_CLIENT_ROAMING_PREFER_SIGNAL_STRENGTH_REQUEST();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_SET_CLIENT_ROAMING_PREFER_SIGNAL_STRENGTH_REQUEST);
        }
        uint8_t& isEnable();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
        uint8_t* m_isEnable = nullptr;
};

class cACTION_BML_SET_CLIENT_ROAMING_PREFER_SIGNAL_STRENGTH_RESPONSE : public BaseClass
{
    public:
        cACTION_BML_SET_CLIENT_ROAMING_PREFER_SIGNAL_STRENGTH_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_BML_SET_CLIENT_ROAMING_PREFER_SIGNAL_STRENGTH_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_BML_SET_CLIENT_ROAMING_PREFER_SIGNAL_STRENGTH_RESPONSE();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_SET_CLIENT_ROAMING_PREFER_SIGNAL_STRENGTH_RESPONSE);
        }
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
};

class cACTION_BML_GET_CLIENT_ROAMING_PREFER_SIGNAL_STRENGTH_REQUEST : public BaseClass
{
    public:
        cACTION_BML_GET_CLIENT_ROAMING_PREFER_SIGNAL_STRENGTH_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_BML_GET_CLIENT_ROAMING_PREFER_SIGNAL_STRENGTH_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_BML_GET_CLIENT_ROAMING_PREFER_SIGNAL_STRENGTH_REQUEST();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_GET_CLIENT_ROAMING_PREFER_SIGNAL_STRENGTH_REQUEST);
        }
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
};

class cACTION_BML_GET_CLIENT_ROAMING_PREFER_SIGNAL_STRENGTH_RESPONSE : public BaseClass
{
    public:
        cACTION_BML_GET_CLIENT_ROAMING_PREFER_SIGNAL_STRENGTH_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_BML_GET_CLIENT_ROAMING_PREFER_SIGNAL_STRENGTH_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_BML_GET_CLIENT_ROAMING_PREFER_SIGNAL_STRENGTH_RESPONSE();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_GET_CLIENT_ROAMING_PREFER_SIGNAL_STRENGTH_RESPONSE);
        }
        uint8_t& isEnable();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
        uint8_t* m_isEnable = nullptr;
};

class cACTION_BML_SET_CLIENT_BAND_STEERING_REQUEST : public BaseClass
{
    public:
        cACTION_BML_SET_CLIENT_BAND_STEERING_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_BML_SET_CLIENT_BAND_STEERING_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_BML_SET_CLIENT_BAND_STEERING_REQUEST();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_SET_CLIENT_BAND_STEERING_REQUEST);
        }
        uint8_t& isEnable();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
        uint8_t* m_isEnable = nullptr;
};

class cACTION_BML_SET_CLIENT_BAND_STEERING_RESPONSE : public BaseClass
{
    public:
        cACTION_BML_SET_CLIENT_BAND_STEERING_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_BML_SET_CLIENT_BAND_STEERING_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_BML_SET_CLIENT_BAND_STEERING_RESPONSE();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_SET_CLIENT_BAND_STEERING_RESPONSE);
        }
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
};

class cACTION_BML_GET_CLIENT_BAND_STEERING_REQUEST : public BaseClass
{
    public:
        cACTION_BML_GET_CLIENT_BAND_STEERING_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_BML_GET_CLIENT_BAND_STEERING_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_BML_GET_CLIENT_BAND_STEERING_REQUEST();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_GET_CLIENT_BAND_STEERING_REQUEST);
        }
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
};

class cACTION_BML_GET_CLIENT_BAND_STEERING_RESPONSE : public BaseClass
{
    public:
        cACTION_BML_GET_CLIENT_BAND_STEERING_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_BML_GET_CLIENT_BAND_STEERING_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_BML_GET_CLIENT_BAND_STEERING_RESPONSE();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_GET_CLIENT_BAND_STEERING_RESPONSE);
        }
        uint8_t& isEnable();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
        uint8_t* m_isEnable = nullptr;
};

class cACTION_BML_SET_IRE_ROAMING_REQUEST : public BaseClass
{
    public:
        cACTION_BML_SET_IRE_ROAMING_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_BML_SET_IRE_ROAMING_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_BML_SET_IRE_ROAMING_REQUEST();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_SET_IRE_ROAMING_REQUEST);
        }
        uint8_t& isEnable();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
        uint8_t* m_isEnable = nullptr;
};

class cACTION_BML_SET_IRE_ROAMING_RESPONSE : public BaseClass
{
    public:
        cACTION_BML_SET_IRE_ROAMING_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_BML_SET_IRE_ROAMING_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_BML_SET_IRE_ROAMING_RESPONSE();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_SET_IRE_ROAMING_RESPONSE);
        }
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
};

class cACTION_BML_GET_IRE_ROAMING_REQUEST : public BaseClass
{
    public:
        cACTION_BML_GET_IRE_ROAMING_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_BML_GET_IRE_ROAMING_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_BML_GET_IRE_ROAMING_REQUEST();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_GET_IRE_ROAMING_REQUEST);
        }
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
};

class cACTION_BML_GET_IRE_ROAMING_RESPONSE : public BaseClass
{
    public:
        cACTION_BML_GET_IRE_ROAMING_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_BML_GET_IRE_ROAMING_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_BML_GET_IRE_ROAMING_RESPONSE();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_GET_IRE_ROAMING_RESPONSE);
        }
        uint8_t& isEnable();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
        uint8_t* m_isEnable = nullptr;
};

class cACTION_BML_SET_LOAD_BALANCER_REQUEST : public BaseClass
{
    public:
        cACTION_BML_SET_LOAD_BALANCER_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_BML_SET_LOAD_BALANCER_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_BML_SET_LOAD_BALANCER_REQUEST();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_SET_LOAD_BALANCER_REQUEST);
        }
        uint8_t& isEnable();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
        uint8_t* m_isEnable = nullptr;
};

class cACTION_BML_SET_LOAD_BALANCER_RESPONSE : public BaseClass
{
    public:
        cACTION_BML_SET_LOAD_BALANCER_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_BML_SET_LOAD_BALANCER_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_BML_SET_LOAD_BALANCER_RESPONSE();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_SET_LOAD_BALANCER_RESPONSE);
        }
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
};

class cACTION_BML_GET_LOAD_BALANCER_REQUEST : public BaseClass
{
    public:
        cACTION_BML_GET_LOAD_BALANCER_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_BML_GET_LOAD_BALANCER_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_BML_GET_LOAD_BALANCER_REQUEST();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_GET_LOAD_BALANCER_REQUEST);
        }
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
};

class cACTION_BML_GET_LOAD_BALANCER_RESPONSE : public BaseClass
{
    public:
        cACTION_BML_GET_LOAD_BALANCER_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_BML_GET_LOAD_BALANCER_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_BML_GET_LOAD_BALANCER_RESPONSE();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_GET_LOAD_BALANCER_RESPONSE);
        }
        uint8_t& isEnable();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
        uint8_t* m_isEnable = nullptr;
};

class cACTION_BML_SET_SERVICE_FAIRNESS_REQUEST : public BaseClass
{
    public:
        cACTION_BML_SET_SERVICE_FAIRNESS_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_BML_SET_SERVICE_FAIRNESS_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_BML_SET_SERVICE_FAIRNESS_REQUEST();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_SET_SERVICE_FAIRNESS_REQUEST);
        }
        uint8_t& isEnable();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
        uint8_t* m_isEnable = nullptr;
};

class cACTION_BML_SET_SERVICE_FAIRNESS_RESPONSE : public BaseClass
{
    public:
        cACTION_BML_SET_SERVICE_FAIRNESS_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_BML_SET_SERVICE_FAIRNESS_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_BML_SET_SERVICE_FAIRNESS_RESPONSE();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_SET_SERVICE_FAIRNESS_RESPONSE);
        }
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
};

class cACTION_BML_GET_SERVICE_FAIRNESS_REQUEST : public BaseClass
{
    public:
        cACTION_BML_GET_SERVICE_FAIRNESS_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_BML_GET_SERVICE_FAIRNESS_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_BML_GET_SERVICE_FAIRNESS_REQUEST();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_GET_SERVICE_FAIRNESS_REQUEST);
        }
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
};

class cACTION_BML_GET_SERVICE_FAIRNESS_RESPONSE : public BaseClass
{
    public:
        cACTION_BML_GET_SERVICE_FAIRNESS_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_BML_GET_SERVICE_FAIRNESS_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_BML_GET_SERVICE_FAIRNESS_RESPONSE();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_GET_SERVICE_FAIRNESS_RESPONSE);
        }
        uint8_t& isEnable();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
        uint8_t* m_isEnable = nullptr;
};

class cACTION_BML_CHANGE_MODULE_LOGGING_LEVEL_REQUEST : public BaseClass
{
    public:
        cACTION_BML_CHANGE_MODULE_LOGGING_LEVEL_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_BML_CHANGE_MODULE_LOGGING_LEVEL_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_BML_CHANGE_MODULE_LOGGING_LEVEL_REQUEST();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_CHANGE_MODULE_LOGGING_LEVEL_REQUEST);
        }
        sLoggingLevelChange& params();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
        sLoggingLevelChange* m_params = nullptr;
};

class cACTION_BML_CHANGE_MODULE_LOGGING_LEVEL_RESPONSE : public BaseClass
{
    public:
        cACTION_BML_CHANGE_MODULE_LOGGING_LEVEL_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_BML_CHANGE_MODULE_LOGGING_LEVEL_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_BML_CHANGE_MODULE_LOGGING_LEVEL_RESPONSE();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_CHANGE_MODULE_LOGGING_LEVEL_RESPONSE);
        }
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
};

class cACTION_BML_WIFI_CREDENTIALS_UPDATE_REQUEST : public BaseClass
{
    public:
        cACTION_BML_WIFI_CREDENTIALS_UPDATE_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_BML_WIFI_CREDENTIALS_UPDATE_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_BML_WIFI_CREDENTIALS_UPDATE_REQUEST();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_WIFI_CREDENTIALS_UPDATE_REQUEST);
        }
        sWifiCredentials& params();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
        sWifiCredentials* m_params = nullptr;
};

class cACTION_BML_WIFI_CREDENTIALS_UPDATE_RESPONSE : public BaseClass
{
    public:
        cACTION_BML_WIFI_CREDENTIALS_UPDATE_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_BML_WIFI_CREDENTIALS_UPDATE_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_BML_WIFI_CREDENTIALS_UPDATE_RESPONSE();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_WIFI_CREDENTIALS_UPDATE_RESPONSE);
        }
        uint32_t& error_code();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
        uint32_t* m_error_code = nullptr;
};

class cACTION_BML_SET_RESTRICTED_CHANNELS_REQUEST : public BaseClass
{
    public:
        cACTION_BML_SET_RESTRICTED_CHANNELS_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_BML_SET_RESTRICTED_CHANNELS_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_BML_SET_RESTRICTED_CHANNELS_REQUEST();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_SET_RESTRICTED_CHANNELS_REQUEST);
        }
        sRestrictedChannels& params();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
        sRestrictedChannels* m_params = nullptr;
};

class cACTION_BML_SET_RESTRICTED_CHANNELS_RESPONSE : public BaseClass
{
    public:
        cACTION_BML_SET_RESTRICTED_CHANNELS_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_BML_SET_RESTRICTED_CHANNELS_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_BML_SET_RESTRICTED_CHANNELS_RESPONSE();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_SET_RESTRICTED_CHANNELS_RESPONSE);
        }
        uint32_t& error_code();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
        uint32_t* m_error_code = nullptr;
};

class cACTION_BML_GET_RESTRICTED_CHANNELS_REQUEST : public BaseClass
{
    public:
        cACTION_BML_GET_RESTRICTED_CHANNELS_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_BML_GET_RESTRICTED_CHANNELS_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_BML_GET_RESTRICTED_CHANNELS_REQUEST();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_GET_RESTRICTED_CHANNELS_REQUEST);
        }
        sRestrictedChannels& params();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
        sRestrictedChannels* m_params = nullptr;
};

class cACTION_BML_GET_RESTRICTED_CHANNELS_RESPONSE : public BaseClass
{
    public:
        cACTION_BML_GET_RESTRICTED_CHANNELS_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_BML_GET_RESTRICTED_CHANNELS_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_BML_GET_RESTRICTED_CHANNELS_RESPONSE();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_GET_RESTRICTED_CHANNELS_RESPONSE);
        }
        sRestrictedChannels& params();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
        sRestrictedChannels* m_params = nullptr;
};

class cACTION_BML_SET_VAP_LIST_CREDENTIALS_REQUEST : public BaseClass
{
    public:
        cACTION_BML_SET_VAP_LIST_CREDENTIALS_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_BML_SET_VAP_LIST_CREDENTIALS_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_BML_SET_VAP_LIST_CREDENTIALS_REQUEST();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_SET_VAP_LIST_CREDENTIALS_REQUEST);
        }
        uint32_t& result();
        uint8_t& vap_list_size();
        std::tuple<bool, sConfigVapInfo&> vap_list(size_t idx);
        bool alloc_vap_list(size_t count = 1);
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
        uint32_t* m_result = nullptr;
        uint8_t* m_vap_list_size = nullptr;
        sConfigVapInfo* m_vap_list = nullptr;
        size_t m_vap_list_idx__ = 0;
        int m_lock_order_counter__ = 0;
};

class cACTION_BML_SET_VAP_LIST_CREDENTIALS_RESPONSE : public BaseClass
{
    public:
        cACTION_BML_SET_VAP_LIST_CREDENTIALS_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_BML_SET_VAP_LIST_CREDENTIALS_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_BML_SET_VAP_LIST_CREDENTIALS_RESPONSE();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_SET_VAP_LIST_CREDENTIALS_RESPONSE);
        }
        uint32_t& result();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
        uint32_t* m_result = nullptr;
};

class cACTION_BML_GET_VAP_LIST_CREDENTIALS_RESPONSE : public BaseClass
{
    public:
        cACTION_BML_GET_VAP_LIST_CREDENTIALS_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_BML_GET_VAP_LIST_CREDENTIALS_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_BML_GET_VAP_LIST_CREDENTIALS_RESPONSE();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_GET_VAP_LIST_CREDENTIALS_RESPONSE);
        }
        uint32_t& result();
        uint8_t& vap_list_size();
        std::tuple<bool, sConfigVapInfo&> vap_list(size_t idx);
        bool alloc_vap_list(size_t count = 1);
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
        uint32_t* m_result = nullptr;
        uint8_t* m_vap_list_size = nullptr;
        sConfigVapInfo* m_vap_list = nullptr;
        size_t m_vap_list_idx__ = 0;
        int m_lock_order_counter__ = 0;
};

class cACTION_BML_GET_VAP_LIST_CREDENTIALS_REQUEST : public BaseClass
{
    public:
        cACTION_BML_GET_VAP_LIST_CREDENTIALS_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_BML_GET_VAP_LIST_CREDENTIALS_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_BML_GET_VAP_LIST_CREDENTIALS_REQUEST();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_GET_VAP_LIST_CREDENTIALS_REQUEST);
        }
        uint32_t& result();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
        uint32_t* m_result = nullptr;
};

class cACTION_BML_STEERING_SET_GROUP_REQUEST : public BaseClass
{
    public:
        cACTION_BML_STEERING_SET_GROUP_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_BML_STEERING_SET_GROUP_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_BML_STEERING_SET_GROUP_REQUEST();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_STEERING_SET_GROUP_REQUEST);
        }
        uint32_t& steeringGroupIndex();
        sSteeringApConfig& cfg_2();
        sSteeringApConfig& cfg_5();
        uint8_t& remove();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
        uint32_t* m_steeringGroupIndex = nullptr;
        sSteeringApConfig* m_cfg_2 = nullptr;
        sSteeringApConfig* m_cfg_5 = nullptr;
        uint8_t* m_remove = nullptr;
};

class cACTION_BML_STEERING_SET_GROUP_RESPONSE : public BaseClass
{
    public:
        cACTION_BML_STEERING_SET_GROUP_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_BML_STEERING_SET_GROUP_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_BML_STEERING_SET_GROUP_RESPONSE();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_STEERING_SET_GROUP_RESPONSE);
        }
        int32_t& error_code();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
        int32_t* m_error_code = nullptr;
};

class cACTION_BML_STEERING_CLIENT_SET_REQUEST : public BaseClass
{
    public:
        cACTION_BML_STEERING_CLIENT_SET_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_BML_STEERING_CLIENT_SET_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_BML_STEERING_CLIENT_SET_REQUEST();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_STEERING_CLIENT_SET_REQUEST);
        }
        uint32_t& steeringGroupIndex();
        sMacAddr& bssid();
        sMacAddr& client_mac();
        sSteeringClientConfig& config();
        uint8_t& remove();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
        uint32_t* m_steeringGroupIndex = nullptr;
        sMacAddr* m_bssid = nullptr;
        sMacAddr* m_client_mac = nullptr;
        sSteeringClientConfig* m_config = nullptr;
        uint8_t* m_remove = nullptr;
};

class cACTION_BML_STEERING_CLIENT_SET_RESPONSE : public BaseClass
{
    public:
        cACTION_BML_STEERING_CLIENT_SET_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_BML_STEERING_CLIENT_SET_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_BML_STEERING_CLIENT_SET_RESPONSE();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_STEERING_CLIENT_SET_RESPONSE);
        }
        int32_t& error_code();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
        int32_t* m_error_code = nullptr;
};

class cACTION_BML_STEERING_EVENT_REGISTER_UNREGISTER_REQUEST : public BaseClass
{
    public:
        cACTION_BML_STEERING_EVENT_REGISTER_UNREGISTER_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_BML_STEERING_EVENT_REGISTER_UNREGISTER_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_BML_STEERING_EVENT_REGISTER_UNREGISTER_REQUEST();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_STEERING_EVENT_REGISTER_UNREGISTER_REQUEST);
        }
        uint8_t& unregister();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
        uint8_t* m_unregister = nullptr;
};

class cACTION_BML_STEERING_EVENT_REGISTER_UNREGISTER_RESPONSE : public BaseClass
{
    public:
        cACTION_BML_STEERING_EVENT_REGISTER_UNREGISTER_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_BML_STEERING_EVENT_REGISTER_UNREGISTER_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_BML_STEERING_EVENT_REGISTER_UNREGISTER_RESPONSE();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_STEERING_EVENT_REGISTER_UNREGISTER_RESPONSE);
        }
        int32_t& error_code();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
        int32_t* m_error_code = nullptr;
};

class cACTION_BML_STEERING_CLIENT_DISCONNECT_REQUEST : public BaseClass
{
    public:
        cACTION_BML_STEERING_CLIENT_DISCONNECT_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_BML_STEERING_CLIENT_DISCONNECT_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_BML_STEERING_CLIENT_DISCONNECT_REQUEST();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_STEERING_CLIENT_DISCONNECT_REQUEST);
        }
        uint32_t& steeringGroupIndex();
        sMacAddr& bssid();
        sMacAddr& client_mac();
        eDisconnectType& type();
        uint32_t& reason();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
        uint32_t* m_steeringGroupIndex = nullptr;
        sMacAddr* m_bssid = nullptr;
        sMacAddr* m_client_mac = nullptr;
        eDisconnectType* m_type = nullptr;
        uint32_t* m_reason = nullptr;
};

class cACTION_BML_STEERING_CLIENT_DISCONNECT_RESPONSE : public BaseClass
{
    public:
        cACTION_BML_STEERING_CLIENT_DISCONNECT_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_BML_STEERING_CLIENT_DISCONNECT_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_BML_STEERING_CLIENT_DISCONNECT_RESPONSE();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_STEERING_CLIENT_DISCONNECT_RESPONSE);
        }
        int32_t& error_code();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
        int32_t* m_error_code = nullptr;
};

class cACTION_BML_STEERING_CLIENT_MEASURE_REQUEST : public BaseClass
{
    public:
        cACTION_BML_STEERING_CLIENT_MEASURE_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_BML_STEERING_CLIENT_MEASURE_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_BML_STEERING_CLIENT_MEASURE_REQUEST();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_STEERING_CLIENT_MEASURE_REQUEST);
        }
        uint32_t& steeringGroupIndex();
        sMacAddr& bssid();
        sMacAddr& client_mac();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
        uint32_t* m_steeringGroupIndex = nullptr;
        sMacAddr* m_bssid = nullptr;
        sMacAddr* m_client_mac = nullptr;
};

class cACTION_BML_STEERING_CLIENT_MEASURE_RESPONSE : public BaseClass
{
    public:
        cACTION_BML_STEERING_CLIENT_MEASURE_RESPONSE(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_BML_STEERING_CLIENT_MEASURE_RESPONSE(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_BML_STEERING_CLIENT_MEASURE_RESPONSE();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_STEERING_CLIENT_MEASURE_RESPONSE);
        }
        int32_t& error_code();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
        int32_t* m_error_code = nullptr;
};

class cACTION_BML_STEERING_EVENTS_UPDATE : public BaseClass
{
    public:
        cACTION_BML_STEERING_EVENTS_UPDATE(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_BML_STEERING_EVENTS_UPDATE(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_BML_STEERING_EVENTS_UPDATE();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_STEERING_EVENTS_UPDATE);
        }
        uint32_t& buffer_size();
        char* buffer(size_t length = 0);
        bool set_buffer(std::string& str);
        bool set_buffer(const std::string& str);
        bool set_buffer(const char buffer[], size_t size);
        bool alloc_buffer(size_t count = 1);
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
        uint32_t* m_buffer_size = nullptr;
        char* m_buffer = nullptr;
        size_t m_buffer_idx__ = 0;
        int m_lock_order_counter__ = 0;
};

class cACTION_BML_TRIGGER_CHANNEL_SELECTION_REQUEST : public BaseClass
{
    public:
        cACTION_BML_TRIGGER_CHANNEL_SELECTION_REQUEST(uint8_t* buff, size_t buff_len, bool parse = false, bool swap_needed = false);
        cACTION_BML_TRIGGER_CHANNEL_SELECTION_REQUEST(std::shared_ptr<BaseClass> base, bool parse = false, bool swap_needed = false);
        ~cACTION_BML_TRIGGER_CHANNEL_SELECTION_REQUEST();

        static eActionOp_BML get_action_op(){
            return (eActionOp_BML)(ACTION_BML_TRIGGER_CHANNEL_SELECTION_REQUEST);
        }
        sMacAddr& al_mac();
        sMacAddr& ruid();
        void class_swap();
        static size_t get_initial_size();

    private:
        bool init();
        eActionOp_BML* m_action_op = nullptr;
        sMacAddr* m_al_mac = nullptr;
        sMacAddr* m_ruid = nullptr;
};

}; // close namespace: beerocks_message

#endif //_BEEROCKS/TLVF_BEEROCKS_MESSAGE_BML_H_
