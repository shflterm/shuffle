#include "workspace.h"

#include <iostream>
#include <regex>
#include <utility>
#include <sstream>
#include <map>

#include "cmd/builtincmd.h"
#include "console.h"
#include "cmd/commandmgr.h"
#include "utils/utils.h"
#include "suggestion.h"
#include "sapp/downloader.h"
#include "sapp/sapp.h"
#include "utils/credit.h"
#include "term.h"
#include "snippetmgr.h"
#include "cmd/executedcmd.h"

using namespace std;
using namespace std::filesystem;

map<string, Workspace *> wsMap;

path Workspace::currentDirectory() {
    return dir;
}

void Workspace::moveDirectory(path newDir) {
    if (!is_directory(newDir)) {
        error("Directory '$0' not found.", {newDir.string()});
        return;
    }
    dir = std::move(newDir);

    if (dir.string()[dir.string().length() - 1] == '\\' || dir.string()[dir.string().length() - 1] == '/')
        dir = dir.parent_path();
}

vector<string> Workspace::getHistory() {
    return history;
}

void Workspace::addHistory(const string &s) {
    history.push_back(s);
    historyIndex = historyIndex + 1;
}

string Workspace::historyUp() {
    if (history.empty()) return "";

    if (0 > historyIndex - 1) return history[historyIndex];
    return history[--historyIndex];
}

string Workspace::historyDown() {
    if (history.empty()) return "";

    if (history.size() <= historyIndex + 1) return history[historyIndex];
    return history[++historyIndex];
}

