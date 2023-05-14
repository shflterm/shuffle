//
// Created by winch on 5/11/2023.
//

#ifndef SHUFFLE_INCLUDE_WORKSPACE_H_
#define SHUFFLE_INCLUDE_WORKSPACE_H_

#include <string>
#include <filesystem>

using namespace std;
using namespace std::filesystem;

class Workspace {
#ifdef _WIN32
  path dir = current_path();
#elif __linux__ || __APPLE__
  path dir = current_path();
#endif

 public:
  path currentDirectory();
  void moveDirectory(path newDir);

  void execute(const string &input);
  void inputCommand(bool enableSuggestion);
};

#endif //SHUFFLE_INCLUDE_WORKSPACE_H_
