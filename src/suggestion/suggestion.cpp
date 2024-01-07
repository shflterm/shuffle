#include "suggestion/suggestion.h"

#include <vector>
#include <utils/console.h>
#include <workspace/snippetmgr.h>

#include "suggestion/proponent.h"
#include "appmgr/appmgr.h"

using std::vector, std::string, cmd::Command, cmd::findCommand;

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

    string getVariableSuggestion(Workspace&ws, std::smatch::const_reference key, std::smatch::const_reference value) {
        if (!value.matched) return "";

        const vector<string> spl = splitBySpace(value);
        if (spl.empty()) return "";

        const int cur = spl.size() - 1;
        if (spl[cur][0] == '(') {
            string cmdName = spl[cur].substr(1);
            if (cmdName.empty()) return "";

            if (cmdName.length() > 1 && cmdName[cmdName.length() - 1] == ')')
                cmdName = cmdName.substr(0, cmdName.length() - 1);
            if (cmdName.length() > 2 && cmdName[cmdName.length() - 2] == ')'
                && spl[cur][spl[cur].length() - 1] == '!')
                cmdName = cmdName.substr(0, cmdName.length() - 2);

            const vector<string> cmdNameSpl = splitBySpace(cmdName);

            shared_ptr<Command> cmd = findCommand(cmdNameSpl.front());
            if (cmd == nullptr) return getSuggestion(ws, cmdName);
            for (int i = 1; i < cmdNameSpl.size(); ++i) {
                if (cmd == nullptr) break;
                cmd = findCommand(cmdNameSpl[i], cmd->getSubcommands());
            }

            if (cmd == nullptr || cmdName[cmdName.size() - 1] == ' ') {
                if (string suggestion = getSuggestion(ws, cmdName);
                    !suggestion.empty())
                    return suggestion;
            }

            cmdName = spl[cur].substr(1);
            if (cmdName.length() > 1 && cmdName[cmdName.length() - 1] == ')')
                return "!";
            if (cmdName.length() > 2 && cmdName[cmdName.length() - 2] == ')'
                && spl[cur][spl[cur].length() - 1] == '!')
                return "";
            return ")!";
        }
        if (spl[cur][0] == '$')
            return findSuggestion(ws, spl[cur].substr(1), makeDictionary(ws.getVariables()))[0];
        return "";
    }

    string getSuggestion(Workspace&ws, const string&input) {
        if (std::smatch matches; regex_match(input, matches, regex("(\\w*)\\s*=\\s*(\"([^\"]*)\"|([\\s\\S]+)*)"))) {
            // when input is a variable
            return getVariableSuggestion(ws, matches[1], matches[2]);
        }

        vector<string> spl = splitBySpace(input);
        if (spl.empty()) return "";

        string suggestion;
        if (input[input.length() - 1] == ' ') spl.emplace_back("");

        if (spl.size() == 1) {
            vector<string> dict = makeDictionary(appmgr::getCommands());
            for (const auto&snippet: snippets) dict.push_back(snippet->getName());

            vector<string> suggestions = findSuggestion(ws, spl[0], dict);
            std::sort(suggestions.begin(), suggestions.end(), [](const string&a, const string&b) {
                return a.length() < b.length();
            });

            suggestion = suggestions[0];
        }
        else {
            shared_ptr<Command> cmd = findCommand(spl[0]);
            if (cmd == nullptr) return "";

            int loops = 1;

            for (int i = 1; i < spl.size() - 1; ++i) {
                if (const shared_ptr<Command> sub = findCommand(spl[i], cmd->getSubcommands()); sub != nullptr) {
                    cmd = sub;
                    loops++;
                }
            }

            vector args(spl.begin() + loops, spl.end());

            const size_t cur = args.size();

            vector<string> usedOptions;
            for (int i = 0; i < args.size(); ++i) {
                if (string item = args[i]; item[0] == '-') {
                    usedOptions.push_back(item.substr(1));
                }
                else {
                    if (i <= 1 || args[i - 1][0] != '-') {
                        usedOptions.push_back(item);
                    }
                }
            }

            bool isEnoughOptions = cmd->getRequiredOptions().size() <= cur - 1;
            bool hasOnlyOneOptionalOption = cmd->getOptions().size() == 1 && cmd->getRequiredOptions().empty();
            bool hasSpecifiedOption = cur > 1 && !args[cur - 2].empty() && args[cur - 2][0] == '-';
            if (!isEnoughOptions) {
                cmd::CommandOption option = cmd->getRequiredOptions()[cur - 1];

                Proponent proponent = findProponent(option.type);
                suggestion = proponent.makeProp(&ws, option, args, cur - 1);
            }
            else if (hasOnlyOneOptionalOption) {
                cmd::CommandOption option = cmd->getOptions()[0];

                Proponent proponent = findProponent(option.type);
                suggestion = proponent.makeProp(&ws, option, args, cur - 1);
            }
            else if (hasSpecifiedOption) {
                string optName = args[cur - 2].substr(1);
                for (auto option: cmd->getOptions()) {
                    if (option.name == optName ||
                        std::find(option.aliases.begin(), option.aliases.end(), optName) != option.aliases.end()) {
                        Proponent proponent = findProponent(option.type);
                        suggestion = proponent.makeProp(&ws, option, args, cur - 1);
                        break;
                    }
                }
            }

            if (!suggestion.empty()) return suggestion;

            if (spl[cur][0] == '-') {
                // Find unused options
                vector<string> dict;
                for (const auto&item: cmd->getOptions()) {
                    if (std::find(usedOptions.begin(), usedOptions.end(), item.name) == usedOptions.end()) {
                        dict.push_back(item.name);
                    }
                }

                suggestion = findSuggestion(ws, spl[cur].substr(1), dict)[0];
            }
            else if (spl[cur][0] == '(') {
                suggestion = getSuggestion(ws, spl[cur].substr(1));
            }
            else if (spl[cur][0] == '$') {
                suggestion = findSuggestion(ws, spl[cur].substr(1), makeDictionary(ws.getVariables()))[0];
            }
            else {
                vector<string> dict;

                // For boolean options
                for (const auto&item: cmd->getOptions())
                    if (item.type == "boolean" &&
                        std::find(usedOptions.begin(), usedOptions.end(), item.name) == usedOptions.end())
                        dict.push_back(item.name);

                // For subcommands
                for (const auto&item: cmd->getSubcommands())
                    dict.push_back(item->getName());

                suggestion = findSuggestion(ws, spl[cur], dict)[0];
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
