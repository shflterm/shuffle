#include "cmd/commandmgr.h"

#include <json/json.h>
#include <memory>
#include <utility>

#include "console.h"
#include "utils/utils.h"
#include "sapp/sapp.h"
#include "cmd/builtincmd.h"

using std::make_shared;

vector<shared_ptr<Command>> commands;

void loadDefaultCommands() {
    commands.push_back(make_shared<BuiltinCommand>(BuiltinCommand(
            "shfl", "Shuffle Command", {
                    CommandOption("reload", BOOL_T, {"rl"}),
                    CommandOption("update", BOOL_T, {"upgrade"}),
                    CommandOption("credits", BOOL_T, {"credit"}),
            }, shflCmd
    )));
    commands.push_back(make_shared<BuiltinCommand>(BuiltinCommand(
            "appmgr", "App Manager", {
                    CommandOption("add", TEXT_T, {"ad", "a"}),
                    CommandOption("remove", TEXT_T, {"rm", "r"}),
            }, appMgrCmd
    )));
    commands.push_back(make_shared<BuiltinCommand>(BuiltinCommand(
            "help", "Show help", {
                    CommandOption("command", TEXT_T, {"cmd", "help"})
            }, helpCmd
    )));
    commands.push_back(make_shared<BuiltinCommand>(BuiltinCommand(
            "snf", "Manage Snippets", {
                    CommandOption("create", TEXT_T, {"mk", "c", "new"}),
                    CommandOption("value", TEXT_T, {"v"}),
            }, snippetCmd
    )));
}

void loadCommand(const CommandData &data) {
    for (const auto &item: commands) {
        if (item->getName() == data.name) return;
    }
    commands.push_back(make_shared<SAPPCommand>(SAPPCommand(data.name)));
}

vector<CommandData> getRegisteredCommands() {
    vector<CommandData> res;

    Json::Value commandList = getShflJson("apps");
    for (auto command: commandList) {
        CommandData data;
        data.name = command["name"].asString();
        res.push_back(data);
    }

    return res;
}

bool addRegisteredCommand(const CommandData &data) {
    vector<CommandData> res;

    Json::Value commandList = getShflJson("apps");

    Json::Value value(Json::objectValue);
    value["name"] = data.name;
    for (const auto &item: commands) {
        if (item->getName() == data.name) return false;
    }

    commandList.append(value);

    setShflJson("apps", commandList);
    return true;
}

void loadCommands() {
    commands.clear();

    loadDefaultCommands();

    for (const CommandData &command: getRegisteredCommands()) {
        loadCommand(command);
    }
}

shared_ptr<Command> findCommand(const string &name, const vector<shared_ptr<Command>> &DICTIONARY) {
    for (auto &item: DICTIONARY) {
        if (item->getName() == name) {
            return item;
        }
    }
    return nullptr;
}

shared_ptr<Command> findCommand(const string &name) {
    return findCommand(name, commands);
}

const string &Command::getName() const {
    return name;
}

const string &Command::getDescription() const {
    return description;
}

void Command::run(Workspace &ws, map<string, string> &optionValues) const {}

Command::Command(string name, string description, vector<CommandOption> options,
                 vector<pair<string, string>> usage)
        : name(std::move(name)),
          description(std::move(description)),
          options(std::move(options)),
          usage(std::move(usage)) {}

Command::Command(string name, string description, vector<CommandOption> options)
        : name(std::move(name)),
          description(std::move(description)),
          options(std::move(options)) {}

Command::Command(string name, string description)
        : name(std::move(name)),
          description(std::move(description)) {}

Command::Command(string name, vector<CommandOption> options)
        : name(std::move(name)),
          description("-"),
          options(std::move(options)) {}

Command::Command(string name)
        : name(std::move(name)),
          description("-") {}

const vector<pair<string, string>> &Command::getUsage() const {
    return usage;
}

const vector<CommandOption> &Command::getOptions() const {
    return options;
}

CommandOption::CommandOption(string name, OptionType type, const vector<string> &aliases) : name(std::move(name)),
                                                                                            type(type),
                                                                                            aliases(aliases) {}
