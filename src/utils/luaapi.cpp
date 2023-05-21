#include "utils/luaapi.h"

void pushStringArray(lua_State *L, const std::vector<std::string> &strings) {
  lua_newtable(L);

  int index = 1;
  for (const auto &str : strings) {
    lua_pushstring(L, str.c_str());
    lua_rawseti(L, -2, index++);
  }
}

int lua_exists(lua_State *L) {
  path p = path(luaL_checkstring(L, 1));

  lua_pushboolean(L, exists(p));
  return 1;
}

int lua_list(lua_State *L) {
  path directoryPath(luaL_checkstring(L, 1));
  if (!exists(directoryPath) || !is_directory(directoryPath)) {
    luaL_error(L, "Invalid directory path");
    return 0;
  }

  std::vector<std::string> files;
  for (const auto &entry : directory_iterator(directoryPath)) {
    files.push_back(entry.path().filename().string());
  }

  pushStringArray(L, files);
  return 1;
}

int lua_isDir(lua_State *L) {
  path directoryPath(luaL_checkstring(L, 1));
  lua_pushboolean(L, is_directory(directoryPath));
  return 1;
}

int lua_isFile(lua_State *L) {
  path directoryPath(luaL_checkstring(L, 1));
  lua_pushboolean(L, is_regular_file(directoryPath));
  return 1;
}

void initFileSystem(lua_State *L) {
  lua_newtable(L);

  lua_pushcfunction(L, lua_exists);
  lua_setfield(L, -2, "exists");

  lua_pushcfunction(L, lua_list);
  lua_setfield(L, -2, "list");

  lua_pushcfunction(L, lua_isDir);
  lua_setfield(L, -2, "isDir");

  lua_pushcfunction(L, lua_isFile);
  lua_setfield(L, -2, "isFile");

  lua_setglobal(L, "fs");
}

void initLua(lua_State *L) {
  initFileSystem(L);
}