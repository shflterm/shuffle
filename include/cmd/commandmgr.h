//
// Created by winch on 5/5/2023.
//

#ifndef SHUFFLE_INCLUDE_COMMANDMGR_H_
#define SHUFFLE_INCLUDE_COMMANDMGR_H_

#include <vector>
#include <string>
#include <memory>
#include "workspace.h"

using std::pair, std::shared_ptr;

enum OptionType {
    TEXT_T,
    BOOL_T,
    INT_T,
};

class CommandOption {
public:
    string name;
    OptionType type;
    vector<string> aliases;

    CommandOption(string name, OptionType type, const vector<string> &aliases);
};

class Command {
protected:
    string name;
    string description;
    vector<Command> subcommands;
    vector<CommandOption> options;

public:
    [[nodiscard]] const string &getName() const;

    [[nodiscard]] const string &getDescription() const;

    [[nodiscard]] const vector<Command> &getSubcommands() const;

    [[nodiscard]] const vector<CommandOption> &getOptions() const;

    virtual void run(Workspace &ws, map<string, string> &optionValues) const;

    Command(string name, string description, const vector<Command> &subcommands, const vector<CommandOption> &options);

    Command(string name, string description, const vector<Command> &subcommands);

    Command(string name, string description, const vector<CommandOption> &options);

    Command(string name, string description);

    Command(string name);
};

class CommandData {
public:
    string name;
};

extern vector<shared_ptr<Command>> commands;

void loadDefaultCommands();

vector<CommandData> getRegisteredCommands();

bool addRegisteredCommand(const CommandData &data);

void clearCommands();

shared_ptr<Command> findCommand(const string &name, const vector<shared_ptr<Command>> &DICTIONARY);

shared_ptr<Command> findCommand(const string &name);

#endif //SHUFFLE_INCLUDE_COMMANDMGR_H_
