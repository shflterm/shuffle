#include "suggestion.h"

#include <vector>

#include "utils.h"

using std::vector, std::string, cmd::Command, cmd::commands, cmd::findCommand;

namespace suggestion {
    vector<string> makeDictionary(const vector<shared_ptr<Command>>&cmds) {
        vector<string> dictionary;
        dictionary.reserve(cmds.size());
        for (const auto&item: cmds) {
            dictionary.push_back(item->getName());
        }
        return dictionary;
    }

    vector<string> makeDictionary(const map<string, string>&variables) {
        vector<string> dictionary;
        dictionary.reserve(variables.size());
        for (auto [key, _]: variables) {
            dictionary.push_back(key);
        }
        return dictionary;
    }

    string getSuggestion(Workspace&ws, const string&input) {
        vector<string> args = splitBySpace(input);
        if (args.empty()) return "";

        string suggestion;
        if (input[input.length() - 1] == ' ') args.emplace_back("");

        if (args.size() == 1) {
            suggestion = findSuggestion(ws, args[0], makeDictionary(commands))[0];
        }
        else {
            const shared_ptr<Command> cmd = findCommand(args[0]);
            if (cmd == nullptr) return "";

            const size_t cur = args.size() - 1;

            vector<string> usedOptions;
            for (int i = 1; i < args.size(); ++i) {
                if (string item = args[i]; item[0] == '-') {
                    usedOptions.push_back(item.substr(1));
                }
                else {
                    if (i <= 1 || args[i - 1][0] != '-') {
                        usedOptions.push_back(item);
                    }
                }
            }

            if ((cur > 1 && args[cur - 1][0] == '-') || cur == 1) {
                cmd::CommandOption option = cmd->getOptions()[0];

                if (cur > 1) {
                    string optName = args[cur - 1].substr(1);
                    for (auto opt: cmd->getOptions()) {
                        if (opt.name == optName || std::find(opt.aliases.begin(), opt.aliases.end(), optName) != opt.
                            aliases.end()) {
                            option = opt;
                            break;
                        }
                    }
                }

                if (option.type == cmd::BOOLEAN) {
                    suggestion = findSuggestion(ws, args[cur], {"true", "false"})[0];
                }
                else if (option.type == cmd::FILE) {
                    // find files in currnet directory WITHOUT DIRECTORY
                    vector<string> files;
                    for (const auto&item: std::filesystem::directory_iterator(ws.currentDirectory())) {
                        if (item.is_directory()) continue;
                        files.push_back(item.path().filename().string());
                    }
                    suggestion = findSuggestion(ws, args[cur], files)[0];
                }
                else if (option.type == cmd::DIRECTORY) {
                    // find directory in currnet directory WITHOUT FILE
                    vector<string> dirs;
                    for (const auto&item: std::filesystem::directory_iterator(ws.currentDirectory())) {
                        if (!item.is_directory()) continue;
                        dirs.push_back(item.path().filename().string());
                    }
                    suggestion = findSuggestion(ws, args[cur], dirs)[0];
                }
                else if (option.type == cmd::COMMAND) {
                    // find commands
                    suggestion = getSuggestion(ws, args[cur]);
                }
            }
            else if (args[cur][0] == '-') {
                // Find unused options
                vector<string> dict;
                for (const auto&item: cmd->getOptions()) {
                    if (std::find(usedOptions.begin(), usedOptions.end(), item.name) == usedOptions.end()) {
                        dict.push_back(item.name);
                    }
                }

                suggestion = findSuggestion(ws, args[cur].substr(1), dict)[0];
            }
            else if (args[cur][0] == '(') {
                suggestion = getSuggestion(ws, args[cur].substr(1));
            }
            else if (args[cur][0] == '$') {
                suggestion = findSuggestion(ws, args[cur].substr(1), makeDictionary(ws.getVariables()))[0];
            }
            else {
                vector<string> dict;

                // For boolean options
                for (const auto&item: cmd->getOptions())
                    if (item.type == cmd::BOOLEAN &&
                        std::find(usedOptions.begin(), usedOptions.end(), item.name) == usedOptions.end())
                        dict.push_back(item.name);

                // For subcommands
                for (const auto&item: cmd->getSubcommands())
                    dict.push_back(item->getName());

                suggestion = findSuggestion(ws, args[cur], dict)[0];
            }
        }
        return suggestion;
    }

    vector<string> findSuggestion(const Workspace&ws,
                                  const string&input,
                                  const vector<string>&DICTIONARY) {
        vector<string> suggestions;
        for (const auto&cmd: DICTIONARY) {
            if (cmd.rfind(input, 0) != 0) continue;
            suggestions.push_back(cmd);
        }
        if (suggestions.empty()) return {""};

        vector<string> res;
        for (const auto&item: suggestions) {
            if (item.size() < input.length() + 1) continue;
            res.push_back(item.substr(input.length()));
        }

        if (res.empty()) return {""};
        return res;
    }

    string getHint(Workspace ws, const string&input) {
        vector<string> args = splitBySpace(input);
        if (args.empty()) return "";

        shared_ptr<Command> command;
        if (input[input.length() - 1] == ' ') args.emplace_back("");

        if (args.size() == 1) {
            command = findCommand(args[0]);
        }
        else {
            const shared_ptr<Command> cmd = findCommand(args[0]);
            if (cmd == nullptr) return "";

            const size_t cur = args.size() - 1;

            if (args[cur][0] == '(') {
                string suggestion = getSuggestion(ws, args[cur].substr(1));
                command = findCommand(args[cur].substr(1) + suggestion);
            }
            else {
                command = findCommand(args[cur], cmd->getSubcommands());
            }
        }
        if (command != nullptr) return command->createHint();
        return "";
    }
}
