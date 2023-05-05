#include "suggestion.h"

#include <string>
#include <vector>

using namespace std;

string findSuggestion(const string &input, const vector<Command> &DICTIONARY) {
  vector<string> suggestions = createSuggestions(input, DICTIONARY);
  if (suggestions.empty()) return "";

  string suggestion = suggestions[0];
  if (suggestion.size() < input.length() + 1) return "";

  suggestion = suggestion.substr(input.length());
  return suggestion;
}

vector<string> createSuggestions(const string &str, const vector<Command> &DICTIONARY) {
  vector<string> res;
  std::vector<std::string> result;
  for (const auto &command : DICTIONARY) {
    if (command.name.substr(0, str.size()) == str) {
      res.push_back(command.name);
    }
  }

  return res;
}