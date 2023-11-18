//
// Created by 08sih on 7/6/2023.
//

#ifndef SHFL_PARSEDCMD_H
#define SHFL_PARSEDCMD_H

#include <map>
#include <string>
#include <memory>

class ParsedCommand;

#include "commandmgr.h"

using std::shared_ptr, std::map, std::string;

enum CommandType {
    COMMAND,
    SNIPPET,
    VARIABLE,
    EMPTY
};

class ParsedCommand {
    CommandType commandType;

public:
    Command* app;
    map<string, string> options;

    string executeApp(Workspace* ws);

    bool isCommand();

    explicit ParsedCommand(Command* app);

    explicit ParsedCommand(CommandType commandType);

    ParsedCommand();
};

#endif //SHFL_PARSEDCMD_H
