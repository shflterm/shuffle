#include "suggestion.h"
#include "utils/utils.h"
#include "term.h"

#include <regex>
#include <iostream>



int main() {
  loadCommands();

  string input = "cd a";
  Workspace ws;

  vector<string> args = split(input, regex(R"(\s+)"));
  string suggestion;
  if (args.size() == 1) {
    suggestion = findSuggestion(ws, args[args.size() - 1], nullptr, commands);
  } else {
    shared_ptr<Command>final = findCommand(args[0]);
    if (final->getName().empty() && final->getValue().empty()) {
      term << "Both name and value is empty!" << endl;
      return 0;
    }

    for (int i = 1; i < args.size() - 1; ++i) {
      shared_ptr<Command>sub = findCommand(args[i], final->getChildren());
      if (sub->getName().empty() && sub->getValue().empty()) {
        term << "Both name and value is empty!" << endl;
        return 0;
      }
      final = sub;
    }

    term << final << "\n";
    suggestion = findSuggestion(ws, args[args.size() - 1], final, final->getChildren());
  }
  if (suggestion.empty()) {
    term << "Suggestion is empty!" << endl;
    return 0;
  }

  term << suggestion;
  return 0;
}