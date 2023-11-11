//
// Created by winch on 5/5/2023.
//

#ifndef SHUFFLE_INCLUDE_BUILTINCMD_H_
#define SHUFFLE_INCLUDE_BUILTINCMD_H_

#include <string>
#include <map>

#include "workspace.h"

void loadCommands();

void shflReloadCmd([[maybe_unused]] Workspace *ws, map<string, string> &optionValues);

void shflUpgradeCmd([[maybe_unused]] Workspace *ws, map<string, string> &optionValues);

void shflCreditsCmd([[maybe_unused]] Workspace *ws, map<string, string> &optionValues);

void shflCmd([[maybe_unused]] Workspace *ws, map<string, string> &optionValues);

void appMgrAddCmd(Workspace *ws, map<string, string> &optionValues);

void appMgrRemoveCmd(Workspace *ws, map<string, string> &optionValues);

void appMgrCmd(Workspace *ws, map<string, string> &optionValues);

void helpCmd(Workspace *ws, map<string, string> &optionValues);

void snippetCmd(Workspace *ws, map<string, string> &optionValues);

void clearCmd([[maybe_unused]] Workspace *ws, map<string, string> &optionValues);

#endif //SHUFFLE_INCLUDE_BUILTINCMD_H_
