#include "lua.hpp"
#include <filesystem>
#include <fstream>
#include <vector>

#include "utils/utils.h"

using namespace std;

int lua_exists(lua_State *L) {
  path p = lua_getPath(L, luaL_checkstring(L, 1));

  lua_pushboolean(L, exists(p));
  return 1;
}

int lua_list(lua_State *L) {
  path p = lua_getPath(L, luaL_checkstring(L, 1));

  if (!exists(p) || !is_directory(p)) {
    luaL_error(L, "Invalid directory path");
    return 0;
  }

  std::vector<std::string> files;
  for (const auto &entry : directory_iterator(p)) {
    files.push_back(entry.path().filename().string());
  }

  pushStringArray(L, files);
  return 1;
}

int lua_isDir(lua_State *L) {
  path p = lua_getPath(L, luaL_checkstring(L, 1));
  lua_pushboolean(L, is_directory(p));
  return 1;
}

int lua_isFile(lua_State *L) {
  path p = lua_getPath(L, luaL_checkstring(L, 1));
  lua_pushboolean(L, is_regular_file(p));
  return 1;
}

int lua_mkDir(lua_State *L) {
  path p = lua_getPath(L, luaL_checkstring(L, 1));
  create_directory(p);
  return 1;
}

int lua_readFile(lua_State *L) {
  path p = lua_getPath(L, luaL_checkstring(L, 1));
  lua_pushstring(L, readFile(p.string()).c_str());
  return 1;
}

int lua_parentDir(lua_State *L) {
  path p = lua_getPath(L, luaL_checkstring(L, 1));
  if (p.string()[p.string().length() - 1] == '\\' || p.string()[p.string().length() - 1] == '/')
    p = p.parent_path();
  lua_pushstring(L, p.parent_path().string().c_str());
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

  lua_pushcfunction(L, lua_mkDir);
  lua_setfield(L, -2, "mkDir");

  lua_pushcfunction(L, lua_readFile);
  lua_setfield(L, -2, "readFile");

  lua_pushcfunction(L, lua_parentDir);
  lua_setfield(L, -2, "parentDir");

  lua_setglobal(L, "fs");
}