#include "appmgr.h"

#include <iostream>
#include <console.h>
#include <vector>
#include <string>
#include <json/json.h>
#include <memory>
#include <pybind11/embed.h>
#include <pybind11/pytypes.h>
#include <pybind11/stl.h>

#include "lua/luaapi.h"

namespace py = pybind11;
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

    PYBIND11_EMBEDDED_MODULE(shfl, m) {
        m.def("currentDirectory", [](const string&name) {
            if (wsMap.find(name) != wsMap.end()) return py::str(wsMap[name]->currentDirectory().string());

            error("Error: Cannot find workspace '" + name + "'!");
            return py::str("");
        }, "Move workspace directory to another directory.");
        m.def("moveDirectory", [](const string&name, const string&newDir) {
            if (wsMap.find(name) != wsMap.end()) { wsMap[name]->moveDirectory(newDir); }
            else error("Error: Cannot find workspace '" + name + "'!");
        }, "Move workspace directory to another directory.");
    }

    Command loadCommandVersion2(Json::Value appInfo, const string&libPath) {
        // NOLINT(*-no-recursion)
        string name = appInfo["name"].asString();
        string usage = appInfo["usage"].asString();
        string description = appInfo["description"].asString();

        const string commandPath = libPath + name + "/";

        vector<shared_ptr<Command>> subcommands;
        for (const auto&subcommandInfo: appInfo["subcommands"]) {
            subcommands.push_back(make_shared<Command>(loadCommandVersion2(subcommandInfo, commandPath + "/")));
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
            py::scoped_interpreter guard{};

            py::dict pOptionValues;
            for (const auto&[key, value]: optionValues)
                pOptionValues[py::str(key)] = py::str(value);

            try {
                const py::module sys = py::module::import("sys");
                sys.attr("path").attr("append")(py::str(PY_PKGS.string()));
                sys.attr("path").attr("append")(py::str(commandPath));

                const py::module module = py::module::import("command");

                const py::object result = module.attr("entrypoint")(py::str(ws->getName()), pOptionValues,
                                                                    py::bool_(backgroundMode));

                return py::str(result);
            }
            catch (const std::exception&e) {
                std::cerr << "Error executing Python function: " << e.what() << std::endl;
                return "Error";
            }
        };
        command.setCmd(cmd);

        return command;
    }

    void App::loadVersion2(const string&appPath, Json::Value appRoot) {
        this->name = appRoot["name"].asString();
        description = appRoot["description"].asString();
        author = appRoot["author"].asString();
        version = appRoot["version"].asString();

        Json::Value commandsJson = appRoot["commands"];
        for (const auto&commandInfo: commandsJson) {
            commands.push_back(
                make_shared<Command>(loadCommandVersion2(commandInfo, appPath + "/lib/")));
        }
    }

    App::App(const string&name) {
        path appPath = DOT_SHUFFLE / "apps" / (name + ".shflapp");
        path appShfl = appPath / "app.shfl";

        Json::Value appRoot;
        Json::Reader appReader;
        appReader.parse(readFile(appShfl), appRoot, false);

        apiVersion = appRoot["api-version"].asInt();

        if (apiVersion == 1)
            error("App '" + name + "' is not supported anymore! (Because of api-version)");
        else if (apiVersion == 2)
            loadVersion2(absolute(appPath).string(), appRoot);
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

        Json::Value value(Json::objectValue);
        value["name"] = name;
        for (const auto&item: commandList) {
            if (item["name"] == name) return false;
        }

        commandList.append(value);

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
            res.push_back(command["name"].asString());
        }

        return res;
    }
}
