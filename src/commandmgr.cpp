#include "commandmgr.h"

#include <json/json.h>
#include <memory>
#include <utility>

#include "console.h"
#include "utils/utils.h"
#include "basic_commands.h"
#include "sapp/sapp.h"
#include "workspace.h"

#define COMMANDS_JSON (DOT_SHUFFLE + "/commands.json")

using namespace std;

vector<unique_ptr<Command>> commands;

void loadDefaultCommands() {
//  commands.push_back(make_unique<Command>(Command("help", "Print a list of commands", helpCmd)));
  commands.push_back(make_unique<Command>(Command("shfl", "Shuffle Command.", shflCmd)));
//  commands.push_back(make_unique<Command>(Command("cd", "Change directory", cdCmd)));
  commands.push_back(make_unique<Command>(Command("list", "Print list file of current directory", listCmd)));
  commands.push_back(make_unique<Command>(Command("lang", "Change language", langCmd)));
  commands.push_back(make_unique<Command>(Command("exit", "Shut down Shuffle", exitCmd)));
  commands.push_back(make_unique<Command>(Command("clear", "Clear console", clearCmd)));
}

void loadCommand(const CommandData &data) {
  if (data.type == SAPP) commands.push_back(make_unique<SAPPCommand>(SAPPCommand(data.name)));
  else if (data.type == EXECUTABLE)
    commands.push_back(make_unique<Command>(Command(data.name,
                                                    data.description,
                                                    EXECUTABLE,
                                                    data.value)));
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

const string &Command::getName() const {
  return name;
}

ExecutableType Command::getType() const {
  return type;
}

const string &Command::getValue() const {
  return value;
}

const string &Command::getDescription() const {
  return description;
}

void Command::run(Workspace ws, const vector<std::string> &args) const {
  executor(std::move(ws), args);
}

Command::Command(string name, string description, ExecutableType type, string value)
    : name(std::move(name)),
      description(std::move(description)),
      type(type),
      value(std::move(value)),
      executor(emptyExecutor) {}

Command::Command(string name, string description, CommandExecutor executor)
    : name(std::move(name)), description(std::move(description)), type(CUSTOM), executor(executor) {}

Command::Command(string name, string description)
    : name(std::move(name)), description(std::move(description)), type(SAPP) {}

Command::Command(string name) : name(std::move(name)), description("-"), type(SAPP) {}
