#include "commandmgr.h"

#include <json/json.h>
#include <memory>
#include <utility>

#include "utils.h"

using std::make_shared;

vector<shared_ptr<Command>> commands;

void do_nothing(Workspace &ws, map<string, string> &optionValues) {}

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

void Command::run(Workspace &ws, map<string, string> &optionValues) const {
    cmd(ws, optionValues);
}

const vector<shared_ptr<Command>> &Command::getSubcommands() const {
    return subcommands;
}

const vector<CommandOption> &Command::getOptions() const {
    return options;
}

Command::Command(string name, string description, const vector<shared_ptr<Command>> &subcommands,
                 const vector<CommandOption> &options, cmd_t cmd) : name(std::move(name)),
                                                                    description(std::move(
                                                                            description)),
                                                                    subcommands(subcommands),
                                                                    options(options), cmd(std::move(cmd)) {}

Command::Command(string name, string description,
                 const vector<shared_ptr<Command>> &subcommands, cmd_t cmd) : name(std::move(name)),
                                                                    description(std::move(description)),
                                                                    subcommands(subcommands), cmd(std::move(cmd)) {}

Command::Command(string name, string description,
                 const vector<CommandOption> &options, cmd_t cmd) : name(std::move(name)),
                                                                    description(std::move(description)),
                                                                    options(options), cmd(std::move(cmd)) {}

Command::Command(string name, string description, cmd_t cmd) : name(std::move(name)),
                                                               description(std::move(description)),
                                                               cmd(std::move(cmd)) {}

Command::Command(string name) : name(std::move(name)), cmd(do_nothing) {}

CommandOption::CommandOption(string name, OptionType type, const vector<string> &aliases) : name(std::move(name)),
                                                                                            type(type),
                                                                                            aliases(aliases) {}
