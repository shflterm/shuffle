//
// Created by winch on 5/5/2023.
//

#ifndef SHUFFLE_INCLUDE_COMMANDMGR_H_
#define SHUFFLE_INCLUDE_COMMANDMGR_H_

#include <utility>
#include <vector>
#include <string>
#include <map>

using namespace std;

enum ExecutableType { CUSTOM, EXECUTE_PROGRAM, RUN_SHFL, RUN_SAPP };

class Command {
 public:
  string name;
  ExecutableType type;
  string value;
  vector<Command> children;

  Command(string name, ExecutableType type, string path, vector<Command> children)
      : name(std::move(name)), type(type), value(std::move(path)), children(std::move(children)) {}

  Command(string name, ExecutableType type, vector<Command> children)
      : name(std::move(name)), type(type), children(std::move(children)) {}

  Command(string name, ExecutableType type, string value)
      : name(std::move(name)), type(type), value(std::move(value)) {}

  Command(string name, ExecutableType type) : name(std::move(name)), type(type) {}
};

extern vector<Command> commands;

void loadCommands();
void inputCommand();

Command findCommand(string &name);

#endif //SHUFFLE_INCLUDE_COMMANDMGR_H_
