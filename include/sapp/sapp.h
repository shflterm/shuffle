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
    SAPPCommand(const SAPPCommand& parent, const string &name);

    lua_State *L{};

public:
    explicit SAPPCommand(const string &name);

    void loadVersion2(const Json::Value& root, const string &name);

    void loadVersion3(const string &name, const string &appPath, const string &value);

    vector<string> makeDynamicSuggestion(Workspace &ws, const string &suggestId);
};

void loadApp(const CommandData &data);

#endif //SHUFFLE_INCLUDE_LOADER_H_
