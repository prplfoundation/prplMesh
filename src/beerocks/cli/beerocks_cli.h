/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _BEEROCKS_CLI_H
#define _BEEROCKS_CLI_H

#include <fstream>
#include <map>
#include <stdio.h>

#include <beerocks/tlvf/beerocks_message.h>

#define ARGSNUM 10

namespace beerocks {
class cli {
public:
    cli();
    virtual ~cli();

    bool parsedCommandArgumentsLinux(std::string line);

    virtual bool connect()      = 0;
    virtual void disconnect()   = 0;
    virtual bool is_connected() = 0;
    virtual bool start()        = 0;
    virtual void stop()         = 0;

    std::string &get_command() { return command; }
    int callCommand();
    std::vector<std::string> get_available_commands();

    //protected:
    //private:

    typedef int (cli::*pFunction)(int);

    enum eArgumentsTypes {
        INT_ARG    = 0,
        STRING_ARG = 1,
        FLOAT_ARG  = 2,
    };

    struct arguments {
        int intArgs[ARGSNUM];
        std::string stringArgs[ARGSNUM];
        float floatArgs[ARGSNUM];
        std::string multipleCommandsArg;
    };

    struct commandInfo {
        std::string help_args;
        std::string help;
        pFunction funcPtr;
        uint8_t minNumOfArgs = 0;
        uint8_t maxNumOfArgs = 0;
        std::vector<eArgumentsTypes> argsTypes;
    };

    // Help functions

    void resetArguments();

    virtual void setFunctionsMapAndArray() = 0;

    bool insertCommandToMap(std::string command, std::string help_args, std::string help,
                            pFunction funcPtr, uint8_t minNumOfArgs, uint8_t maxNumOfArgs,
                            eArgumentsTypes type1 = INT_ARG, eArgumentsTypes type2 = INT_ARG,
                            eArgumentsTypes type3 = INT_ARG, eArgumentsTypes type4 = INT_ARG,
                            eArgumentsTypes type5 = INT_ARG, eArgumentsTypes type6 = INT_ARG,
                            eArgumentsTypes type7 = INT_ARG, eArgumentsTypes type8 = INT_ARG,
                            eArgumentsTypes type9 = INT_ARG, eArgumentsTypes type10 = INT_ARG);

    // Caller functions

    int help_caller(int numOfArgs);

    int multiple_commands_caller(int numOfArgs);

    int exit_caller(int numOfArgs);

    // Functions

    int help(bool print_header = true);

    int multiple_commands(std::string line);

    int exit();

    // Variables
    std::map<std::string, commandInfo> functionsMap;
    std::map<std::string, commandInfo>::const_iterator commandIt;
    arguments args;
    int commandNumOfArgs = 0;
    std::string command;
    std::vector<std::string> commandTokens;
    std::vector<std::string> multipleCommandTokens;

    size_t helpLineSize = 0;
    std::string err;
};
} // namespace beerocks

#endif // _BEEROCKS_CLI_H
