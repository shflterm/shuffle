#include "suggestion.h"
#include "utils/utils.h"

#include <regex>
#include <iostream>

using namespace std;

int main() {
  loadCommands();

  string input = "cd a";
  Workspace ws;

  vector<string> args = split(input, regex(R"(\s+)"));
  string suggestion;
  if (args.size() == 1) {
    suggestion = findSuggestion(ws, args[args.size() - 1], nullptr, commands);
  } else {
    Command *final = findCommand(args[0]);
    if (final->getName().empty() && final->getValue().empty()) {
      cout << "Both name and value is empty!" << endl;
      return 0;
    }

    for (int i = 1; i < args.size() - 1; ++i) {
      Command *sub = findCommand(args[i], final->getChildren());
      if (sub->getName().empty() && sub->getValue().empty()) {
        cout << "Both name and value is empty!" << endl;
        return 0;
      }
      final = sub;
    }

    cout << final << "\n";
    suggestion = findSuggestion(ws, args[args.size() - 1], final, final->getChildren());
  }
  if (suggestion.empty()) {
    cout << "Suggestion is empty!" << endl;
    return 0;
  }

  cout << suggestion;
  return 0;
}