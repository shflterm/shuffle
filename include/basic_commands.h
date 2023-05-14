#pragma clang diagnostic push
#pragma ide diagnostic ignored "performance-unnecessary-value-param"
//
// Created by winch on 5/6/2023.
//

#ifndef SHUFFLE_INCLUDE_BASIC_COMMANDS_H_
#define SHUFFLE_INCLUDE_BASIC_COMMANDS_H_

#include <vector>
#include <filesystem>

#include "console.h"
#include "i18n.h"
#include "workspace.h"

using namespace std;
namespace fs = std::filesystem;

void shflCmd(Workspace &workspace, const vector<string> &args) {
  if (args.size() < 2) {
    too_many_arguments();
    return;
  }

  if (args[1] == "register") {
    if (args.size() != 4) {
      too_many_arguments();
      return;
    }

    CommandData newData;
    newData.name = args[2];
    newData.value = args[3];
    newData.type = EXECUTABLE;
    addRegisteredCommand(newData);
  } else if (args[1] == "reload") {
    info("Reloading command...");
    loadCommands();
    success("Reloaded all commands!");
  }
}

void listCmd(Workspace &workspace, const vector<string> &args) {
  if (args.size() != 1) {
    too_many_arguments();
    return;
  }
  path dir = workspace.currentDirectory();

  for (const auto &entry : fs::directory_iterator(dir)) {
    string name = replace(absolute(entry.path()).string(), absolute(dir).string(), "");
    if (name[0] == '\\' || name[0] == '/') name = name.substr(1);
    if (entry.is_directory()) {
      info(BG_BLUE + name + "/");
    } else {
      info(name);
    }
  }
}

void langCmd(Workspace &workspace, const vector<string> &args) {
  if (args.size() != 2) {
    too_many_arguments();
    return;
  }

  loadLanguageFile(args[1]);
  info("lang.changed", {args[1]});

}

void exitCmd(Workspace &workspace, const vector<string> &args) {
  info("exit.bye");
  exit(0);
}

void clearCmd(Workspace &workspace, const vector<string> &args) {
  clear();
}

#endif //SHUFFLE_INCLUDE_BASIC_COMMANDS_H_

#pragma clang diagnostic pop