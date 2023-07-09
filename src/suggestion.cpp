#include "suggestion.h"

#include "sapp/sapp.h"
#include "console.h"

#include <string>
#include <utility>
#include <vector>
#include <iostream>

using namespace std;

vector<string> findSuggestion(Workspace ws,
                              const string &input,
                              const vector<string> &DICTIONARY) {
    vector<string> suggestions = createSuggestions(std::move(ws), input, DICTIONARY);
    if (suggestions.empty()) return {""};

    vector<string> res;
    for (const auto &item: suggestions) {
        if (item.size() < input.length() + 1) continue;
        res.push_back(item.substr(input.length()));
    }

    if (res.empty()) return {""};
    return res;
}

vector<string> createSuggestions(Workspace ws,
                                 const string &str,
                                 const vector<string> &DICTIONARY) {
    vector<string> res;
    for (const auto &cmd: DICTIONARY) {
        if (cmd.rfind(str, 0) != 0) continue;
        res.push_back(cmd);
    }

    return res;
}