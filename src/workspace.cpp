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
#include "cmd/parsedcmd.h"
#include "cmd/cmdparser.h"

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

void Workspace::execute(const string &input, bool isSnippet) {
    vector<string> inSpl = splitBySpace(input);
    if (inSpl.empty()) return;

    if (!isSnippet) {
        bool isSnippetFound = false;
        for (const auto &item: snippets) {
            if (item->getName() != inSpl[0]) continue;
            isSnippetFound = true;

            term << "[*] " << item->getTarget() << newLine << newLine;
            execute(item->getTarget(), true);
        }

        if (isSnippetFound) return;
    }

    Command *app = findCommand(inSpl[0]).get();

    if (app == nullptr) {
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

    vector<string> args;
    for (int i = 1; i < inSpl.size(); ++i) inSpl.push_back(args[i]);

    parseCommand(app, args).executeApp(*this);
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
