#include "suggestion.h"
#include "utils.h"
#include "term.h"
#include "sapp.h"

#include <iostream>

using namespace std;

vector<string> makeDictionary_(const vector<shared_ptr<Command>> &cmds) {
    vector<string> dictionary;
    dictionary.reserve(cmds.size());
    for (const auto &item: cmds) {
        dictionary.push_back(item->getName());
    }
    return dictionary;
}

void loadCommands() {
    clearCommands();

    for (const CommandData &command: getRegisteredCommands()) {
        loadApp(command);
    }
}

int main() {
    loadCommands();

    string input = "test colored -message asdf -";
    Workspace ws;

    vector<string> args = split(input, regex(R"(\s+)"));
    string suggestion;
    if (input[input.length() - 1] == ' ') args.emplace_back("");

    if (args.size() == 1) {
        suggestion = findSuggestion(ws, args[0], makeDictionary_(commands))[0];
    } else {
        shared_ptr<Command> cmd = findCommand(args[0]);

        size_t cur = args.size() - 1;

        vector<string> usedOptions;
        for (int i = 1; i < args.size(); ++i) {
            string item = args[i];
            if (item[0] == '-') {
                usedOptions.push_back(item.substr(1));
            } else {
                if (i <= 1 || args[i - 1][0] != '-') {
                    usedOptions.push_back(item);
                }
            }
        }

        if (args[cur][0] == '-') {
            vector<string> optionNames;
            for (const auto &item: cmd->getOptions()) {
                if (std::find(usedOptions.begin(), usedOptions.end(), item.name) == usedOptions.end()) {
                    optionNames.push_back(item.name);
                }
            }
            suggestion = findSuggestion(ws, args[cur].substr(1), optionNames)[0];
        } else {
            vector<string> optionNames;
            for (const auto &item: cmd->getOptions())
                if (item.type == BOOL_T &&
                    std::find(usedOptions.begin(), usedOptions.end(), item.name) == usedOptions.end())
                    optionNames.push_back(item.name);
            suggestion = findSuggestion(ws, args[cur], optionNames)[0];
        }
    }
    cout << suggestion;
    return 0;
}