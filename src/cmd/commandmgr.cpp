#include "cmd/commandmgr.h"

#include <iostream>
#include <json/json.h>
#include <memory>
#include <utility>

#include "console.h"
#include "utils/utils.h"
#include "sapp/sapp.h"
#include "cmd/builtincmd.h"

using namespace std;

vector<shared_ptr<Command>> commands;

void loadDefaultCommands() {
    commands.push_back(make_shared<BuiltinCommand>(BuiltinCommand(
            "shfl", "Shuffle Command", {}, shflCmd
    )));
    commands.push_back(make_shared<BuiltinCommand>(BuiltinCommand(
            "help", "Show help", {{"command", {"cmd", "c"}}}, helpCmd
    )));
    commands.push_back(make_shared<BuiltinCommand>(BuiltinCommand(
            "snf", "Manage Snippets", {{"create", {"mk", "c", "new"}}}, snippetCmd
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

Command::Command(string name, string description, map<string, vector<string>> options, vector<pair<string, string>> usage)
        : name(std::move(name)),
          description(std::move(description)),
          options(std::move(options)),
          usage(std::move(usage)) {}

Command::Command(string name, string description, map<string, vector<string>> options)
        : name(std::move(name)),
          description(std::move(description)),
          options(std::move(options)) {}

Command::Command(string name, string description)
        : name(std::move(name)),
          description(std::move(description)) {}

Command::Command(string name, map<string, vector<string>> options)
        : name(std::move(name)),
          description("-"),
          options(std::move(options)) {}

Command::Command(string name)
        : name(std::move(name)),
          description("-") {}

const vector<pair<string, string>> &Command::getUsage() const {
    return usage;
}

const map<string, vector<string>> &Command::getOptions() const {
    return options;
}
