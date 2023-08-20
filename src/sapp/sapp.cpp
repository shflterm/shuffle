#include "sapp.h"

#include <vector>
#include <string>
#include <json/json.h>
#include <memory>

#include "lua/luaapi.h"

using std::make_shared;

#ifdef _WIN32
#define NOMINMAX 1
#define byte win_byte_override
#define WIN32_LEAN_AND_MEAN
#define _HAS_STD_BYTE 0

#include <Windows.h>

#undef near
#undef far
#elif __linux__

#include <dlfcn.h>

#endif

vector<string> SAPPCommand::makeDynamicSuggestion(Workspace &ws, const string &suggestId) {
    // Create workspace table
    lua_newtable(L);
    {
        lua_pushstring(L, ws.currentDirectory().string().c_str());
        lua_setfield(L, -2, "dir");
    } // ws.dir

    lua_setglobal(L, "workspace");

    // Create args list
    lua_pushstring(L, suggestId.c_str());
    lua_setglobal(L, "suggestId");

    lua_getglobal(L, "suggest");
    if (lua_type(L, -1) != LUA_TFUNCTION) {
        error("Error: 'suggest' is not a function!");
        return {};
    }
    int err = lua_pcall(L, 0, 0, 0);
    if (err) {
        error("\nAn error occurred while generating suggestion.\n\n" + string(lua_tostring(L, -1)));
    }

    lua_Unsigned tableSize = lua_rawlen(L, -1);

    if (lua_istable(L, -1)) {
        vector<string> resultArray;
        for (int i = 1; i <= tableSize; ++i) {
            lua_rawgeti(L, -1, i);

            if (lua_isstring(L, -1)) {
                resultArray.emplace_back(lua_tostring(L, -1));
            }

            lua_pop(L, 1);
        }
        return resultArray;
    }
    return {};
}

void SAPPCommand::loadVersion2(const Json::Value &root, const string &name) {
    string appPath = DOT_SHUFFLE + "/apps/" + name + ".app/";

    string value = appPath + "/lib/entrypoint.lua";
    L = luaL_newstate();

    luaL_openlibs(L);
    initLua(L);
    int err = luaL_loadfile(L, value.c_str());
    if (err) error("Error: " + string(lua_tostring(L, -1)));

    err = lua_pcall(L, 0, 0, 0);
    if (err) error("Error: " + string(lua_tostring(L, -1)));

    //Read help.shfl
    string helpDotShfl = appPath + "/help.shfl";
    Json::Value helpRoot;
    Json::Reader helpReader;
    helpReader.parse(readFile(helpDotShfl), helpRoot, false);

    description = helpRoot["description"].asString();

    Json::Value optionsJson = helpRoot["options"];
    for (const auto &item: optionsJson) {
        string optionName = item["name"].asString();
        vector<string> aliases;
        aliases.push_back(optionName);
        for (const auto &item2: item["aliases"]) aliases.push_back(item2.asString());
        OptionType type;
        if (item["type"].asString() == "TEXT_T") type = TEXT_T;
        else if (item["type"].asString() == "BOOL_T") type = BOOL_T;
        else if (item["type"].asString() == "INT_T") type = INT_T;
        else {
            error("Error: Invalid option type in " + name + ".");
            type = TEXT_T;
        }

        options.emplace_back(optionName, type, aliases);
    }

//    for (const auto &item: helpRoot["example"]) {
//        usage.emplace_back(item["usage"].asString(), item["description"].asString());
//    }
}

void run_sapp(lua_State *L, Workspace &ws, map<string, string> &optionValues) {
    // Create workspace table
    lua_newtable(L);
    {
        lua_pushstring(L, ws.currentDirectory().string().c_str());
        lua_setfield(L, -2, "dir");
    } // ws.dir

    lua_setglobal(L, "workspace");

    // Create args list
    for (const auto &item: optionValues) {
        lua_pushstring(L, item.second.c_str());
        lua_setglobal(L, item.first.c_str());
    }

    lua_getglobal(L, "entrypoint");
    if (lua_type(L, -1) != LUA_TFUNCTION) {
        error("Error: 'entrypoint' is not a function!");
        return;
    }
    int err = lua_pcall(L, 0, 0, 0);
    if (err) error("Error: " + string(lua_tostring(L, -1)));

    // Update workspace
    lua_getglobal(L, "workspace");
    lua_getfield(L, -1, "dir");

    path newDir = lua_tostring(L, -1);
    if (newDir.is_relative()) newDir = ws.currentDirectory() / newDir;
    ws.moveDirectory(newDir);
}

void SAPPCommand::loadVersion3(const string &name, const string &appPath, const string &value) {
    L = luaL_newstate();

    luaL_openlibs(L);
    initLua(L);
    int err = luaL_loadfile(L, value.c_str());
    if (err) error("Error: " + string(lua_tostring(L, -1)));

    err = lua_pcall(L, 0, 0, 0);
    if (err) error("Error: " + string(lua_tostring(L, -1)));

    //Read help.shfl
    string helpDotShfl = appPath + "/help.shfl";
    Json::Value helpRoot;
    Json::Reader helpReader;
    helpReader.parse(readFile(helpDotShfl), helpRoot, false);

    description = helpRoot["description"].asString();

    Json::Value optionsJson = helpRoot["options"];
    for (const auto &item: optionsJson) {
        string optionName = item["name"].asString();
        vector<string> aliases;
        aliases.push_back(optionName);
        for (const auto &item2: item["aliases"]) aliases.push_back(item2.asString());
        OptionType type;
        if (item["type"].asString() == "TEXT_T") type = TEXT_T;
        else if (item["type"].asString() == "BOOL_T") type = BOOL_T;
        else if (item["type"].asString() == "INT_T") type = INT_T;
        else {
            error("Error: Invalid option type in " + name + ".");
            type = TEXT_T;
        }

        options.emplace_back(optionName, type, aliases);
    }

    Json::Value subcommandsJson = helpRoot["subcommands"];
    for (const auto &item: optionsJson) {
        string commandName = item["name"].asString();
        string commandDescription = item["description"].asString();
        SAPPCommand subcommand = SAPPCommand(*this, name, description);
        subcommands.push_back(subcommand);
    }

    cmd = [this](Workspace &ws, map<string, string> &optionValues) {
        run_sapp(this->L, ws, optionValues);
    };
}

SAPPCommand::SAPPCommand(const string &name) : Command(name) {
    string runDotShfl = DOT_SHUFFLE + "/apps/" + name + ".app/run.shfl";

    Json::Value root;
    Json::Reader reader;
    reader.parse(readFile(runDotShfl), root, false);
    if (root["version"].asInt() == 1) {
        error("This app(" + name + ") is no longer supported. Please upgrade the app.");
    } else if (root["version"].asInt() == 2) {
        loadVersion2(root, name);
    } else if (root["version"].asInt() == 3) {
        string appPath = DOT_SHUFFLE + "/apps/" + name + ".app/";
        string value = appPath + "/lib/entrypoint.lua";
        loadVersion3(name, appPath, value);
    } else {
        error("Error: Invalid version number in " + name + ".");
    }
}

SAPPCommand::SAPPCommand(const SAPPCommand& parent, const string &name, const string &description) : Command(name) {
    string appPath = DOT_SHUFFLE + "/apps/" + parent.name + ".app/" + name;
    string value = appPath + "/lib/entrypoint.lua";
    loadVersion3(name, appPath, value);
}

void loadApp(const CommandData &data) {
    for (const auto &item: commands) {
        if (item->getName() == data.name) return;
    }
    commands.push_back(make_shared<SAPPCommand>(SAPPCommand(data.name)));
}