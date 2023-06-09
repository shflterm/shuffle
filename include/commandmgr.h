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
  string value;
  vector<Command> children;

 public:
  [[nodiscard]] const string &getName() const;
  [[nodiscard]] const string &getDescription() const;
  [[nodiscard]] const string &getValue() const;
  [[nodiscard]] vector<unique_ptr<Command>> getChildren() const;
  void addChild(const Command &command);

  virtual void run(Workspace &ws, const vector<std::string> &args) const;

  Command(string name, string description, vector<Command> children);
  Command(string name, string description);

  Command(string name, vector<Command> children);
  explicit Command(string name);
};

class OptionSubCommand : public Command {
 public:
  OptionSubCommand(const string &name, string description);
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

Command *findCommand(const string &name, const vector<unique_ptr<Command>> &DICTIONARY);
Command *findCommand(const string &name);

#endif //SHUFFLE_INCLUDE_COMMANDMGR_H_
