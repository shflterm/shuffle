//
// Created by winch on 5/6/2023.
//

#ifndef SHUFFLE_INCLUDE_LOADER_H_
#define SHUFFLE_INCLUDE_LOADER_H_

#include "commandmgr.h"
#include "lua.hpp"
#include "utils.h"
#include "console.h"

#include <vector>
#include <string>
#include <json/json.h>

class SAPPCommand : public Command {
protected:
    lua_State *L{};

public:
    explicit SAPPCommand(const string &name);

    void loadVersion2(const string &name);

    void run(Workspace &ws, map<string, string> &optionValues) const override;

    vector<string> makeDynamicSuggestion(Workspace &ws, const string &suggestId);
};

void loadApp(const CommandData &data);

#endif //SHUFFLE_INCLUDE_LOADER_H_
