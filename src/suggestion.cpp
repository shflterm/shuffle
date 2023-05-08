#include "suggestion.h"

#include <string>
#include <vector>
#include <memory>

using namespace std;

string findSuggestion(const string &input, const vector<unique_ptr<Command>> &DICTIONARY) {
  vector<string> suggestions = createSuggestions(input, DICTIONARY);
  if (suggestions.empty()) return "";

  string suggestion = suggestions[0];
  if (suggestion.size() < input.length() + 1) return "";

  suggestion = suggestion.substr(input.length());
  return suggestion;
}

vector<string> createSuggestions(const string &str, const vector<unique_ptr<Command>> &DICTIONARY) {
  vector<string> res;
  for (const auto &item : DICTIONARY) {
    Command &command = *item;
    if (command.getName().substr(0, str.size()) == str) {
      res.push_back(command.getName());
    }
  }

  return res;
}