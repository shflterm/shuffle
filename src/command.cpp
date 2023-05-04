#include <iostream>
#include <string>
#include <utility>
#include <vector>
#include <filesystem>
#include <sys/stat.h>
#include <filesystem>

#include "console.cpp"
#include "scriptmgr.cpp"
#include "utils.cpp"

using namespace std;
using std::filesystem::current_path;
namespace fs = std::filesystem;

enum ExecutableType {
  CUSTOM, EXECUTE_PROGRAM, RUN_SHFL, RUN_SAPP
};

typedef struct Command {
  string name;
  ExecutableType type;
  string path;

  Command(string name, ExecutableType type, string path) : name(std::move(name)), type(type), path(std::move(path)) {}

  Command(string name, ExecutableType type) : name(std::move(name)), type(type) {}
} Command;

vector<Command> commands;
string path = current_path();

void loadCommands() {
  commands.emplace_back("exit", CUSTOM);
  commands.emplace_back("cd", CUSTOM);
  commands.emplace_back("list", CUSTOM);
  commands.emplace_back("help", EXECUTE_PROGRAM, "help");
  commands.emplace_back("clear", EXECUTE_PROGRAM, "clear");
}

void execute(string input) {
  vector<string> cmd = split(std::move(input), ' ');
  if (cmd.empty()) return;

  bool isCommandFounded = false;
  for (const Command &command : commands) {
    if (command.name != cmd[0]) continue;
    isCommandFounded = true;

    if (command.type == CUSTOM) {
      if (cmd[0] == "exit") {
        info("bye");
        exit(0);
      } else if (cmd[0] == "cd") {
        if (cmd.size() != 2) {
          too_many_arguments();
          return;
        }

        string newPath = cmd[1][0] == '/' || cmd[1][0] == '\\' ? cmd[1] : path.append("/").append(cmd[1]);

        struct stat sb{};
        if (stat(newPath.c_str(), &sb) == 0)
          path = newPath;
        else error("directory_not_found", {newPath});
      } else if (cmd[0] == "list") {
        if (cmd.size() != 1) {
          too_many_arguments();
          return;
        }

        for (const auto &entry : fs::directory_iterator(path))
          print(INFO, replace(entry.path(), path, ""));
      }
    } else if (command.type == EXECUTE_PROGRAM) {
      system(command.path.c_str());
    } else if (command.type == RUN_SHFL) {
    } else if (command.type == RUN_SAPP) {
    }
    break;
  }

  if (!isCommandFounded) {
    error("command_not_found", {cmd[0]});
    pair<int, Command> similarWord = {1000000000, {"", CUSTOM, ""}};
    for (const Command &command : commands) {
      int dist = levenshteinDist(cmd[0], command.name);
      if (dist < similarWord.first) similarWord = {dist, command};
    }

    if (similarWord.first <= 2) warning("check_command");
    else warning("similar_command", {similarWord.second.name});
  }
}

void command() {
  cout << path << "> ";
  string input;
  getline(cin, input);
  execute(input);
}