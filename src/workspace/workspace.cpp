#include "workspace/workspace.h"

#include <iostream>
#include <utility>
#include <map>
#include <sstream>
#include <json/reader.h>
#include <json/value.h>
#include <json/writer.h>

#include "shfl.h"
#include "appmgr/appmgr.h"
#include "cmd/cmdparser.h"
#include "cmd/job.h"
#include "suggestion/suggestion.h"
#include "utils/console.h"
#include "utils/utils.h"
#include "workspace/snippetmgr.h"
#include "ai/shlfai.h"

using std::cout, std::endl, std::cin, std::stringstream, std::make_shared, std::map, job::Job, cmd::Command,
        suggestion::getSuggestion, suggestion::getHint;

map<string, Workspace *> wsMap;

path Workspace::currentDirectory() const {
    return absolute(path(dir));
}

void Workspace::moveDirectory(const path&newDir) {
    const path path = absolute(dir / newDir);
    if (!is_directory(path)) {
        error("Directory '$0' not found.", {path.string()});
        return;
    }
    dir = path.string();

    executableFilesInCurrentDirectory = getExecutableFilesInPath({dir});

    if (dir[dir.length() - 1] == '\\' || dir[dir.length() - 1] == '/')
        dir = currentDirectory().parent_path().string();
}

vector<string> Workspace::getHistory() {
    return history;
}

void Workspace::addHistory(const string&s) {
    history.push_back(s);
    historyIndex = static_cast<int>(history.size());
}

map<string, string> Workspace::getVariables() {
    return variables;
}

shared_ptr<Job> Workspace::getCurrentJob() {
    return currentJob;
}

string Workspace::historyUp() {
    if (history.empty()) return "";

    if (0 > historyIndex - 1) return history[historyIndex];
    return history[--historyIndex];
}

string Workspace::historyDown() {
    if (history.empty()) return "";

    if (history.size() <= historyIndex + 1) return history[history.size() - 1];
    return history[++historyIndex];
}

string Workspace::processArgument(string argument) {
    if (argument[0] == '"' && argument.back() == '"') argument = argument.substr(1, argument.size() - 2);

    if (argument[0] == '$') argument = variables[argument.substr(1)];
    else if (argument[argument.size() - 1] == '!') {
        string cmd = argument.substr(0, argument.size() - 1);
        const shared_ptr<Job> job = createJob(cmd);
        if (job != nullptr && job->isSuccessed())
            argument = job->start(this, true);
    }

    return argument;
}

shared_ptr<Job> Workspace::createJob(string&input) {
    if (input[0] == '(' && input[input.size() - 1] == ')')
        input = input.substr(1, input.size() - 2);

    if (std::smatch matches; regex_match(input, matches, regex("(\\w*)\\s*=\\s*(\"([^\"]*)\"|([\\s\\S]+)*)"))) {
        const string name = matches[1].str();
        string value;
        if (matches[3].matched) {
            value = matches[3].str();
        }
        else {
            value = matches[2].str();
        }
        variables[name] = processArgument(value);

        return make_shared<Job>(Job(job::VARIABLE));
    }

    vector<string> inSpl = splitBySpace(input);
    if (inSpl.empty()) return {};

    for (const auto&item: snippets) {
        if (item->getName() != inSpl[0]) continue;
        string target = item->getTarget();

        cout << "[*] " << target << endl << endl;
        createJob(target)->start(this);
        return make_shared<Job>(Job(job::SNIPPET));
    }

    shared_ptr<Command> app = cmd::findCommand(inSpl[0]);

    vector<string> args;
    for (int i = 1; i < inSpl.size(); ++i) {
        args.push_back(processArgument(inSpl[i]));
    }

    if (app == nullptr) {
        if (const path script = currentDirectory() / inSpl[0]; !is_directory(script) && endsWith(inSpl[0], ".ss")) {
            args.push_back("script=" + absolute(script).string());
            app = make_shared<Command>(Command(
                "SCRIPT", "A SCRIPT COMMAND",
                {cmd::CommandOption("script", "scriptPath", "text", true)},
                {},
                [=](Workspace* ws, map<string, string>&options, bool bgMode, string id) {
                    vector<string> scriptCommands;
                    for (const auto&line: split(readFile(path(options["script"])), regex("\n"))) {
                        scriptCommands.push_back(trim(line));
                    }

                    for (auto cmd: scriptCommands) {
                        if (const shared_ptr<Job> job = createJob(cmd); job != nullptr)job->start(this);
                    }
                    return "true";
                }));
        }
        if (isExecutableInPath(currentDirectory(), inSpl[0])) {
            return make_shared<Job>(Job(job::EXECUTABLE_COMMAND, input));
        }
    }

    auto parsed = make_shared<Job>(parseCommand(app, args));
    currentJob = parsed;
    return parsed;
}

