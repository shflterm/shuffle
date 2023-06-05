#include "commandmgr.h"

#include <iostream>
#include <json/json.h>
#include <memory>
#include <utility>

#include "console.h"
#include "utils/utils.h"
#include "basic_commands.h"
#include "sapp/sapp.h"

using namespace std;

vector<unique_ptr<Command>> commands;

void loadDefaultCommands() {
  commands.push_back(make_unique<Command>(Command(
      "shfl", "Shuffle Command",
      {
          Command("reload", "Reload all commands"),
          Command("apps", "Manage SAPP",
                  {Command("add", "Install SAPP"),
                   Command("remove", "Delete SAPP")}),
          Command("update", "Update Shuffle"),
      })));
}

void loadCommand(const CommandData &data) {
  commands.push_back(make_unique<SAPPCommand>(SAPPCommand(data.name)));
}

vector<CommandData> getRegisteredCommands() {
  vector<CommandData> res;

  Json::Value commandList = getShflJson("commands");
  for (auto command : commandList) {
    CommandData data;
    data.name = command["name"].asString();
    res.push_back(data);
  }

  return res;
}

void addRegisteredCommand(const CommandData &data) {
  vector<CommandData> res;

  Json::Value commandList = getShflJson("commands");

  Json::Value value(Json::objectValue);
  value["name"] = data.name;
  value["value"] = data.value;
  commandList.append(value);

  setShflJson("commands", commandList);
}

void loadCommands() {
  commands.clear();

  loadDefaultCommands();

  for (const CommandData &command : getRegisteredCommands()) {
    loadCommand(command);
  }
}

Command *findCommand(const string &name, const vector<unique_ptr<Command>> &DICTIONARY) {
  for (auto &item : DICTIONARY) {
    if (item->getName() == name) {
      return new Command(*item.get());
    }
  }
  return nullptr;
}

Command *findCommand(const string &name) {
  return findCommand(name, commands);
}

const string &Command::getName() const {
  return name;
}

const string &Command::getDescription() const {
  return description;
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

void Command::addChild(const Command &command) {
  children.push_back(command);
}

void Command::run(Workspace &ws, const vector<std::string> &args) const { }

Command::Command(string name, string description, vector<Command> children)
    : name(std::move(name)),
      description(std::move(description)),
      children(std::move(children)) {}

Command::Command(string name, string description)
    : name(std::move(name)),
      description(std::move(description)) {}

Command::Command(string name, vector<Command> children)
    : name(std::move(name)),
      description("-"),
      children(std::move(children)) {}

Command::Command(string name)
    : name(std::move(name)),
      description("-") {}

OptionSubCommand::OptionSubCommand(const string &name, string description)
    : Command("option." + name,
              std::move(description)) {}
