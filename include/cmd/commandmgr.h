//
// Created by winch on 5/5/2023.
//

#ifndef SHUFFLE_INCLUDE_COMMANDMGR_H_
#define SHUFFLE_INCLUDE_COMMANDMGR_H_

#include <vector>
#include <string>
#include <memory>
#include "workspace.h"

using namespace std;

class Command {
protected:
    string name;
    string description;
    vector<pair<string, string>> usage;
    map<string, vector<string>> options;

public:
    [[nodiscard]] const string &getName() const;

    [[nodiscard]] const string &getDescription() const;

    [[nodiscard]] const vector<pair<string, string>> &getUsage() const;

    [[nodiscard]] const map<string, vector<string>> &getOptions() const;

    virtual void run(Workspace &ws, const map<string, string> &optionValues) const;

    Command(string name, string description, map<string, vector<string>> options, vector<pair<string, string>> usage);

    Command(string name, string description, map<string, vector<string>> options);

    Command(string name, string description);

    Command(string name, map<string, vector<string>> options);

    explicit Command(string name);
};

class CommandData {
public:
    string name;
};

extern vector<shared_ptr<Command>> commands;

void loadDefaultCommands();

void loadCommand(const CommandData &data);

vector<CommandData> getRegisteredCommands();

bool addRegisteredCommand(const CommandData &data);

void loadCommands();

shared_ptr<Command> findCommand(const string &name, const vector<shared_ptr<Command>> &DICTIONARY);

shared_ptr<Command> findCommand(const string &name);

#endif //SHUFFLE_INCLUDE_COMMANDMGR_H_