string Workspace::prompt(const bool fullPath) const {
    stringstream ss;
    if (!name.empty())
        ss << fg_yellow << "[" << name << "] ";

    const path dir = currentDirectory();
    ss << fg_cyan << "(";
    if (fullPath)
        ss << dir.string();
    else {
        if (dir == dir.root_path())
            ss << dir.root_name().string();
        else if (dir.parent_path() == dir.root_path())
            ss << dir.root_name().string() << "/" << dir.filename().string();
        else
            ss << dir.root_name().string() << "/.../" << dir.filename().string();
    }
    ss << ")";

    ss << fg_yellow << " \u2192 " << reset;
    return ss.str();
}

void handleBackspace(const Workspace* ws, string&input, int&cursor, const int inputStartX) {
    if (input.empty() || cursor - 1 < inputStartX) return;
    cursor--;
    input.erase(cursor - inputStartX, 1);
    cout << save_cursor_pos
            << teleport(0, wherey()) << erase_line << ws->prompt() << input
            << restore_cursor_pos << teleport(wherex() - 1, wherey());
}

void handleEnter(Workspace* ws, string&input) {
    cout << endl;

    if (!input.empty()) {
        cout << erase_line;
        ws->addHistory(input);
        shared_ptr<Job> job = ws->createJob(input);
        if (job->isEmptyCausedByNoSuchCommand()) {
            vector<string> inSpl = splitBySpace(input);
            error("Sorry. Command '$0' not found.", {inSpl[0]});
            pair similarWord = {1000000000, Command("")};
            for (const auto&cmd: appmgr::getCommands()) {
                if (int dist = levenshteinDist(inSpl[0], cmd->getName());
                    dist < similarWord.first)
                    similarWord = {dist, *cmd};
            }

            if (similarWord.first > 1) warning("Please make sure you entered the correct command.");
            else warning("Did you mean '$0'?", {similarWord.second.getName()});
            return;
        }
        if (job->isCommand()) job->start(ws);
    }
}

void handleTab(Workspace* ws, string&input, int&cursor, int&inputStartX) {
    if (input.empty()) {
        cout << teleport(0, wherey()) << erase_line << ws->prompt(true);
        inputStartX = wherex();
    }
    else {
        string suggestion = getSuggestion(*ws, input);
        if (suggestion[0] == '<' && suggestion.back() == '>') return;

        cout << teleport(inputStartX + input.size(), wherey());
        input += suggestion;
        cout << "\033[0m" << suggestion;
        cursor += static_cast<int>(suggestion.size());
    }
}

void handleUpArrow(Workspace* ws, string&input) {
    cout << teleport(wherex() - input.size(), wherey());
    cout << erase_cursor_to_end;
    input = ws->historyUp();
    cout << input;
}

void handleDownArrow(Workspace* ws, string&input) {
    cout << teleport(wherex() - input.size(), wherey());
    cout << erase_cursor_to_end;
    input = ws->historyDown();
    cout << input;
}

void handleLeftArrow(const int inputStartX, int&cursor) {
    if (wherex() > inputStartX) {
        cout << teleport(wherex() - 1, wherey());
        cursor--;
    }
}

void handleRightArrow(const int inputStartX, int&cursor, const string&input) {
    if (wherex() < inputStartX + static_cast<int>(input.size())) {
        cout << teleport(wherex() + 1, wherey());
        cursor++;
    }
}

void handleDelete(const Workspace* ws, string&input, const int&cursor, const int inputStartX) {
    if (input.empty()) return;
    int deleteAt = cursor - inputStartX;
    if (deleteAt >= input.size()) return;
    if (deleteAt < 0) deleteAt = 0;

    input.erase(deleteAt, 1);
    cout << save_cursor_pos
            << teleport(0, wherey()) << erase_line << ws->prompt() << input
            << restore_cursor_pos;
}

bool handleAt(const string&input) {
    if (!input.empty()) return false;

    cout << teleport(wherex() - static_cast<int>(input.size()) - 2, wherey());
    cout << erase_cursor_to_end;
    cout << fg_yellow << "@ " << reset;
    string wsName;
    getline(cin, wsName);

    cout << endl;
    if (wsMap.find(wsName) != wsMap.end()) {
        currentWorkspace = wsMap[wsName];
    }
    else {
        info("{FG_BLUE}New workspace created: {BG_GREEN}$0", {wsName});
        currentWorkspace = new Workspace(wsName);
    }

    return true;
}

