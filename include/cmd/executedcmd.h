//
// Created by 08sih on 7/6/2023.
//

#ifndef SHFL_EXECUTEDCMD_H
#define SHFL_EXECUTEDCMD_H

#include <map>
#include <string>

#include "commandmgr.h"

using namespace std;

class ExecutedCommand {
public:
    Command *app;
    map<string, string> options;

    void executeApp(Workspace ws);

    explicit ExecutedCommand(Command *app);
};

#endif //SHFL_EXECUTEDCMD_H
