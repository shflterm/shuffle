#include "commandmgr.h"

#include <appmgr.h>
#include <console.h>
#include <lua/luaapi.h>
#include <memory>
#include <utility>

using std::make_shared;

vector<shared_ptr<Command>> commands;

string do_nothing(Workspace* ws, map<string, string>& options, const bool bgMode, const string& id) {
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

string Command::run(Workspace* ws, map<string, string>&optionValues, const bool backgroundMode, const string&taskId) const {
    if (!cmd) {
        return "Invalid command";
    }
    return cmd(ws, optionValues, backgroundMode, taskId);
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

void Command::setCmd(cmd_t cmd) {
    this->cmd = std::move(cmd);
}

Command::Command(string name, string description, string usage,
                 const vector<shared_ptr<Command>>&subcommands, const vector<CommandOption>&options,
                 const vector<string>&aliases,
                 const vector<string>&examples, cmd_t cmd): name(std::move(name)),
                                                            description(std::move(description)),
                                                            usage(std::move(usage)),
                                                            subcommands(subcommands),
                                                            options(options),
                                                            aliases(aliases),
                                                            examples(examples),
                                                            cmd(std::move(cmd)) {
}

Command::Command(string name, string description, string usage,
                 const vector<shared_ptr<Command>>&subcommands, const vector<CommandOption>&options,
                 const vector<string>&aliases,
                 const vector<string>&examples): name(std::move(name)),
                                                 description(std::move(description)),
                                                 usage(std::move(usage)),
                                                 subcommands(subcommands),
                                                 options(options),
                                                 aliases(aliases),
                                                 examples(examples),
                                                 cmd(do_nothing) {
}

Command::Command(string name, string description, const vector<Command>&subcommands,
                 const vector<CommandOption>&options, const vector<string>&examples, cmd_t cmd)
    : name(std::move(name)),
      description(std::move(description)),
      options(options),
      examples(examples), cmd(std::move(cmd)) {
    for (const auto&subcommand: subcommands) {
        this->subcommands.push_back(make_shared<Command>(subcommand));
    }
}

Command::Command(string name, string description, const vector<Command>&subcommands, const vector<string>&examples,
                 cmd_t cmd)
    : name(std::move(name)),
      description(std::move(description)),
      examples(examples),
      cmd(std::move(cmd)) {
    for (const auto&subcommand: subcommands) {
        this->subcommands.push_back(make_shared<Command>(subcommand));
    }
}

Command::Command(string name, string description, const vector<CommandOption>&options, const vector<string>&examples,
                 cmd_t cmd)
    : name(std::move(name)),
      description(std::move(description)),
      options(options),
      examples(examples), cmd(std::move(cmd)) {
}

Command::Command(string name, string description, const vector<string>&examples, cmd_t cmd)
    : name(std::move(name)),
      description(std::move(description)),
      examples(examples),
      cmd(std::move(cmd)) {
}

Command::Command(string name)
    : name(std::move(name)), cmd(do_nothing) {
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
