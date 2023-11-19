#include "suggestion.h"

#include <vector>

using std::vector, std::string;

vector<string> findSuggestion(const Workspace& ws,
                              const string &input,
                              const vector<string> &DICTIONARY) {
    const vector<string> suggestions = createSuggestions(ws, input, DICTIONARY);
    if (suggestions.empty()) return {""};

    vector<string> res;
    for (const auto &item: suggestions) {
        if (item.size() < input.length() + 1) continue;
        res.push_back(item.substr(input.length()));
    }

    if (res.empty()) return {""};
    return res;
}

vector<string> createSuggestions(const Workspace& ws,
                                 const string &str,
                                 const vector<string> &DICTIONARY) {
    vector<string> res;
    for (const auto &cmd: DICTIONARY) {
        if (cmd.rfind(str, 0) != 0) continue;
        res.push_back(cmd);
    }

    return res;
}