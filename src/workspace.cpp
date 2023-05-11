#include "workspace.h"

#include <iostream>
#include <regex>

#include "console.h"
#include "commandmgr.h"
#include "sapp/sapp.h"
#include "utils/utils.h"
#include "suggestion.h"
#include "hash_suggestion.h"

using namespace std;

void Workspace::execute(const string &input) {
  vector<string> args = split(input, regex(R"(\s+)"));
  if (args.empty()) return;

  bool isCommandFounded = false;
  for (const auto &item : commands) {
    Command &command = *item;
    if (command.getName() != args[0]) continue;
    isCommandFounded = true;

    vector<string> newArgs;
    for (int i = 1; i < args.size(); ++i) newArgs.push_back(args[i]);
    if (command.getType() == CUSTOM) {
      command.run(*this, args);
    } else if (command.getType() == SAPP) {
      dynamic_cast<SAPPCommand &>(command).run(*this, newArgs);
    } else if (command.getType() == EXECUTABLE) {
      string cmd = command.getValue();
      for (const auto &arg : newArgs) {
        cmd.append(" ").append(arg);
      }
      system(cmd.c_str());
    }
    break;
  }

  if (!isCommandFounded) {
    error("system.command_not_found", {args[0]});
    pair<int, Command> similarWord = {1000000000, {"", "", CUSTOM, ""}};
    for (const auto &item : commands) {
      Command &command = *item;
      int dist = levenshteinDist(args[0], command.getName());
      if (dist < similarWord.first) similarWord = {dist, command};
    }

    if (similarWord.first > 1) warning("system.check_command");
    else warning("system.similar_command", {similarWord.second.getName()});
  }
}

void Workspace::inputCommand(bool enableSuggestion) {
  cout << FG_CYAN << "(" << dir.root_name().string() << "/../" << dir.filename().string() << ")"
       << FG_YELLOW << " \u2192 " << RESET;
  cout.flush();

  string input;
  if (enableSuggestion) {
    char c;
    while (true) {
      c = readChar();

      if (c == '\b' || c == 127) {
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
      } else if (c == '#' && input.empty()) {
        gotoxy(wherex() - 2, wherey());
        cout << FG_YELLOW << "# " << RESET;
        string prompt;
        getline(cin, prompt);

        info(createHashSuggestion(prompt));
        return;
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
  } else {
    getline(cin, input);
  }

  if (!input.empty()) execute(input);
}
