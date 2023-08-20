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

void loadCommands();

void shflCmd(Workspace &ws, map<string, string> &optionValues);

void appMgrCmd(Workspace &ws, map<string, string> &optionValues);

void helpCmd(Workspace &ws, map<string, string> &optionValues);

void snippetCmd(Workspace &ws, map<string, string> &optionValues);

#endif //SHUFFLE_INCLUDE_BUILTINCMD_H_
