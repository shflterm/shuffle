#include "commandmgr.h"

#include <iostream>
#include <regex>
#include <utility>
#include <json/json.h>

#include "executor.h"
#include "console.h"
#include "suggestion.h"
#include "basic_commands.h"
#include "utils/utils.h"
#include "sapp/sapp.h"

#define COMMANDS_JSON (DOT_SHUFFLE + "/commands.json")

using namespace std;

vector<unique_ptr<Command>> commands;

void loadDefaultCommands() {
  commands.push_back(make_unique<Command>(Command("help", helpCmd)));
  commands.push_back(make_unique<Command>(Command("shfl", shflCmd)));
  commands.push_back(make_unique<Command>(Command("cd", cdCmd)));
  commands.push_back(make_unique<Command>(Command("list", listCmd)));
  commands.push_back(make_unique<Command>(Command("lang", langCmd, {Command("en_us", emptyExecutor),
                                                                    Command("ko_kr", emptyExecutor)})));
  commands.push_back(make_unique<Command>(Command("exit", exitCmd)));
  commands.push_back(make_unique<Command>(Command("clear", clearCmd)));
}

void loadCommand(const CommandData &data) {
  if (data.type == SAPP) commands.push_back(make_unique<SAPPCommand>(SAPPCommand(data.name)));
  else if (data.type == EXECUTABLE)
    commands.push_back(make_unique<Command>(Command(data.name,
                                                    EXECUTABLE,
                                                    data.value)));
}

void inputCommand(bool enableSuggestion) {
  cout << absolute(dir).string() << "> ";
  cout.flush();

  string input;
  if (enableSuggestion) {
    char c;
    while (true) {
      c = readChar();

      if (c == '\b') {
        if (!input.empty()) {
          gotoxy(wherex() - 1, wherey());
          cout << ' ';
          gotoxy(wherex() - 1, wherey());
          input = input.substr(0, input.length() - 1);
        }
      } else if (c == '\n' || c == '\r') {
        break;
      } else if (c == '\t') {
        vector<string> args = split(input, regex(R"(\s+)"));
        string suggestion;
        if (args.size() == 1) {
          suggestion = findSuggestion(args[args.size() - 1], commands);
        } else {
          Command command = findCommand(args[0]);
          if (command.getName().empty() && command.getValue().empty()) continue;

          suggestion = findSuggestion(args[args.size() - 1], command.getChildren());
        }
        if (suggestion.empty()) continue;

        input += suggestion;
        cout << "\033[0m" << suggestion;
      } else {
        cout << "\033[0m" << c;
        input += c;
      }

      // suggestion
      vector<string> args = split(input, regex(R"(\s+)"));
      string suggestion;
      if (args.size() == 1) {
        suggestion = findSuggestion(args[args.size() - 1], commands);
      } else {
        Command command = findCommand(args[0]);
        if (command.getName().empty() && command.getValue().empty()) continue;

        suggestion = findSuggestion(args[args.size() - 1], command.getChildren());
      }
      if (suggestion.empty()) continue;

      cout << "\033[90m" << suggestion;
      gotoxy(wherex() - (int) suggestion.size(), wherey());
    }
    white();
  } else {
    getline(cin, input);
  }

  if (!input.empty()) execute(input);
}

vector<CommandData> getRegisteredCommands() {
  vector<CommandData> res;

  Json::Value root;
  Json::Reader reader;
  reader.parse(readFile(COMMANDS_JSON), root, false);

  Json::Value commandList = root["commands"];
  for (auto command : commandList) {
    CommandData data;
    data.name = command["name"].asString();
    if (command["type"].asString() == "SAPP") {
      data.type = SAPP;
    } else if (command["type"].asString() == "EXECUTABLE") {
      data.type = EXECUTABLE;
      data.value = command["value"].asString();
    }
    res.push_back(data);
  }

  return res;
}

void addRegisteredCommand(const CommandData &data) {
  vector<CommandData> res;

  Json::Value root;
  Json::Reader reader;
  reader.parse(readFile(COMMANDS_JSON), root, false);

  Json::Value commandList = root["commands"];

  Json::Value value(Json::objectValue);
  value["name"] = data.name;
  value["value"] = data.value;
  if (data.type == CUSTOM) value["type"] = "CUSTOM";
  else if (data.type == SAPP) value["type"] = "SAPP";
  else if (data.type == EXECUTABLE) value["type"] = "EXECUTABLE";
  commandList.append(value);

  root["commands"] = commandList;

  writeFile(COMMANDS_JSON, root.toStyledString());
}

void loadCommands() {
  commands.clear();

  loadDefaultCommands();

  for (const CommandData &command : getRegisteredCommands()) {
    loadCommand(command);
  }
}

Command findCommand(const string& name) {
  for (const auto &item : commands) {
    Command &command = *item;
    if (command.getName() == name) return command;
  }
  return {"", emptyExecutor};
}

const string &Command::getName() const {
  return name;
}

ExecutableType Command::getType() const {
  return type;
}

const string &Command::getValue() const {
  return value;
}

vector<unique_ptr<Command>> Command::getChildren() const {
  vector<unique_ptr<Command>> newChildren;
  newChildren.reserve(children.size());
  for (const Command &item : children) {
    newChildren.push_back(make_unique<Command>(item));
  }
  return newChildren;
}

void Command::run(const vector<std::string> &args) const {
  executor(args);
}

Command::Command(string name, ExecutableType type, string value, vector<Command> children)
    : name(std::move(name)),
      type(type),
      value(std::move(value)),
      executor(emptyExecutor),
      children(std::move(children)) {}

Command::Command(string name, CommandExecutor executor, vector<Command> children)
    : name(std::move(name)), type(CUSTOM), executor(executor), children(std::move(children)) {}

Command::Command(string name, ExecutableType type, string value)
    : name(std::move(name)), type(type), value(std::move(value)), executor(emptyExecutor) {}

Command::Command(string name, CommandExecutor executor) : name(std::move(name)), type(CUSTOM), executor(executor) {}

Command::Command(string name) : name(std::move(name)), type(SAPP) {}