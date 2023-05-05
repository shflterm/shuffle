//
// Created by winch on 5/5/2023.
//

#ifndef SHUFFLE_INCLUDE_COMMANDMGR_H_
#define SHUFFLE_INCLUDE_COMMANDMGR_H_

#include <vector>
#include <string>

using namespace std;

enum ExecutableType { CUSTOM, SAPP, EXECUTABLE };
class Command {
 public:
  string name;
  ExecutableType type;
  string value;

  Command(string name, ExecutableType type, string path) : name(std::move(name)), type(type), value(std::move(path)) {}

  Command(string name, ExecutableType type) : name(std::move(name)), type(type) {}
};

class CommandData {
 public:
  string name;
  string execute;
  ExecutableType type;
};

extern vector<Command> commands;

void loadDefaultCommands();
void loadCommand(const CommandData& data);
void inputCommand();

vector<CommandData> getRegisteredCommands();
void addRegisteredCommand(const CommandData& data);

#endif //SHUFFLE_INCLUDE_COMMANDMGR_H_
