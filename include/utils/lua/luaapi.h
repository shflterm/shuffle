//
// Created by winch on 5/21/2023.
//

#ifndef SHFL_INCLUDE_UTILS_LUAAPI_H_
#define SHFL_INCLUDE_UTILS_LUAAPI_H_

#include "lua.hpp"
#include <filesystem>
#include <fstream>
#include <vector>

using std::vector, std::string, std::filesystem::path;

void pushStringArray(lua_State *L, const vector <string> &strings);

path lua_getPath(lua_State *L, const string &s);

void initLua(lua_State *L);

#endif //SHFL_INCLUDE_UTILS_LUAAPI_H_