map<string, string> *parseOptions(Command *app, const vector<string> &args) {
    auto *parsedOptions = new map<string, string>();
    map<string, vector<string>> optionAbbreviations = app->getOptions();
    vector<string> optionNames = app->getOptionNames();
    vector<string> optionNamesWithAbbr = app->getOptionNames();
    for (auto &option: optionNames) {
        for (const auto &item: optionAbbreviations[option]) {
            optionNamesWithAbbr.push_back(item);
        }
    }

    size_t optionIndex = 0;

    for (size_t i = 0; i < args.size(); ++i) {
        const string &arg = args[i];
        string key, value;

        size_t delimiterPos = arg.find('=');
        if (delimiterPos != string::npos) {
            key = arg.substr(0, delimiterPos);
            value = arg.substr(delimiterPos + 1);
        } else if (arg[0] == '-' && arg.size() > 1) {
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
            } else {
                error("Missing value for option '-" + key + "'.");
                delete parsedOptions;
                return nullptr;
            }
        } else if (arg[0] == '!' && arg.size() > 1) {
            key = arg.substr(1);
            value = "false";
        } else {
            if (find(optionNamesWithAbbr.begin(), optionNamesWithAbbr.end(), arg) != optionNamesWithAbbr.end()) {
                key = arg;
                value = "true";
            } else if (optionIndex < optionNames.size()) {
                key = optionNames[optionIndex++];
                value = arg;
                // Add this line to handle multiple arguments for an option
                while (i + 1 < args.size() && args[i + 1][0] != '-') {
                    value += " " + args[i + 1];
                    ++i;
                }
            } else {
                error("Unexpected argument '" + arg + "'.");
                delete parsedOptions;
                return nullptr;
            }
        }

        bool foundAbbreviation = false;
        for (const auto &entry: optionAbbreviations) {
            if (entry.first == key) {
                foundAbbreviation = true;
                break;
            }
            for (const auto &abbreviation: entry.second) {
                if (abbreviation == key) {
                    key = entry.first;
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

void Workspace::execute(const string &input) {
    vector<string> inSpl = splitBySpace(input);
    if (inSpl.empty()) return;

    bool isSnippetFound = false;
    for (const auto &item: snippets) {
        if (item->getName() != inSpl[0]) continue;
        isSnippetFound = true;

        term << "[*] " << item->getTarget() << newLine << newLine;
        execute(item->getTarget());
    }

    if (isSnippetFound) return;

    bool isCommandFound = false;
    Command *app;
    for (const auto &cmd: commands) {
        if (cmd->getName() != inSpl[0]) continue;
        isCommandFound = true;

        app = dynamic_cast<SAPPCommand *>(cmd.get());
        if (app == nullptr) app = dynamic_cast<BuiltinCommand *>(cmd.get());
        break;
    }

    if (!isCommandFound || app == nullptr) {
        error("Sorry. Command '$0' not found.", {inSpl[0]});
        pair<int, Command> similarWord = {1000000000, Command("")};
        for (const auto &cmd: commands) {
            int dist = levenshteinDist(inSpl[0], cmd->getName());
            if (dist < similarWord.first) similarWord = {dist, *cmd};
        }

        if (similarWord.first > 1) warning("Please make sure you entered the correct command.");
        else warning("Did you mean '$0'?", {similarWord.second.getName()});

        return;
    }

    ExecutedCommand executed = ExecutedCommand(app);

    vector<string> args;
    for (int i = 1; i < inSpl.size(); ++i) args.push_back(inSpl[i]);

    map<string, string> *parsedOptions = parseOptions(app, args);
    if (parsedOptions == nullptr) return;

    executed.options = *parsedOptions;
    executed.executeApp(*this);
}

vector<string> makeDictionary(const vector<shared_ptr<Command>> &cmds) {
    vector<string> dictionary;
    dictionary.reserve(cmds.size());
    for (const auto &item: cmds) {
        dictionary.push_back(item->getName());
    }
    return dictionary;
}

string getSuggestion(const Workspace &ws, const string &input) {
    vector<string> args = split(input, regex(R"(\s+)"));
    string suggestion;
    if (input[input.length() - 1] == ' ') args.emplace_back("");

    if (args.size() == 1) {
        suggestion = findSuggestion(ws, args[0], makeDictionary(commands))[0];
    } else {
        shared_ptr<Command> cmd = findCommand(args[0]);

        size_t cur = args.size() - 1;
        if (args[cur][0] == '-') {
            suggestion = findSuggestion(ws, args[cur].substr(1), cmd->getOptionNames())[0];
        } else {
            suggestion = "else";
        }
    }
    return suggestion;
}

string getHint(const Workspace &ws, const string &input) {
//    vector<string> args = split(input, regex(R"(\s+)"));
//
//    if (args.size() == 1) {
//        shared_ptr<Command> command = findCommand(args[0]);
//        if (command == nullptr) return "";
//        else return command->getDescription();
//    } else {
//        shared_ptr<Command> final = findCommand(args[0]);
//        if (final == nullptr) return "";
//
//        for (int i = 1; i < args.size(); i++) {
//            shared_ptr<Command> sub = findCommand(args[i], final->getChildren());
//            if (sub == nullptr) {
//                return final->getDescription();
//            }
//            final = sub;
//        }
//
//        return final->getDescription();
//    }
    return "";
}

string Workspace::prompt() {
    stringstream ss;
    if (!name.empty())
        ss << color(FOREGROUND, Yellow) << "[" << name << "] ";

    ss << color(FOREGROUND, Cyan) << "(";
    if (dir == dir.root_path())
        ss << dir.root_name().string();
    else if (dir.parent_path() == dir.root_path())
        ss << dir.root_name().string() << "/" << dir.filename().string();
    else
        ss << dir.root_name().string() << "/../" << dir.filename().string();
    ss << ")";

    ss << color(FOREGROUND, Yellow) << " \u2192 " << resetColor;
    return ss.str();
}

void Workspace::inputPrompt(bool enableSuggestion) {
    term << prompt();

    string input;
    if (enableSuggestion) {
        int x = wherex();
        term << newLine;
        term << teleport(x, wherey() - 1);

        int c;
        while (true) {
            c = readChar();
            term << eraseFromCursorToLineEnd;

            if (c == '\b' || c == 127) {
                if (!input.empty()) {
                    term << teleport(wherex() - 1, wherey());
                    term << eraseFromCursorToLineEnd;
                    input = input.substr(0, input.length() - 1);
                }
            } else if (c == '\n' || c == '\r') {
                break;
            } else if (c == '\t') {
                string suggestion = getSuggestion(*this, input);
                input += suggestion;
                term << "\033[0m" << suggestion;
            } else if (c == 224) {
                int i = readChar();
                int mv = (int) input.size();
                mv *= -1;
                switch (i) {
                    case 72: {
                        gotoxy(wherex() - (int) input.size(), wherey());
                        term << eraseFromCursorToLineEnd;
                        input = historyUp();
                        term << input;
                        break;
                    }
                    case 80: {
                        gotoxy(wherex() - (int) input.size(), wherey());
                        term << eraseFromCursorToLineEnd;
                        input = historyDown();
                        term << input;
                        break;
                    }
                    default:
                        break;
                }
            } else if (c == '@') {
                gotoxy(wherex() - (int) input.size() - 2, wherey());
                term << eraseFromCursorToLineEnd;
                term << color(FOREGROUND, Yellow) << "@ " << resetColor;
                string wsName;
                getline(cin, wsName);

                term << newLine;
                if (wsMap.find(wsName) != wsMap.end()) {
                    currentWorkspace = wsMap[wsName];
                } else {
                    info("{FG_BLUE}New workspace created: {BG_GREEN}$0", {wsName});
                    currentWorkspace = new Workspace(wsName);
                }
                return;
            } else if (c == '&') {
                gotoxy(wherex() - (int) input.size() - 2, wherey());
                term << eraseFromCursorToLineEnd;
                term << color(FOREGROUND, Yellow) << "& " << resetColor;
                string command;
                getline(cin, command);

                system(command.c_str());
                return;
            } else {
                string character(1, (char) c);
                term << resetColor << character;
                input += character;
            }

            string suggestion = getSuggestion(*this, input);
            term << color(FOREGROUND_BRIGHT, Black) << suggestion << resetColor;
            gotoxy(wherex() - (int) suggestion.size(), wherey());

            string hint = getHint(*this, input);
            term << saveCursorPosition
                 << teleport(wherex() - ((int) hint.size() / 2), wherey() + 1)
                 << eraseLine
                 << color(FOREGROUND_BRIGHT, Black) << hint
                 << loadCursorPosition;
        }
        term << newLine;
    } else {
        getline(cin, input);
    }

    if (!input.empty()) {
        term << eraseLine;
        addHistory(input);
        execute(input);
    }
}

Workspace::Workspace(
        const string &name) : name(name) {
    wsMap[name] = this;
}

Workspace::Workspace() =
default;
