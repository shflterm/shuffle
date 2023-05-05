//
// Created by winch on 5/5/2023.
//

#ifndef SHUFFLE_INCLUDE_COMMANDMGR_H_
#define SHUFFLE_INCLUDE_COMMANDMGR_H_

#include <vector>
#include <string>

using namespace std;

enum ExecutableType { CUSTOM, RUN_APP };
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
  string version;
  vector<string> alias;
  string author;
  string website;
  string execute;
};

extern vector<Command> commands;

void loadDefaultCommands();
void loadCommand(const CommandData& data);
void inputCommand();

vector<CommandData> getRegisteredCommands();

#endif //SHUFFLE_INCLUDE_COMMANDMGR_H_
