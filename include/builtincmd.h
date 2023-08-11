//
// Created by winch on 5/5/2023.
//

#ifndef SHUFFLE_INCLUDE_BUILTINCMD_H_
#define SHUFFLE_INCLUDE_BUILTINCMD_H_

#include <string>
#include <utility>
#include <vector>
#include <map>

#include "workspace.h"
#include "commandmgr.h"

typedef void(*cmd_t)(Workspace &, map<string, string> &);


class BuiltinCommand : public Command {
    cmd_t cmd;

public:
    BuiltinCommand(string name, string description, vector<CommandOption> options, cmd_t cmd)
            : Command(std::move(name), std::move(description), std::move(options)), cmd(cmd) {
    };

    void run(Workspace &ws, map<string, string> &optionValues) const override;
};

void loadCommands();

void shflCmd(Workspace &ws, map<string, string> &optionValues);

void appMgrCmd(Workspace &ws, map<string, string> &optionValues);

void helpCmd(Workspace &ws, map<string, string> &optionValues);

void snippetCmd(Workspace &ws, map<string, string> &optionValues);

void automationCmd(Workspace &ws, map<string, string> &optionValues);

#endif //SHUFFLE_INCLUDE_BUILTINCMD_H_
