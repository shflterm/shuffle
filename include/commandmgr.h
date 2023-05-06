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

enum ExecutableType { CUSTOM, SAPP, EXECUTABLE };
class Command {
 protected:
  string name;
  ExecutableType type;
  string value;
  CommandExecutor executor{};
 public:
  [[nodiscard]] const string &getName() const;
  [[nodiscard]] ExecutableType getType() const;
  [[nodiscard]] const string &getValue() const;

  virtual void run(const vector<std::string> &args) const;

  Command(string name, ExecutableType type, string value);

  Command(string name, CommandExecutor executor);

  explicit Command(string name);
};

class CommandData {
 public:
  string name;
  string value;
  ExecutableType type;
};

extern vector<unique_ptr<Command>> commands;

void loadDefaultCommands();
void loadCommand(const CommandData &data);
void inputCommand(bool enableSuggestion);

vector<CommandData> getRegisteredCommands();
void addRegisteredCommand(const CommandData &data);

#endif //SHUFFLE_INCLUDE_COMMANDMGR_H_
