//
// Created by 08sih on 7/16/2023.
//

#ifndef SHFL_CMDPARSER_H
#define SHFL_CMDPARSER_H

#include <vector>
#include <map>

#include "commandmgr.h"
#include "parsedcmd.h"

using namespace std;

ParsedCommand parseCommand(Command *app, const vector<string>& args);

map<string, string> *parseOptions(Command *app, const vector<string> &args);

#endif //SHFL_CMDPARSER_H
