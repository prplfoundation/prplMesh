/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include "beerocks_cli.h"

#include <bcl/beerocks_string_utils.h>

#include <iostream>
#include <iterator>
#include <map>
#include <sstream>

using namespace beerocks;

#define MAXPARSEDLINE 25
#define HELP_STRING_LINE_SIZE 80

cli::cli() {}

cli::~cli() {}

//
// Help functions
//

bool cli::insertCommandToMap(std::string command, std::string help_args, std::string help,
                             pFunction funcPtr, uint8_t minNumOfArgs, uint8_t maxNumOfArgs,
                             eArgumentsTypes type1, eArgumentsTypes type2, eArgumentsTypes type3,
                             eArgumentsTypes type4, eArgumentsTypes type5, eArgumentsTypes type6,
                             eArgumentsTypes type7, eArgumentsTypes type8, eArgumentsTypes type9,
                             eArgumentsTypes type10)
{

    commandInfo info;
    eArgumentsTypes types[ARGSNUM] = {type1, type2, type3, type4, type5,
                                      type6, type7, type8, type9, type10};

    info.help_args        = help_args;
    info.help             = help;
    size_t help_line_size = command.size() + help_args.size();
    if (helpLineSize < help_line_size)
        helpLineSize = help_line_size;

    info.funcPtr      = funcPtr;
    info.minNumOfArgs = minNumOfArgs;
    info.maxNumOfArgs = maxNumOfArgs;
    if (maxNumOfArgs <= 10) {
        for (int i = 0; i < maxNumOfArgs; i++) {
            info.argsTypes.push_back(types[i]);
        }
    }

    functionsMap[command] = info;

    return true;
}

void cli::resetArguments()
{
    command.clear();
    commandIt        = functionsMap.end();
    commandNumOfArgs = 0;
    memset(args.intArgs, 0, sizeof(args.intArgs));
    memset(args.floatArgs, 0, sizeof(args.floatArgs));
    for (int i = 0; i < ARGSNUM; i++) {
        args.stringArgs[i] = "";
    }
    args.multipleCommandsArg = "";
}

bool cli::parsedCommandArgumentsLinux(std::string line)
{
    //split line into tokens
    std::istringstream iss(line);
    commandTokens.clear();
    std::copy(std::istream_iterator<std::string>(iss), std::istream_iterator<std::string>(),
              back_inserter(commandTokens));
    resetArguments();
    command = *commandTokens.begin();

    commandIt = functionsMap.find(command);
    if (commandIt == functionsMap.end()) {
        if (!command.empty()) {
            std::cout << "Error, command not found: " << command << std::endl;
        }
        resetArguments();
        return false;
    }

    const commandInfo &info = commandIt->second;
    commandNumOfArgs        = commandTokens.size() - 1;

    bool late_processing = false;
    auto first_arg       = std::next(commandTokens.begin());
    if (first_arg != commandTokens.end() && (*first_arg)[0] == '"') {
        late_processing  = true;
        commandNumOfArgs = 1;
    }

    if (commandNumOfArgs < info.minNumOfArgs) {
        std::cout << "Error, Entered less than minimum number of arguments required." << std::endl;
        resetArguments();
        return false;
    }
    if ((commandNumOfArgs > info.maxNumOfArgs) && (info.maxNumOfArgs <= 10)) {
        std::cout << "Error, Entered more than maximum number of arguments allowed." << std::endl;
        resetArguments();
        return false;
    }

    int i = 0;
    for (std::vector<std::string>::iterator it = commandTokens.begin(); it != commandTokens.end();
         it++) {
        if (it == commandTokens.begin())
            continue;
        if (late_processing) {
            // put all tokens into args.stringArgs[0]
            args.stringArgs[0] += " " + *it;
        } else if (info.maxNumOfArgs > 10) { //multi commands
            args.multipleCommandsArg +=
                " " +
                *it; //for multi commands, put all the parameters as one line in args.stringArgs[0]
        } else {     //regular command
            switch (info.argsTypes.at(i)) {
            case INT_ARG:
                args.intArgs[i] = std::atoi((*it).c_str());
                break;
            case STRING_ARG:
                args.stringArgs[i] = *it;
                break;
            case FLOAT_ARG:
                args.floatArgs[i] = std::atof((*it).c_str());
                break;
            }
        }
        i++;
    }

    if (late_processing) {
        string_utils::trim(args.stringArgs[0], "\"");
    }

    return true;
}

int cli::callCommand()
{
    if (commandIt == functionsMap.end())
        return -1;
    return ((this->*commandIt->second.funcPtr)(commandNumOfArgs));
}

