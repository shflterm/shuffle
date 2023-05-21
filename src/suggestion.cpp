#include "suggestion.h"

#include "sapp/sapp.h"
#include "console.h"

#include <string>
#include <iostream>
#include <utility>
#include <vector>
#include <memory>

using namespace std;

string findSuggestion(Workspace ws,
                      const string &input,
                      Command *rootCommand,
                      const vector<unique_ptr<Command>> &DICTIONARY) {
  vector<string> suggestions = createSuggestions(std::move(ws), input, rootCommand, DICTIONARY);
  if (suggestions.empty()) return "";

  string suggestion = suggestions[0];
  if (suggestion.size() < input.length() + 1) return "";

  suggestion = suggestion.substr(input.length());
  return suggestion;
}

vector<string> createSuggestions(Workspace ws,
                                 const string &str,
                                 Command *rootCommand,
                                 const vector<unique_ptr<Command>> &DICTIONARY) {
  vector<string> res;
  for (const auto &item : DICTIONARY) {
    Command &command = *item;

    Command *cmd = &command;
    if (cmd->getName().rfind("option.", 0) == 0) {
      auto *sappCmd = dynamic_cast<SAPPCommand *>(rootCommand);
      if (sappCmd == nullptr) continue;

      vector<string> suggestions = sappCmd->makeDynamicSuggestion(ws, cmd->getName().substr(7));
      for (const auto &item2 : suggestions)
        if (item2.substr(0, str.size()) == str)
          res.push_back(item2);

    } else {
      if (command.getName().substr(0, str.size()) != str) continue;

      res.push_back(command.getName());
    }
  }

  return res;
}