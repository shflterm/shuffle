#include "commandmgr.h"

#include <json/json.h>
#include <memory>
#include <utility>

#include "utils.h"

using std::make_shared;

vector<shared_ptr<Command>> commands;

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

void clearCommands() {
    commands.clear();
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
