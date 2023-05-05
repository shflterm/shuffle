#include "commandmgr.h"

#include <iostream>
#include <regex>

#include "executor.h"
#include "console.h"
#include "suggestion.h"
#include "utils.h"

using namespace std;

vector<Command> commands;

void loadCommands() {
  commands.emplace_back("exit", CUSTOM);
  commands.emplace_back("cd", CUSTOM);
  commands.emplace_back("list", CUSTOM);
  commands.push_back(Command("lang", CUSTOM, {Command("en_us", CUSTOM),
                                              Command("ko_kr", CUSTOM)}));
  commands.emplace_back("help", CUSTOM);
  commands.emplace_back("clear", EXECUTE_PROGRAM, "clear");
}

void inputCommand() {
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
      vector<string> args = split(input, regex(R"(\s+)"));
      string suggestion;
      if (args.size() == 1) {
        suggestion = findSuggestion(args[args.size() - 1], commands);
      } else {
        Command command = findCommand(args[0]);
        suggestion = findSuggestion(args[args.size() - 1], command.children);
      }
      if (suggestion.empty()) continue;

      input += suggestion;
      cout << "\033[0m" << suggestion;
    } else {
      cout << "\033[0m" << c;
      input += c;
    }

    // suggestion
    vector<string> args = split(input, regex(R"(\s+)"));
    string suggestion;
    if (args.size() == 1) {
      suggestion = findSuggestion(args[args.size() - 1], commands);
    } else {
      Command command = findCommand(args[0]);
      suggestion = findSuggestion(args[args.size() - 1], command.children);
    }
    if (suggestion.empty()) continue;

    cout << "\033[90m" << suggestion;
    gotoxy(wherex() - (int) suggestion.size(), wherey());
  }
  white();
  if (!input.empty()) execute(input);
}

Command findCommand(string &name) {
  for (const auto &item : commands) if (item.name == name) return item;
  return {"", CUSTOM};
}
