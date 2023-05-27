//
// Created by winch on 5/11/2023.
//

#ifndef SHUFFLE_INCLUDE_WORKSPACE_H_
#define SHUFFLE_INCLUDE_WORKSPACE_H_

#include <string>
#include <filesystem>
#include <vector>

using namespace std;
using namespace std::filesystem;

class Workspace {
#ifdef _WIN32
  path dir = current_path();
#elif __linux__ || __APPLE__
  path dir = current_path();
#endif
  vector<string> history;
  int historyIndex = 0;
  string name;

  string prompt();

 public:
  path currentDirectory();
  void moveDirectory(path newDir);
  void addHistory(const string &s);
  string historyUp();
  string historyDown();

  void execute(const string &input);
  void inputPrompt(bool enableSuggestion);

  explicit Workspace(const string& name);
  Workspace();
};

#endif //SHUFFLE_INCLUDE_WORKSPACE_H_
