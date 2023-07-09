#include "suggestion.h"

#include "sapp/sapp.h"
#include "console.h"

#include <string>
#include <utility>
#include <vector>
#include <memory>

using namespace std;

vector<string> findSuggestion(Workspace ws,
                              const string &input,
                              shared_ptr<Command> rootCommand,
                              const vector<shared_ptr<Command>> &DICTIONARY) {
    vector<string> suggestions = createSuggestions(std::move(ws), input, rootCommand, DICTIONARY);
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
                                 shared_ptr<Command> rootCommand,
                                 const vector<shared_ptr<Command>> &DICTIONARY) {
    vector<string> res;
    for (const auto &cmd: DICTIONARY) {
        if (cmd && cmd->getName().rfind("option.", 0) == 0) {
            auto *sappCmd = dynamic_cast<SAPPCommand *>(rootCommand.get());
            if (sappCmd == nullptr) continue;

            vector<string> suggestions = sappCmd->makeDynamicSuggestion(ws, cmd->getName().substr(7));
            for (const auto &item2: suggestions)
                if (item2.substr(0, str.size()) == str)
                    res.push_back(item2);

        } else {
            if (cmd->getName().substr(0, str.size()) != str) continue;

            res.push_back(cmd->getName());
        }
    }

    return res;
}