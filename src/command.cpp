#include <iostream>
#include <string>
#include <vector>

#include "console.cpp"
#include "scriptmgr.cpp"
#include "utils.cpp"

using namespace std;

enum ExecutableType { CUSTOM, EXECUTE_PROGRAM, RUN_SHFL, RUN_SAPP };

typedef struct {
  string name;
  ExecutableType type;
  string path;
} Command;

vector<Command> commands;

void loadCommands() {
  commands.push_back({"exit", CUSTOM, ""});
  commands.push_back({"help", EXECUTE_PROGRAM, "help"});
  commands.push_back({"clear", EXECUTE_PROGRAM, "clear"});
}

void execute(string input) {
  vector<string> cmd = split(input, ' ');
  bool isCommandFounded = false;
  for (Command command : commands) {
    if (command.name != cmd[0]) continue;
    isCommandFounded = true;

    if (command.type == CUSTOM) {
      if (cmd[0] == "exit") {
        print(INFO, "Bye.");
        exit(0);
      }
    } else if (command.type == EXECUTE_PROGRAM) {
      system(command.path.c_str());
    } else if (command.type == RUN_SHFL) {
    } else if (command.type == RUN_SAPP) {
    }
    break;
  }

  if (!isCommandFounded) {
    print(ERROR, "Command '" + cmd[0] +
                     "' not found. Make sure you have the correct command.");
    pair<int, Command> similarWord = {1000000000, {"", CUSTOM, ""}};
    for (Command command : commands) {
      int dist = levenshteinDist(cmd[0], command.name);
      if (dist < similarWord.first) similarWord = {dist, command};
    }

    if (similarWord.first <= 2)
      print(WARNING, "Did you mean '" + similarWord.second.name + "'?");
  }
}

void command() {
  cout << "> ";
  string input;
  getline(cin, input);
  execute(input);
}