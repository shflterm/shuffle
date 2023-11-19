//
// Created by winch on 5/11/2023.
//

#ifndef SHUFFLE_INCLUDE_WORKSPACE_H_
#define SHUFFLE_INCLUDE_WORKSPACE_H_

#include <string>
#include <filesystem>
#include <vector>
#include <map>

class Workspace;

#include "parsedcmd.h"

using std::string, std::vector, std::map, std::filesystem::path, std::filesystem::current_path;

class Workspace {
    string name;
    path dir = current_path();
    vector<string> history;
    int historyIndex = 0;
    map<string, string> variables;

    [[nodiscard]] string prompt() const;

public:
    path currentDirectory();

    void moveDirectory(path newDir);

    vector<string> getHistory();

    void addHistory(const string&s);

    map<string, string> getVariables();

    string historyUp();

    string historyDown();

    string processArgument(string argument);

    ParsedCommand parse(string input);

    void inputPrompt();

    string getName();

    explicit Workspace(const string&name);

    Workspace();
};

extern map<string, Workspace *> wsMap;

#endif //SHUFFLE_INCLUDE_WORKSPACE_H_
