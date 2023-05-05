#include "command.h"

#include <iostream>
#include <string>
#include <utility>
#include <vector>
#include <filesystem>
#include <sys/stat.h>

#include "console.h"
#include "utils.h"

#include "completion.h"
#include "i18n.h"

using namespace std;
using std::filesystem::current_path;
namespace fs = std::filesystem;

vector<Command> commands;
#ifdef _WIN32
string path = current_path().string();
#elif __linux__
string path = current_path();
#endif

void loadCommands() {
  commands.emplace_back("exit", CUSTOM);
  commands.emplace_back("cd", CUSTOM);
  commands.emplace_back("list", CUSTOM);
  commands.emplace_back("lang", CUSTOM);
  commands.emplace_back("help", CUSTOM);
  commands.emplace_back("clear", EXECUTE_PROGRAM, "clear");
}

void execute(const string &input) {
  vector<string> cmd = split(input, regex(R"(\s+)"));
  if (cmd.empty()) return;

  bool isCommandFounded = false;
  for (const Command &command : commands) {
    if (command.name != cmd[0]) continue;
    isCommandFounded = true;

    if (command.type == CUSTOM) {
      if (cmd[0] == "exit") {
        info("exit.bye");
        exit(0);
      } else if (cmd[0] == "help") {
        info("system.wip");
      } else if (cmd[0] == "cd") {
        if (cmd.size() != 2) {
          too_many_arguments();
          return;
        }

        string newPath = cmd[1][0] == '/' || cmd[1][0] == '\\' ? cmd[1] : path.append("/").append(cmd[1]);

        struct stat sb{};
        if (stat(newPath.c_str(), &sb) == 0)
          path = newPath;
        else error("cd.directory_not_found", {newPath});
      } else if (cmd[0] == "list") {
        if (cmd.size() != 1) {
          too_many_arguments();
          return;
        }

        for (const auto &entry : fs::directory_iterator(path)) {
#ifdef _WIN32
          print(INFO, replace(entry.path().string(), path, ""));
#elif __linux__
          print(INFO, replace(entry.path(), path, ""));
#endif
        }
      } else if (cmd[0] == "lang") {
        if (cmd.size() != 2) {
          too_many_arguments();
          return;
        }

        loadLanguageFile(cmd[1]);
        info("lang.changed", {cmd[1]});
      }
    } else if (command.type == EXECUTE_PROGRAM) {
      system(command.path.c_str());
    } else if (command.type == RUN_SHFL) {
    } else if (command.type == RUN_SAPP) {
    }
    break;
  }

  if (!isCommandFounded) {
    error("system.command_not_found", {cmd[0]});
    pair<int, Command> similarWord = {1000000000, {"", CUSTOM, ""}};
    for (const Command &command : commands) {
      int dist = levenshteinDist(cmd[0], command.name);
      if (dist < similarWord.first) similarWord = {dist, command};
    }

    if (similarWord.first > 1) warning("system.check_command");
    else warning("system.similar_command", {similarWord.second.name});
  }
}

string findSuggestion(const string &input) {
  vector<string> suggestions = createSuggestions(input, commands);
  if (suggestions.empty()) return "";

  string suggestion = suggestions[0];
  if (suggestion.size() < input.length() + 1) return "";

  suggestion = suggestion.substr(input.length());
  return suggestion;
}

void command() {
  cout << path << "> ";
  cout.flush();

  string input;
  char c;
  while (true) {
    c = readChar();

    if (c == '\b') {
      if (!input.empty()) {
        gotoxy(wherex() - 1, wherey());
        cout << ' ';
        gotoxy(wherex() - 1, wherey());
        input = input.substr(0, input.length() - 1);
      }
    } else if (c == '\n' || c == '\r') {
      break;
    } else if (c == '\t') {
      string suggestion = findSuggestion(input);
      if (suggestion.empty()) continue;

      input += suggestion;
      cout << "\033[0m" << suggestion;
    } else {
      cout << "\033[0m" << c;
      input += c;
    }

    string suggestion = findSuggestion(input);
    if (suggestion.empty()) continue;

    cout << "\033[90m" << suggestion;
    gotoxy(wherex() - (int) suggestion.size(), wherey());
  }
  white();
  if (!input.empty()) execute(input);
}
