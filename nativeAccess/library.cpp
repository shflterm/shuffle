#include <string>
#include <vector>
#include "lua.hpp"

#include "commandmgr.h"
#include "utils/lua/luaapi.h"

using namespace std;

int lua_commands(lua_State *L) {
  vector<string> commandNames;
  commandNames.reserve(commands.size());
  for (const auto &entry : commands) {
    commandNames.push_back(entry->getName());
  }

  pushStringArray(L, commandNames);
  return 1;
}

extern "C" __declspec(dllexport) void initialize(lua_State *L) {
  lua_newtable(L);

  lua_pushcfunction(L, lua_commands);
  lua_setfield(L, -2, "commands");

  lua_setglobal(L, "shuffle");
}
