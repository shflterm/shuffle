#include "commandmgr.h"

#include <iostream>

#include "executor.h"
#include "console.h"
#include "suggestion.h"

using namespace std;

vector<Command> commands;

void loadCommands() {
  commands.emplace_back("exit", CUSTOM);
  commands.emplace_back("cd", CUSTOM);
  commands.emplace_back("list", CUSTOM);
  commands.emplace_back("lang", CUSTOM);
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
      string suggestion = findSuggestion(input, commands);
      if (suggestion.empty()) continue;

      input += suggestion;
      cout << "\033[0m" << suggestion;
    } else {
      cout << "\033[0m" << c;
      input += c;
    }

    string suggestion = findSuggestion(input, commands);
    if (suggestion.empty()) continue;

    cout << "\033[90m" << suggestion;
    gotoxy(wherex() - (int) suggestion.size(), wherey());
  }
  white();
  if (!input.empty()) execute(input);
}
