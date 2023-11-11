#include "appmgr.h"

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

#include <Windows.h>

#undef near
#undef far
#elif __linux__

#include <dlfcn.h>

#endif

vector<shared_ptr<App>> loadedApps;

// vector<string> App::makeDynamicSuggestion(Workspace&ws, const string&suggestId) const {
//     // Create workspace table
//     lua_newtable(L); {
//         lua_pushstring(L, ws.currentDirectory().string().c_str());
//         lua_setfield(L, -2, "dir");
//     } // ws.dir
//
//     lua_setglobal(L, "workspace");
//
//     // Create args list
//     lua_pushstring(L, suggestId.c_str());
//     lua_setglobal(L, "suggestId");
//
//     lua_getglobal(L, "suggest");
//     if (lua_type(L, -1) != LUA_TFUNCTION) {
//         error("Error: 'suggest' is not a function!");
//         return {};
//     }
//     if (lua_pcall(L, 0, 0, 0)) {
//         error("\nAn error occurred while generating suggestion.\n\n" + string(lua_tostring(L, -1)));
//     }
//
//     const lua_Unsigned tableSize = lua_rawlen(L, -1);
//
//     if (lua_istable(L, -1)) {
//         vector<string> resultArray;
//         for (int i = 1; i <= tableSize; ++i) {
//             lua_rawgeti(L, -1, i);
//
//             if (lua_isstring(L, -1)) {
//                 resultArray.emplace_back(lua_tostring(L, -1));
//             }
//
//             lua_pop(L, 1);
//         }
//         return resultArray;
//     }
//     return {};
// }

void App::loadVersion1(const string&name) {
    string appPath = DOT_SHUFFLE + "/apps/" + name + ".app";
    string appShfl = appPath + "/app.shfl";

    Json::Value appRoot;
    Json::Reader appReader;
    appReader.parse(readFile(appShfl), appRoot, false);

    this->name = appRoot["name"].asString();
    description = appRoot["description"].asString();
    author = appRoot["author"].asString();
    version = appRoot["version"].asString();
    apiVersion = appRoot["api-version"].asInt();

    Json::Value commandsJson = appRoot["commands"];
    for (const auto&commandInfo: commandsJson) {
        commands.push_back(
            make_shared<Command>(Command(commandInfo, appPath + "/lib/" + commandInfo["name"].asString() + ".lua")));
    }
}

App::App(const string&name) {
    loadVersion1(name);
}

void loadApp(const string&name) {
    for (const auto&app: loadedApps) {
        if (app->getName() == name) return;
    }
    loadedApps.push_back(make_shared<App>(App(name)));
}

bool addApp(const string&name) {
    vector<string> res;

    Json::Value commandList = getShflJson("apps");

    Json::Value value(Json::objectValue);
    value["name"] = name;
    for (const auto&item: commands) {
        if (item->getName() == name) return false;
    }

    commandList.append(value);

    setShflJson("apps", commandList);
    return true;
}

vector<string> getApps() {
    vector<string> res;

    Json::Value commandList = getShflJson("apps");
    for (auto command: commandList) {
        res.push_back(command["name"].asString());
    }

    return res;
}
