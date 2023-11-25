#include "appmgr.h"

#include <iostream>
#include <console.h>
#include <vector>
#include <string>
#include <json/json.h>
#include <memory>
#ifdef _WIN32
#include <windows.h>
#elif defined(__linux__) || defined(__APPLE__)
#include <dlfcn.h>
#endif

using std::make_shared, std::cout, std::to_string, std::ofstream, cmd::Command, cmd::CommandOption, cmd::OptionType,
        cmd::commands;

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

namespace appmgr {
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

    extern "C" {
    typedef string (*entrypoint_t)(string, const map<string, string>&, bool);
    }

#ifdef _WIN32
    void closeLibrary(HMODULE libraryHandle) {
        FreeLibrary(libraryHandle);
    }
#else
    void closeLibrary(void* libraryHandle) {
        dlclose(libraryHandle);
    }
#endif

    Command loadCommandVersion3(Json::Value appInfo, const string&libPath) {
        // NOLINT(*-no-recursion)
        string name = appInfo["name"].asString();
        string usage = appInfo["usage"].asString();
        string description = appInfo["description"].asString();

        const string commandPath = libPath + name + "/";

        vector<shared_ptr<Command>> subcommands;
        for (const auto&subcommandInfo: appInfo["subcommands"]) {
            subcommands.push_back(make_shared<Command>(loadCommandVersion3(subcommandInfo, commandPath + "/")));
        }

        vector<CommandOption> options;
        for (const auto&option: appInfo["options"]) {
            const string optionName = option["name"].asString();
            OptionType type;
            if (option["type"].asString() == "string" || option["type"].asString() == "text") type = cmd::TEXT;
            else if (option["type"].asString() == "boolean") type = cmd::BOOLEAN;
            else if (option["type"].asString() == "number") type = cmd::NUMBER;
            else if (option["type"].asString() == "file") type = cmd::FILE;
            else if (option["type"].asString() == "directory") type = cmd::DIRECTORY;
            else if (option["type"].asString() == "fileordir") type = cmd::FILE_OR_DIRECTORY;
            else if (option["type"].asString() == "command") type = cmd::COMMAND;
            else {
                error("Error: Invalid option type in " + optionName + ".");
                type = cmd::TEXT;
            }
            const string optionDescription = option["description"].asString();
            vector<string> aliases;
            for (const auto&alias: option["aliases"]) aliases.push_back(alias.asString());
            options.emplace_back(optionName, optionDescription, type, aliases);
        }

        vector<string> aliases;
        for (const auto&alias: appInfo["aliases"]) aliases.push_back(alias.asString());

        vector<string> examples;
        for (const auto&example: appInfo["examples"]) examples.push_back(example.asString());
        Command command = Command(name, description, usage, subcommands, options, aliases, examples);

        cmd_t cmd = [=](Workspace* ws, map<string, string>&optionValues, const bool backgroundMode,
                        const string&id) -> string {
            // 라이브러리 경로
            string libraryPath =
#ifdef _WIN32
                    commandPath + "command.dll";
#elif __APPLE__
                    commandPath + "command.dylib";
#else
                    commandPath + "command.so";
#endif

            // 라이브러리 열기
#ifdef _WIN32
            HMODULE libraryHandle = LoadLibraryA(libraryPath.c_str());
#elif defined(__linux__) || defined(__APPLE__)
            void* libraryHandle = dlopen(libraryPath.c_str(), RTLD_LAZY);
#endif

            if (!libraryHandle) {
                error("Failed to open the library. Please check if the library exists.");
                return "ERROR_OPENING_LIBRARY";
            }

            // 라이브러리에서 함수 로드
            const auto entrypoint =
#ifdef _WIN32
                    reinterpret_cast<entrypoint_t>(GetProcAddress(libraryHandle, "entrypoint"));
#elif defined(__APPLE__) || defined(__linux__)
                    reinterpret_cast<entrypoint_t>(dlsym(libraryHandle, "entrypoint"));
#endif

            if (!entrypoint) {
                error("Failed to load the entrypoint function.");
                closeLibrary(libraryHandle);
                return "ERROR_LOADING_FUNCTION";
            }

            string res = entrypoint(ws->getName(), optionValues, backgroundMode);
            closeLibrary(libraryHandle);

            return res;
        };
        command.setCmd(cmd);

        return command;
    }

    void App::loadVersion3(const string&appPath, Json::Value appRoot) {
        this->name = appRoot["name"].asString();
        description = appRoot["description"].asString();
        author = appRoot["author"].asString();
        version = appRoot["version"].asString();

        Json::Value commandsJson = appRoot["commands"];
        for (const auto&commandInfo: commandsJson) {
            commands.push_back(
                make_shared<Command>(loadCommandVersion3(commandInfo, appPath + "/lib/")));
        }
    }

    App::App(const string&name) {
        path appPath = DOT_SHUFFLE / "apps" / (name + ".shflapp");
        path appShfl = appPath / "app.shfl";

        Json::Value appRoot;
        Json::Reader appReader;
        appReader.parse(readFile(appShfl), appRoot, false);

        apiVersion = appRoot["api-version"].asInt();

        if (apiVersion == 1 || apiVersion == 2)
            error("App '" + name + "' is not supported anymore! (Because of api-version)");
        else if (apiVersion == 3)
            loadVersion3(absolute(appPath).string(), appRoot);
        else
            error("App '" + name + "' has an invalid API version! (" + std::to_string(apiVersion) + ")");
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
        commandList.append(name);
        setShflJson("apps", commandList);
        return true;
    }

    void unloadAllApps() {
        loadedApps.clear();
    }

    vector<string> getApps() {
        vector<string> res;

        Json::Value commandList = getShflJson("apps");
        for (auto command: commandList) {
            res.push_back(command.asString());
        }

        return res;
    }
}
