//
// Created by winch on 5/21/2023.
//

#ifndef SHFL_INCLUDE_UTILS_LUAAPI_H_
#define SHFL_INCLUDE_UTILS_LUAAPI_H_

#include "lua.hpp"
#include <filesystem>
#include <fstream>
#include <vector>

using namespace std;
using namespace std::filesystem;

void pushStringArray(lua_State *L, const vector<string> &strings);
void initLua(lua_State *L);

#endif //SHFL_INCLUDE_UTILS_LUAAPI_H_
