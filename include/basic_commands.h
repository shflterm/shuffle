//
// Created by winch on 5/6/2023.
//

#ifndef SHUFFLE_INCLUDE_BASIC_COMMANDS_H_
#define SHUFFLE_INCLUDE_BASIC_COMMANDS_H_

#include <vector>
#include <filesystem>

#include "console.h"
#include "i18n.h"

using namespace std;
namespace fs = std::filesystem;

void helpCmd(const vector<string> &args) {
  if (args.size() == 1) {
    vector<string> helps;
    for (const auto &item : commands) {
      Command &command = *item;
      string text = command.getName() + ": " + command.getDescription();
      helps.push_back(text);
    }

    info("Only some commands are shown:");
    info("\033[90m(Type shfl cmds to see all commands.)");
    for (const auto &item : helps) info(item);
  } else if (args.size() == 2) {
    info("system.wip"); // TODO
  } else {
    too_many_arguments();
    return;
  }
}

void shflCmd(const vector<string> &args) {
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

void cdCmd(const vector<string> &args) {
  if (args.size() != 2) {
    too_many_arguments();
    return;
  }

  dir.append(args[1]);

  if (!is_directory(dir) || !exists(dir)) {
    error("cd.directory_not_found", {dir.string()});
    dir = dir.parent_path();
  }
}

void listCmd(const vector<string> &args) {
  if (args.size() != 1) {
    too_many_arguments();
    return;
  }

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

void langCmd(const vector<string> &args) {
  if (args.size() != 2) {
    too_many_arguments();
    return;
  }

  loadLanguageFile(args[1]);
  info("lang.changed", {args[1]});

}

void exitCmd(const vector<string> &args) {
  info("exit.bye");
  exit(0);
}

void clearCmd(const vector<string> &args) {
  clear();
}

#endif //SHUFFLE_INCLUDE_BASIC_COMMANDS_H_
