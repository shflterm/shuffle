#include "appmgr/appmgr.h"

#include <iostream>
#include <vector>
#include <string>
#include <json/json.h>
#include <memory>
#ifdef _WIN32
#include <windows.h>
#elif defined(__linux__) || defined(__APPLE__)
#include <dlfcn.h>
#endif

#include "utils/console.h"
#include "utils/shfljson.h"

using std::make_shared, std::cout, std::to_string, std::ofstream, cmd::Command, cmd::CommandOption;

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

    vector<shared_ptr<Command>> getCommands() {
        vector<shared_ptr<Command>> res;
        for (const auto&app: loadedApps) {
            for (const auto&command: app->commands) {
                res.push_back(command);
            }
        }
        return res;
    }

    extern "C" {
    typedef string (*entrypoint_t)(Workspace*, const map<string, string>&, bool);

    typedef string (*startup_t)();
    }

#ifdef _WIN32
    void closeLibrary(const HMODULE libraryHandle) {
        FreeLibrary(libraryHandle);
    }
#else
    void closeLibrary(void* libraryHandle) {
        dlclose(libraryHandle);
    }
#endif

    Command loadCommandVersion3(Json::Value commandInfo, const string&libPath, App* app) {
        // NOLINT(*-no-recursion)
        string name = commandInfo["name"].asString();
        string usage = commandInfo["usage"].asString();
        string description = commandInfo["description"].asString();

        const string commandPath = libPath + name + "/";

        vector<shared_ptr<Command>> subcommands;
        for (const auto&subcommandInfo: commandInfo["subcommands"]) {
            subcommands.push_back(make_shared<Command>(loadCommandVersion3(subcommandInfo, commandPath + "/", app)));
        }

        vector<CommandOption> options;
        for (const auto&option: commandInfo["options"]) {
            const string optionName = option["name"].asString();
            const string optionDescription = option["description"].asString();
            const string optionType = option["type"].asString();
            vector<string> aliases;
            for (const auto&alias: option["aliases"]) aliases.push_back(alias.asString());
            const bool isRequired = option["required"].asBool();
            options.emplace_back(optionName, optionDescription, optionType, aliases, isRequired);
        }

        vector<string> aliases;
        for (const auto&alias: commandInfo["aliases"]) aliases.push_back(alias.asString());

        vector<cmd::CommandExample> examples;
        for (const auto&example: commandInfo["examples"]) examples.emplace_back(example.asString(), "");
        auto command = Command(name, description, usage, subcommands, options, aliases, examples);

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
            error("Failed to load the library. (" + libraryPath + ")");
            return command;
        }

        const auto startup =
#ifdef _WIN32
                reinterpret_cast<startup_t>(GetProcAddress(libraryHandle, "startup"));
#elif defined(__APPLE__) || defined(__linux__)
                    reinterpret_cast<startup_t>(dlsym(libraryHandle, "startup"));
#endif
        if (startup) startup();

        cmd_t cmd = [=](Workspace* ws, const map<string, string>&optionValues, const bool backgroundMode,
                        const string&id) -> string {
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

            try {
                string res = entrypoint(ws, optionValues, backgroundMode);
                return res;
            }
            catch (const std::exception&ex) {
                error("Sorry. An error occurred while executing the command. ($0)", {string(ex.what())});
                error("");
                warning("This is not Shuffle's error. Please do not report this issue to the Shuffle team.");
                return "error";
            }
        };
        command.setCmd(cmd);

        if (app) {
            app->onUnload.emplace_back([=] {
                closeLibrary(libraryHandle);
            });
        }

        return command;
    }

    void loadVersion3(App* app, const string&appPath, Json::Value appRoot) {
        app->name = appRoot["name"].asString();
        app->description = appRoot["description"].asString();
        app->author = appRoot["author"].asString();
        app->version = appRoot["version"].asString();

        Json::Value commandsJson = appRoot["commands"];
        for (const auto&commandInfo: commandsJson) {
            app->commands.push_back(
                make_shared<Command>(loadCommandVersion3(commandInfo, appPath + "/lib/", app)));
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
            loadVersion3(this, absolute(appPath).string(), appRoot);
        else
            error("App '" + name + "' has an invalid API version! (" + std::to_string(apiVersion) + ")");
    }

    App::App(string name, string description, string author, string version,
             vector<shared_ptr<Command>> commands): name(std::move(name)), description(std::move(description)),
                                                    author(std::move(author)),
                                                    version(std::move(version)), commands(std::move(commands)) {
    }

    void App::unload() const {
        for (const auto&function: onUnload) function();
    }


    void loadApp(const shared_ptr<App>&app) {
        for (const auto&loadedApp: loadedApps) {
            if (loadedApp->getName() == app->name) return;
        }
        loadedApps.push_back(app);
    }

    bool addApp(const string&name) {
        vector<string> res;

        Json::Value apps = getShflJson("apps");
        bool alreadyExists = false;
        for (const auto&app: apps) {
            if (app.asString() == name) {
                alreadyExists = true;
                break;
            }
        }
        if (!alreadyExists) apps.append(name);
        setShflJson("apps", apps);
        return true;
    }

    void unloadAllApps() {
        for (const auto&app: loadedApps) {
            app->unload();
        }
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
