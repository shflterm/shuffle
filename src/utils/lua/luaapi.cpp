#include "utils/lua/luaapi.h"

#include "luafs.cpp"

void initLua(lua_State *L) {
  initFileSystem(L);
}