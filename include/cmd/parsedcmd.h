//
// Created by 08sih on 7/6/2023.
//

#ifndef SHFL_PARSEDCMD_H
#define SHFL_PARSEDCMD_H

#include <map>
#include <string>

#include "commandmgr.h"


class ParsedCommand {
public:
    Command *app;
    map<string, string> options;

    void executeApp(Workspace ws);

    explicit ParsedCommand(Command *app);
};

#endif //SHFL_PARSEDCMD_H
