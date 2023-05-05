//
// Created by winch on 5/5/2023.
//

#ifndef SHUFFLE_INCLUDE_COMMANDMGR_H_
#define SHUFFLE_INCLUDE_COMMANDMGR_H_

#include <vector>
#include <string>

using namespace std;

enum ExecutableType { CUSTOM, EXECUTE_PROGRAM, RUN_SHFL, RUN_SAPP };
class Command {
 public:
  string name;
  ExecutableType type;
  string value;

  Command(string name, ExecutableType type, string path) : name(std::move(name)), type(type), value(std::move(path)) {}

  Command(string name, ExecutableType type) : name(std::move(name)), type(type) {}
};

extern vector<Command> commands;

void loadCommands();
void inputCommand();

#endif //SHUFFLE_INCLUDE_COMMANDMGR_H_