bool handleAi(Workspace* ws, const string&input) {
    if (!input.empty()) return false;

    cout << teleport(wherex() - static_cast<int>(input.size()) - 2, wherey());
    cout << erase_cursor_to_end;
    cout << fg_yellow << "# " << reset;
    string aiPrompt;
    getline(cin, aiPrompt);

    shflai_response response = generateResponse(ws, aiPrompt);
    if (response.type == shflai_response::TEXT) {
        cout << response.result_str << endl;
    }
    else if (response.type == shflai_response::COMMAND) {
        info("Shuffle AI(Beta) wants to run the command $0 $1 $2. Do you want to run it? [Y/n] ",
             {"\033[100m", trim(response.result_str), "\033[0m"}, false);
        if (int answer = readChar(); answer == 'Y' || answer == 'y') {
            cout << endl << ws->prompt() << trim(response.result_str) << endl;
            cout.flush();
            ws->createJob(response.result_str)->start(ws);
        }
        else cout << "Aborted." << endl;
    }

    warning("* This answer was generated by Shuffle AI(Beta). It may not be correct.");
    return true;
}

auto handleInput(const Workspace* ws, string&input, int&cursor, const int&inputStartX, const int&c) -> void {
    cout << erase_line;
    input.insert(cursor - inputStartX, 1, static_cast<char>(c));
    cursor++;
    int x = wherex();
    cout << teleport(0, wherey()) << erase_line << ws->prompt() << input << teleport(x + 1, wherey());
}

void Workspace::inputPrompt() {
    cout << prompt();
    int inputStartX = wherex();
    int cursor = inputStartX;

    string input;
    if (isAnsiSupported()) {
        const int x = wherex();
        cout << endl;
        cout << teleport(x, wherey() - 1);
    }

    while (true) {
        int c = readChar();

        switch (c) {
            case '\b':
            case 127:
                handleBackspace(this, input, cursor, inputStartX);
                break;
            case '\n':
            case '\r':
                handleEnter(this, input);
                return;
            case '\t': {
                handleTab(this, input, cursor, inputStartX);
                break;
            }
#ifdef _WIN32
            case 0xE0:
                switch (readChar()) {
                    case 72:
                        handleUpArrow(this, input);
                        break;
                    case 80:
                        handleDownArrow(this, input);
                        break;
                    case 75:
                        handleLeftArrow(inputStartX, cursor);
                        break;
                    case 77:
                        handleRightArrow(inputStartX, cursor, input);
                        break;
                    case 83:
                        handleDelete(this, input, cursor, inputStartX);
                        break;
                    default:
                        break;
                }
                break;
#elif defined(__linux__) || defined(__APPLE__)
            case 0x1B: {
                int i = readChar();
                if (i != '[') break;
                i = readChar();
                switch (i) {
                    case 65:
                        handleUpArrow(this, input);
                        break;
                    case 66:
                        handleDownArrow(this, input);
                        break;
                    case 68:
                        handleLeftArrow(inputStartX, cursor);
                        break;
                    case 67:
                        handleRightArrow(inputStartX, cursor, input);
                        break;
                    case 51:
                        if (readChar() != 126) break;
                        handleDelete(this, input, cursor, inputStartX);
                        break;
                    default:
                        break;
                }
                break;
            }
#endif
            case '@': {
                if (handleAt(input)) return;
            }
            case '#':
                if (handleAi(this, input)) return;
            default: {
                handleInput(this, input, cursor, inputStartX, c);
            }
        }

        if (isAnsiSupported()) {
            int suggestionStartX = inputStartX + input.size();
            string suggestion = getSuggestion(*this, input);
            cout << save_cursor_pos
                    << teleport(suggestionStartX, wherey()) << fgb_black << suggestion << reset
                    << restore_cursor_pos;

            string hint = getHint(*this, input + suggestion);
            const int xPos = wherex() - static_cast<int>(hint.size()) / 2;
            cout << save_cursor_pos
                    << teleport(xPos < 0 ? 0 : xPos, wherey() + 1)
                    << erase_line
                    << fgb_black << hint
                    << restore_cursor_pos;
        }
    }
}

string Workspace::getName() {
    return name;
}

Workspace::Workspace(
    const string&name) : name(name) {
    wsMap[name] = this;
    executableFilesInPath = getExecutableFilesInPath(getPathDirectories());
}

Workspace::Workspace() {
    executableFilesInPath = getExecutableFilesInPath(getPathDirectories());
}
