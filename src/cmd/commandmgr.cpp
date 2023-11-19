#include "commandmgr.h"

#include <iostream>
#include <appmgr.h>
#include <console.h>
#include <lua/luaapi.h>
#include <json/json.h>
#include <memory>
#include <utility>

#include "utils.h"

using std::make_shared, std::cout;

vector<shared_ptr<Command>> commands;

string do_nothing(Workspace* ws, map<string, string>&optionValues, const bool backgroundMode) {
    return "do_nothing";
}

shared_ptr<Command> findCommand(const string&name, const vector<shared_ptr<Command>>&DICTIONARY) {
    for (auto&item: DICTIONARY) {
        if (item->getName() == name) {
            return item;
        }
    }
    return nullptr;
}

shared_ptr<Command> findCommand(const string&name) {
    return findCommand(name, commands);
}

const string& Command::getName() const {
    return name;
}

const string& Command::getDescription() const {
    return description;
}

const string& Command::getUsage() const {
    return usage;
}

string Command::run(Workspace* ws, map<string, string>&optionValues, const bool backgroundMode) const {
    return cmd(ws, optionValues, backgroundMode);
}

string Command::createHint() const {
    string hint;
    hint += description;
    if (!usage.empty())
        hint += " / " + usage;
    return hint;
}

const vector<shared_ptr<Command>>& Command::getSubcommands() const {
    return subcommands;
}

const vector<CommandOption>& Command::getOptions() const {
    return options;
}

const vector<string>& Command::getAliases() const {
    return aliases;
}

const vector<string>& Command::getExamples() const {
    return examples;
}

Command::Command(string name, string description, const vector<Command>&subcommands,
                 const vector<CommandOption>&options, cmd_t cmd)
    : name(std::move(name)),
      description(std::move(description)),
      options(options), cmd(std::move(cmd)) {
    for (const auto&subcommand: subcommands) {
        this->subcommands.push_back(make_shared<Command>(subcommand));
    }
}

Command::Command(string name, string description, const vector<Command>&subcommands, cmd_t cmd)
    : name(std::move(name)),
      description(std::move(description)),
      cmd(std::move(cmd)) {
    for (const auto&subcommand: subcommands) {
        this->subcommands.push_back(make_shared<Command>(subcommand));
    }
}

Command::Command(string name, string description, const vector<CommandOption>&options, cmd_t cmd)
    : name(std::move(name)),
      description(std::move(description)),
      options(options), cmd(std::move(cmd)) {
}

Command::Command(string name, string description, cmd_t cmd)
    : name(std::move(name)),
      description(std::move(description)),
      cmd(std::move(cmd)) {
}

Command::Command(string name)
    : name(std::move(name)), cmd(do_nothing) {
}

Command::Command(Json::Value appInfo, const string&libPath) {
    // NOLINT(*-no-recursion)
    name = appInfo["name"].asString();
    usage = appInfo["usage"].asString();
    description = appInfo["description"].asString();

    const string commandPath = libPath + name + "/";

    for (const auto&subcommandInfo: appInfo["subcommands"]) {
        subcommands.push_back(make_shared<Command>(
            Command(subcommandInfo, commandPath + "/")
        ));
    }

    for (const auto&option: appInfo["options"]) {
        const string name = option["name"].asString();
        OptionType type;
        if (option["type"].asString() == "string") type = TEXT_T;
        else if (option["type"].asString() == "boolean") type = BOOL_T;
        else if (option["type"].asString() == "integer") type = INT_T;
        else {
            error("Error: Invalid option type in " + name + ".");
            type = TEXT_T;
        }
        const string description = option["description"].asString();
        vector<string> aliases;
        for (const auto&alias: option["aliases"]) aliases.push_back(alias.asString());
        options.emplace_back(name, description, type, aliases);
    }

    for (const auto&alias: appInfo["aliases"]) aliases.push_back(alias.asString());

    for (const auto&example: appInfo["examples"]) examples.push_back(example.asString());

    lua_State* L = luaL_newstate();
    luaL_openlibs(L);
    initLua(L);
    int err = luaL_loadfile(L, (commandPath + "command.lua").c_str());
    if (err) error("Error: " + string(lua_tostring(L, -1)));

    err = lua_pcall(L, 0, 0, 0);
    if (err) error("Error: " + string(lua_tostring(L, -1)));

    cmd = [=](Workspace* ws, map<string, string>&optionValues, const bool backgroundMode) -> string {
        // Create workspace table
        lua_newtable(L); {
            lua_pushstring(L, ws->currentDirectory().string().c_str());
            lua_setfield(L, -2, "dir");
        } // ws.dir

        lua_setglobal(L, "workspace");

        // Create args list
        for (const auto&[key, value]: optionValues) {
            lua_pushstring(L, value.c_str());
            lua_setglobal(L, key.c_str());
        }

        lua_pushboolean(L, backgroundMode);
        lua_setglobal(L, "backgroundMode");

        if (!backgroundMode) {
            lua_pushcfunction(L, [](lua_State* LL) {
                              debug(lua_tostring(LL, -1));
                              return 0;
                              });
            lua_setglobal(L, "debug");

            lua_pushcfunction(L, [](lua_State* LL) {
                              cout << lua_tostring(LL, -1);
                              return 0;
                              });
            lua_setglobal(L, "print");

            lua_pushcfunction(L, [](lua_State* LL) {
                              info(lua_tostring(LL, -1));
                              return 0;
                              });
            lua_setglobal(L, "info");

            lua_pushcfunction(L, [](lua_State* LL) {
                              warning(lua_tostring(LL, -1));
                              return 0;
                              });
            lua_setglobal(L, "warning");

            lua_pushcfunction(L, [](lua_State* LL) {
                              error(lua_tostring(LL, -1));
                              return 0;
                              });
            lua_setglobal(L, "error");
        }
        else {
            lua_pushcfunction(L, [](lua_State* LL) { return 0; });
            lua_setglobal(L, "debug");

            lua_pushcfunction(L, [](lua_State* LL) { return 0; });
            lua_setglobal(L, "print");

            lua_pushcfunction(L, [](lua_State* LL) { return 0; });
            lua_setglobal(L, "info");

            lua_pushcfunction(L, [](lua_State* LL) { return 0; });
            lua_setglobal(L, "warning");

            lua_pushcfunction(L, [](lua_State* LL) { return 0; });
            lua_setglobal(L, "error");
        }

        lua_getglobal(L, "entrypoint");
        if (lua_type(L, -1) != LUA_TFUNCTION) {
            error("Error: 'entrypoint' is not a function!");
            return "false";
        }
        if (lua_pcall(L, 0, 1, 0)) error("Error: " + string(lua_tostring(L, -1)));

        string res = "null";
        if (lua_isnumber(L, -1)) {
            res = std::to_string(lua_tonumber(L, -1));
        }
        else if (lua_isstring(L, -1)) {
            res = lua_tostring(L, -1);
        }

        // Update workspace
        lua_getglobal(L, "workspace");
        lua_getfield(L, -1, "dir");

        path newDir = lua_tostring(L, -1);
        if (newDir.is_relative()) newDir = ws->currentDirectory() / newDir;
        ws->moveDirectory(newDir);

        return res;
    };
}

CommandOption::CommandOption(string name, string description, const OptionType type)
    : name(std::move(name)),
      description(std::move(description)),
      type(type),
      aliases({}) {
}

CommandOption::CommandOption(string name, string description, const OptionType type, const vector<string>&aliases)
    : name(std::move(name)),
      description(std::move(description)),
      type(type),
      aliases(aliases) {
}
