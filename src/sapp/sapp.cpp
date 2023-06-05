#include "sapp/sapp.h"

#include <vector>
#include <string>
#include <json/json.h>

#include "utils/utils.h"
#include "console.h"
#include "utils/lua/luaapi.h"

#ifdef _WIN32
#define NOMINMAX 1
#define byte win_byte_override
#define WIN32_LEAN_AND_MEAN
#define _HAS_STD_BYTE 0

#include <Windows.h>
#elif __linux__
#include <dlfcn.h>
#endif

typedef void (*entrypoint_t)(Workspace &workspace, const vector<string> &args);
typedef vector<string> (*suggest_t)(Workspace &workspace, const string &suggestId);

void SAPPCommand::run(Workspace &ws, const vector<std::string> &args) const {
  // Create workspace table
  lua_newtable(L);
  {
    lua_pushstring(L, ws.currentDirectory().string().c_str());
    lua_setfield(L, -2, "dir");
  } // ws.dir

  lua_setglobal(L, "workspace");

  // Create args list
  lua_newtable(L);
  for (int i = 0; i < args.size(); ++i) {
    lua_pushstring(L, args[i].c_str());
    lua_rawseti(L, -2, i + 1);
  }
  lua_setglobal(L, "args");

  lua_getglobal(L, "entrypoint");
  if (lua_type(L, -1) != LUA_TFUNCTION) {
    error("Error: 'entrypoint' is not a function!");
    return;
  }
  int err = lua_pcall(L, 0, 0, 0);
  if (err) error("Error: " + string(lua_tostring(L, -1)));

  // Update workspace
  lua_getglobal(L, "workspace");
  lua_getfield(L, -1, "dir");

  path newDir = lua_tostring(L, -1);
  if (newDir.is_relative()) newDir = ws.currentDirectory() / newDir;
  ws.moveDirectory(newDir);
}

void addChildren(const Json::Value &json, Command *command) {
  for (Json::Value item : json) {
    if (item["type"] == "option") {
      OptionSubCommand child = OptionSubCommand(item["name"].asString(), item["description"].asString());
      command->addChild(child);
    } else if (item["type"] == "subcommand") {
      Command child = Command(item["name"].asString(), item["description"].asString());
      if (item.isMember("children")) addChildren(item["children"], &child);

      command->addChild(child);
    }
  }
}

vector<string> SAPPCommand::makeDynamicSuggestion(Workspace &ws, const string &suggestId) {
  // Create workspace table
  lua_newtable(L);
  {
    lua_pushstring(L, ws.currentDirectory().string().c_str());
    lua_setfield(L, -2, "dir");
  } // ws.dir

  lua_setglobal(L, "workspace");

  // Create args list
  lua_pushstring(L, suggestId.c_str());
  lua_setglobal(L, "suggestId");

  lua_getglobal(L, "suggest");
  if (lua_type(L, -1) != LUA_TFUNCTION) {
    error("Error: 'suggest' is not a function!");
    return {};
  }
  int err = lua_pcall(L, 0, 1, 0);
  if (err) {
    error("\nAn error occurred while generating suggestion.\n\n" + string(lua_tostring(L, -1)));
  }

  lua_Unsigned tableSize = lua_rawlen(L, -1);

  if (lua_istable(L, -1)) {
    vector<string> resultArray;
    for (int i = 1; i <= tableSize; ++i) {
      lua_rawgeti(L, -1, i);

      if (lua_isstring(L, -1)) {
        resultArray.emplace_back(lua_tostring(L, -1));
      }

      lua_pop(L, 1);
    }
    return resultArray;
  }
  return {};
}

void SAPPCommand::loadVersion1(Json::Value root, const string &name) {
  string appPath = DOT_SHUFFLE + "/apps/" + name + ".app/";

  value = appPath + "/lib/entrypoint.lua";
  L = luaL_newstate();

  luaL_openlibs(L);
  initLua(L);
  int err = luaL_loadfile(L, value.c_str());
  if (err) error("Error: " + string(lua_tostring(L, -1)));

  err = lua_pcall(L, 0, 0, 0);
  if (err) error("Error: " + string(lua_tostring(L, -1)));

  //Read help.shfl
  string helpDotShfl = appPath + "/help.shfl";
  Json::Value helpRoot;
  Json::Reader helpReader;
  helpReader.parse(readFile(helpDotShfl), helpRoot, false);
  Json::Value children = helpRoot["children"];
  addChildren(children, this);
}

SAPPCommand::SAPPCommand(const string &name) : Command(name) {
  string runDotShfl = DOT_SHUFFLE + "/apps/" + name + ".app/run.shfl";

  Json::Value root;
  Json::Reader reader;
  reader.parse(readFile(runDotShfl), root, false);
  if (root["version"].asInt() == 1) {
    loadVersion1(root, name);
  } else {
    warning("Error: Invalid version number in " + name + ". Load with version 1.");
    loadVersion1(root, name);
  }
}