//
// Caller functions
//

int cli::help_caller(int numOfArgs)
{
    if (numOfArgs < 0)
        return -1;
    return help();
}

int cli::multiple_commands_caller(int numOfArgs)
{
    if (numOfArgs < 1)
        return -1;
    return multiple_commands(args.multipleCommandsArg);
}

int cli::exit_caller(int numOfArgs)
{
    if (numOfArgs < 0)
        return -1;
    return exit();
}

//
// Functions
//
std::vector<std::string> cli::get_available_commands()
{
    std::vector<std::string> ret_vec;

    if (is_connected()) {
        for (auto it = functionsMap.begin(); it != functionsMap.end(); ++it) {
            if (it->first == "-c")
                continue;
            ret_vec.push_back(it->first);
        }
    }
    return ret_vec;
}

int cli::help(bool print_header)
{

    std::string l, pad, help_copy, aligned_help;
    unsigned int extra_chars;
    unsigned int help_str_line_size;

    //get console window size
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

    //printing help
    if (print_header) {
        std::cout << "To enter the same command several time, use the '*' operator with  the "
                     "number of times you want the commamd will be executed."
                  << std::endl
                  << "For example, this will print connectivity map 5 times: dump_conn_map *5"
                  << std::endl
                  << "To enter the same"
                  << " command in infinite loop use the '!' operator. For example: dump_conn_map !"
                  << std::endl
                  << "These operators"
                  << " (*,!) can be place anywhere in the command." << std::endl;
        std::cout << "Usage: command <arguments>" << std::endl;
    }
    for (auto it = functionsMap.begin(); it != functionsMap.end(); ++it) {
        l = it->first + " " + it->second.help_args;
        pad.clear();
        int pad_size = int(helpLineSize + 4) - int(l.size());
        if (pad_size > 0)
            pad.insert(0, pad_size, ' ');

        //aligning help text
        int spaces_to_colons = l.size() + pad.size() + 10;
        if (w.ws_col > spaces_to_colons + 50) {
            help_str_line_size = w.ws_col - spaces_to_colons - 10;
        } else {
            help_str_line_size = 50;
        }
        help_copy = it->second.help;
        aligned_help.clear();

        while (help_copy.size() > help_str_line_size) {
            extra_chars = 0;
            while (help_copy[help_str_line_size + extra_chars] != 0) {
                if (help_copy[help_str_line_size + extra_chars] == ' ') {
                    break;
                }
                extra_chars++;
            }
            //extra_chars++;
            aligned_help.append(help_copy, 0, help_str_line_size + extra_chars + 1);
            aligned_help.pop_back();
            aligned_help.append("\n");
            for (int space = 0; space <= spaces_to_colons; space++) {
                aligned_help.append(" "); //space
            }
            if (help_copy.size() > (help_str_line_size + extra_chars)) {
                help_copy = help_copy.substr(help_str_line_size + extra_chars + 1);
            } else {
                help_copy = help_copy.substr(help_str_line_size + extra_chars);
            }
        }
        aligned_help.append(help_copy);
        std::cout << "\t" << l << pad << " : " << aligned_help << std::endl;
    }
    std::cout << std::endl;
    return 0;
}

int cli::multiple_commands(std::string line)
{
    //split line into tokens
    std::istringstream iss(line);
    multipleCommandTokens.clear();
    std::copy(std::istream_iterator<std::string>(iss), std::istream_iterator<std::string>(),
              back_inserter(multipleCommandTokens));

    //initiate a line for current command
    std::string currCommand     = *multipleCommandTokens.begin();
    std::string currCommandLine = currCommand;

    //call parsedCommandArguments + callCommand() with it's arguments for each command
    for (std::vector<std::string>::iterator it = multipleCommandTokens.begin();
         it != multipleCommandTokens.end(); ++it) {
        if (it == multipleCommandTokens.begin())
            continue;
        currCommand = *it;
        commandIt   = functionsMap.find(currCommand);
        if (commandIt ==
            functionsMap
                .end()) { //current value is a command parameter and not the command itself, add it to currCommandLine
            currCommandLine += " " + currCommand;
        } else { //finished reading current command parameters, execute the command and continue to the next one
            if (parsedCommandArgumentsLinux(currCommandLine)) {
                if (callCommand() < 0) {
                    break;
                }
            }
            currCommandLine = currCommand;
        }
    }
    //we didn't execute last command in the for loop - execute it now
    if (parsedCommandArgumentsLinux(currCommandLine)) {
        if (callCommand() < 0) {
            return -1;
        }
    }
    return 1;
}

int cli::exit() { return -1; }
