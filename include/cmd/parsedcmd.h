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
    CommandType commandType = EMPTY;

public:
    shared_ptr<Command> app;
    map<string, string> options;

    string executeApp(Workspace* ws, bool backgroundMode = false);

    bool isCommand() const;

    bool isSuccessed() const;

    explicit ParsedCommand(const shared_ptr<Command>&app);

    explicit ParsedCommand(CommandType commandType);

    ParsedCommand();
};

#endif //SHFL_PARSEDCMD_H
