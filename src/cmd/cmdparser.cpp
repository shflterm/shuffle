#include "cmdparser.h"

#include <algorithm>

#include "console.h"
#include "utils.h"

Job parseCommand(shared_ptr<Command> app, const vector<string>&args) {
    if (app == nullptr) return Job(EMPTY);

    vector<string> newArgs = args;
    for (int i = 0; i < args.size(); ++i) {
        const string&item = args[i];
        for (const auto&subcommand: app->getSubcommands()) {
            if (subcommand->getName() == trim(item)) {
                app = subcommand;
                newArgs.clear();
                newArgs.insert(newArgs.begin(), args.begin() + i + 1, args.end());
                break;
            }
        }
    }

    Job parsed = Job(app);
    const map<string, string>* parsedOptions = parseOptions(app, newArgs);
    if (parsedOptions == nullptr) return Job(EMPTY);

    parsed.options = *parsedOptions;
    return parsed;
}

map<string, string>* parseOptions(const shared_ptr<Command>& app, const vector<string>&args) {
    auto* parsedOptions = new map<string, string>();

    vector<CommandOption> options = app->getOptions();

    map<string, vector<string>> optionAbbreviations;
    for (auto&option: options) {
        optionAbbreviations[option.name].push_back(option.name);
        for (auto&abbr: option.aliases) {
            optionAbbreviations[option.name].push_back(abbr);
        }
    }

    vector<string> optionNames, optionNamesWithAbbr;
    for (const auto&option: options) {
        optionNames.push_back(option.name);
        optionNamesWithAbbr.push_back(option.name);
        for (const auto&item: option.aliases) {
            optionNamesWithAbbr.push_back(item);
        }
    }

    size_t optionIndex = 0;

    for (size_t i = 0; i < args.size(); ++i) {
        const string&arg = args[i];
        string key, value;

        if (size_t delimiterPos = arg.find('='); delimiterPos != string::npos) {
            key = arg.substr(0, delimiterPos);
            value = arg.substr(delimiterPos + 1);
        }
        else if (arg[0] == '-' && arg.size() > 1) {
            key = arg.substr(1);
            if (key[0] == '-') {
                error("Invalid option format '" + arg +
                      "'. Options should be in the format '-key value' or 'key=value'.");
                delete parsedOptions;
                return nullptr;
            }
            if (i + 1 < args.size() && args[i + 1][0] != '-') {
                value = args[i + 1];
                ++i;
            }
            else {
                error("Missing value for option '-" + key + "'.");
                delete parsedOptions;
                return nullptr;
            }
        }
        else if (arg[0] == '!' && arg.size() > 1) {
            key = arg.substr(1);
            value = "false";
        }
        else {
            if (find(optionNamesWithAbbr.begin(), optionNamesWithAbbr.end(), arg) != optionNamesWithAbbr.end()) {
                key = arg;
                value = "true";
            }
            else if (optionIndex < optionNames.size()) {
                key = optionNames[optionIndex++];
                value = arg;
                // Add this line to handle multiple arguments for an option
                while (i + 1 < args.size() && args[i + 1][0] != '-') {
                    value += " " + args[i + 1];
                    ++i;
                }
            }
            else {
                error("Unexpected argument '" + arg + "'.");
                delete parsedOptions;
                return nullptr;
            }
        }

        bool foundAbbreviation = false;
        for (const auto&[optionName, abbrs]: optionAbbreviations) {
            if (optionName == key) {
                foundAbbreviation = true;
                break;
            }
            for (const auto&abbreviation: abbrs) {
                if (abbreviation == key) {
                    key = optionName;
                    foundAbbreviation = true;
                    break;
                }
            }
            if (foundAbbreviation) {
                break;
            }
        }

        if (!foundAbbreviation) {
            error("Invalid option key '" + key + "'.");
            delete parsedOptions;
            return nullptr;
        }

        (*parsedOptions)[key] = value;
    }

    return parsedOptions;
}
