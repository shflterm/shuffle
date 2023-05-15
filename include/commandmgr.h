//
// Created by winch on 5/5/2023.
//

#ifndef SHUFFLE_INCLUDE_COMMANDMGR_H_
#define SHUFFLE_INCLUDE_COMMANDMGR_H_

#include <vector>
#include <string>
#include <memory>
#include "utils/cmdexecutor.h"

using namespace std;

class Command {
 protected:
  string name;
  string description;
  string value;
  CommandExecutor executor{};
  vector<Command> children;

 public:
  [[nodiscard]] const string &getName() const;
  [[nodiscard]] const string &getDescription() const;
  [[nodiscard]] const string &getValue() const;
  [[nodiscard]] vector<unique_ptr<Command>> getChildren() const;

  virtual void run(Workspace &ws, const vector<std::string> &args) const;

  Command(string name, string description, CommandExecutor executor, vector<Command> children);
  Command(string name, string description, CommandExecutor executor);

  Command(string name, string description, vector<Command> children);
  Command(string name, string description);

  Command(string name, vector<Command> children);
  explicit Command(string name);
};

class CommandData {
 public:
  string name;
  string description;
  string value;
};

extern vector<unique_ptr<Command>> commands;

void loadDefaultCommands();
void loadCommand(const CommandData &data);

vector<CommandData> getRegisteredCommands();
void addRegisteredCommand(const CommandData &data);

void loadCommands();

Command findCommand(const string &name, const vector<unique_ptr<Command>>& DICTIONARY);
Command findCommand(const string &name);

#endif //SHUFFLE_INCLUDE_COMMANDMGR_H_
