#include "commandmgr.h"

#include <appmgr.h>
#include <console.h>
#include <lua/luaapi.h>
#include <json/json.h>
#include <memory>
#include <utility>

#include "utils.h"

using std::make_shared;

vector<shared_ptr<Command>> commands;

void do_nothing([[maybe_unused]] Workspace* ws, [[maybe_unused]] map<string, string>&optionValues) {
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

void Command::run(Workspace* ws, map<string, string>&optionValues) const {
    cmd(ws, optionValues);
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

Command::Command(Json::Value info, const string&libPath) {
    // NOLINT(*-no-recursion)
    name = info["name"].asString();
    usage = info["usage"].asString();
    description = info["description"].asString();

    const string commandPath = libPath + name + "/";

    for (const auto&subcommandInfo: info["subcommands"]) {
        subcommands.push_back(make_shared<Command>(
            Command(subcommandInfo, commandPath + "/")
        ));
    }

    for (const auto&option: info["options"]) {
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

    for (const auto&alias: info["aliases"]) aliases.push_back(alias.asString());

    for (const auto&example: info["examples"]) examples.push_back(example.asString());

    lua_State* L = luaL_newstate();
    luaL_openlibs(L);
    initLua(L);
    int err = luaL_loadfile(L, (commandPath + "command.lua").c_str());
    if (err) error("Error: " + string(lua_tostring(L, -1)));

    err = lua_pcall(L, 0, 0, 0);
    if (err) error("Error: " + string(lua_tostring(L, -1)));

    cmd = [=](Workspace* ws, map<string, string>&optionValues) {
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

        lua_getglobal(L, "entrypoint");
        if (lua_type(L, -1) != LUA_TFUNCTION) {
            error("Error: 'entrypoint' is not a function!");
            return;
        }
        if (lua_pcall(L, 0, 0, 0)) error("Error: " + string(lua_tostring(L, -1)));

        // Update workspace
        lua_getglobal(L, "workspace");
        lua_getfield(L, -1, "dir");

        path newDir = lua_tostring(L, -1);
        if (newDir.is_relative()) newDir = ws->currentDirectory() / newDir;
        ws->moveDirectory(newDir);
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
