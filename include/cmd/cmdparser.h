//
// Created by 08sih on 7/16/2023.
//

#ifndef SHFL_CMDPARSER_H
#define SHFL_CMDPARSER_H

#include <vector>
#include <map>
#include <memory>

#include "commandmgr.h"
#include "parsedcmd.h"

using std::shared_ptr;

ParsedCommand parseCommand(shared_ptr<Command> app, const vector<string> &args);

map<string, string> *parseOptions(const shared_ptr<Command>& app, const vector<string> &args);

#endif //SHFL_CMDPARSER_H
