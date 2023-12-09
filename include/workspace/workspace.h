//
// Created by winch on 5/11/2023.
//

#ifndef SHUFFLE_INCLUDE_WORKSPACE_H_
#define SHUFFLE_INCLUDE_WORKSPACE_H_

#include <string>
#include <filesystem>
#include <vector>
#include <map>
#include <memory>

class Workspace;

#include "cmd/job.h"

using std::string, std::vector, std::map, std::filesystem::path, std::filesystem::current_path, std::shared_ptr;

class Workspace {
    string name;
    string dir = current_path().string();
    vector<string> history;
    int historyIndex = 0;
    map<string, string> variables;

    [[nodiscard]] string prompt(bool fullPath = false) const;

public:
    path currentDirectory() const;

    void moveDirectory(const path& newDir);

    vector<string> getHistory();

    void addHistory(const string&s);

    map<string, string> getVariables();

    string historyUp();

    string historyDown();

    string processArgument(string argument);

    shared_ptr<job::Job> createJob(string &input);

    void inputPrompt();

    string getName();

    explicit Workspace(const string&name);

    Workspace();
};

extern map<string, Workspace *> wsMap;

#endif //SHUFFLE_INCLUDE_WORKSPACE_H_
