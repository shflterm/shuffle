#include "commandmgr.h"

#include <iostream>
#include <json/json.h>

#include "executor.h"
#include "console.h"
#include "suggestion.h"
#include "utils.h"

#define COMMANDS_JSON string(getenv("APPDATA")) + "/.shuffle/commands.json"

using namespace std;

vector<Command> commands;

void loadDefaultCommands() {
  commands.emplace_back("exit", CUSTOM);
  commands.emplace_back("cd", CUSTOM);
  commands.emplace_back("list", CUSTOM);
  commands.emplace_back("lang", CUSTOM);
  commands.emplace_back("help", CUSTOM);
  commands.emplace_back("shfl", CUSTOM);
//  commands.emplace_back("clear", EXECUTE_PROGRAM, "clear");
}

void loadCommand(const CommandData &data) {
  commands.emplace_back(data.name, data.type, data.execute);
}

void inputCommand() {
  cout << path << "> ";
  cout.flush();

  string input;
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
      string suggestion = findSuggestion(input, commands);
      if (suggestion.empty()) continue;

      input += suggestion;
      cout << "\033[0m" << suggestion;
    } else {
      cout << "\033[0m" << c;
      input += c;
    }

    string suggestion = findSuggestion(input, commands);
    if (suggestion.empty()) continue;

    cout << "\033[90m" << suggestion;
    gotoxy(wherex() - (int) suggestion.size(), wherey());
  }
  white();
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
    data.execute =
        replace(command["execute"].asString(), "{SHUFFLE_APPS}", string(getenv("APPDATA")) + "/.shuffle/apps");
    if (command["type"].asString() == "SAPP") data.type = SAPP;
    else data.type = EXECUTABLE;
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
  value["execute"] = data.execute;
  value["type"] = data.type;
  commandList.append(value);

  root["commands"] = commandList;

  writeFile(COMMANDS_JSON, root.toStyledString());
}
