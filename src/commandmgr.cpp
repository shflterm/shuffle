#include "commandmgr.h"

#include <iostream>
#include <regex>

#include "executor.h"
#include "console.h"
#include "suggestion.h"

using namespace std;

vector<unique_ptr<Command>> commands;

void loadDefaultCommands() {
  commands.push_back(make_unique<Command>(Command("help", helpCmd)));
  commands.push_back(make_unique<Command>(Command("shfl", shflCmd)));
  commands.push_back(make_unique<Command>(Command("cd", cdCmd)));
  commands.push_back(make_unique<Command>(Command("list", listCmd)));
  commands.push_back(make_unique<Command>(Command("lang", langCmd, {Command("en_us", CUSTOM),
                                                                    Command("ko_kr", CUSTOM)})));
  commands.push_back(make_unique<Command>(Command("exit", exitCmd)));
  commands.push_back(make_unique<Command>(Command("clear", clearCmd)));
}

void inputCommand(bool enableSuggestion) {
  cout << absolute(dir).string() << "> ";
  cout.flush();

  string input;
  if (enableSuggestion) {
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
  } else {
    getline(cin, input);
  }

  if (!input.empty()) execute(input);
}

Command findCommand(string &name) {
  for (const auto &item : commands) if (item.name == name) return item;
  return {"", CUSTOM};
}
