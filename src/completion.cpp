#include "completion.h"

#include <string>
#include <vector>

#include "command.h"

using namespace std;

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