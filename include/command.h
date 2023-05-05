//
// Created by winch on 5/5/2023.
//

#ifndef SHUFFLE_INCLUDE_COMMAND_H_
#define SHUFFLE_INCLUDE_COMMAND_H_

#include <string>

using namespace std;

enum ExecutableType { CUSTOM, EXECUTE_PROGRAM, RUN_SHFL, RUN_SAPP };

class Command {
 public:
  string name;
  ExecutableType type;
  string path;

  Command(string name, ExecutableType type, string path) : name(std::move(name)), type(type), path(std::move(path)) {}

  Command(string name, ExecutableType type) : name(std::move(name)), type(type) {}
};

void loadCommands();
void execute(const string& input);
void command();

#endif //SHUFFLE_INCLUDE_COMMAND_H_
