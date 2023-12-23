#include "cmd/commandmgr.h"

#include <memory>
#include <utility>

#include "appmgr/appmgr.h"
#include "workspace/workspace.h"

using std::make_shared;

string do_nothing(Workspace* ws, map<string, string>&options, const bool bgMode, const string&id) {
    return "do_nothing";
}

namespace cmd {
    shared_ptr<Command> findCommand(const string&name, const vector<shared_ptr<Command>>&DICTIONARY) {
        for (auto&item: DICTIONARY) {
            if (item->getName() == name) {
                return item;
            }
        }
        return nullptr;
    }

    shared_ptr<Command> findCommand(const string&name) {
        return findCommand(name, appmgr::getCommands());
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

    string Command::run(Workspace* ws, map<string, string>&optionValues, const bool backgroundMode,
                        const string&taskId) const {
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

    const vector<CommandExample>& Command::getExamples() const {
        return examples;
    }

    void Command::setCmd(cmd_t cmd) {
        this->cmd = std::move(cmd);
    }

    Command::Command(string name, string description, string usage,
                     const vector<Command>&subcommands, const vector<CommandOption>&options,
                     const vector<string>&aliases,
                     const vector<CommandExample>&examples, cmd_t cmd): name(std::move(name)),
                                                                        description(std::move(description)),
                                                                        usage(std::move(usage)),
                                                                        options(options),
                                                                        aliases(aliases),
                                                                        examples(examples),
                                                                        cmd(std::move(cmd)) {
        for (auto subcommand: subcommands) {
            subcommand.parent = make_shared<Command>(*this);
            this->subcommands.push_back(make_shared<Command>(subcommand));
        }
    }

    Command::Command(string name, string description, string usage,
                     const vector<shared_ptr<Command>>&subcommands, const vector<CommandOption>&options,
                     const vector<string>&aliases,
                     const vector<CommandExample>&examples): name(std::move(name)),
                                                             description(std::move(description)),
                                                             usage(std::move(usage)),
                                                             subcommands(subcommands),
                                                             options(options),
                                                             aliases(aliases),
                                                             examples(examples),
                                                             cmd(do_nothing) {
    }

    Command::Command(string name, string description, const vector<Command>&subcommands,
                     const vector<CommandOption>&options, const vector<CommandExample>&examples, cmd_t cmd)
        : name(std::move(name)),
          description(std::move(description)),
          options(options),
          examples(examples), cmd(std::move(cmd)) {
        for (auto subcommand: subcommands) {
            subcommand.parent = make_shared<Command>(*this);
            this->subcommands.push_back(make_shared<Command>(subcommand));
        }
    }

    Command::Command(string name, string description, const vector<Command>&subcommands,
                     const vector<CommandExample>&examples,
                     cmd_t cmd)
        : name(std::move(name)),
          description(std::move(description)),
          examples(examples),
          cmd(std::move(cmd)) {
        for (auto subcommand: subcommands) {
            subcommand.parent = make_shared<Command>(*this);
            this->subcommands.push_back(make_shared<Command>(subcommand));
        }
    }

    Command::Command(string name, string description, const vector<CommandOption>&options,
                     const vector<CommandExample>&examples,
                     cmd_t cmd)
        : name(std::move(name)),
          description(std::move(description)),
          options(options),
          examples(examples), cmd(std::move(cmd)) {
    }

    Command::Command(string name, string description, const vector<CommandExample>&examples, cmd_t cmd)
        : name(std::move(name)),
          description(std::move(description)),
          examples(examples),
          cmd(std::move(cmd)) {
    }

    Command::Command(string name)
        : name(std::move(name)), cmd(do_nothing) {
    }

    CommandOption::CommandOption(string name, string description, string type)
        : name(std::move(name)),
          description(std::move(description)),
          type(std::move(type)),
          aliases({}) {
    }

    CommandOption::CommandOption(string name, string description, string type, const vector<string>&aliases)
        : name(std::move(name)),
          description(std::move(description)),
          type(std::move(type)),
          aliases(aliases) {
    }

    CommandExample::CommandExample(string command, string what_it_does): command(std::move(command)),
                                                                         whatItDoes(std::move(what_it_does)) {
    }
}
