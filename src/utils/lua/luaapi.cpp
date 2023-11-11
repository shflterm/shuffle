#include "lua/luaapi.h"

#include "luafs.cpp"

#ifdef _WIN32

#include <Windows.h>

#elif __linux__

#include <dlfcn.h>

#endif

typedef void (*initialize_t)(lua_State *L);

void pushStringArray(lua_State *L, const vector<string> &strings) {
    lua_newtable(L);

    int index = 1;
    for (const auto &str: strings) {
        lua_pushstring(L, str.c_str());
        lua_rawseti(L, -2, index++);
    }
}

path lua_getPath(lua_State *L, const string &s) {
    path p = path(s);
    if (p.is_relative()) {
        lua_getglobal(L, "workspace");
        lua_getfield(L, -1, "dir");
        p = path(lua_tostring(L, -1)) / p;
    }

    return p;
}

void initLua(lua_State *L) {
    initFileSystem(L);

    Json::Value libs = getShflJson("libs");
    for (const auto &item: libs) {
        string name = item.asString();

#ifdef _WIN32
        HINSTANCE lib = LoadLibrary((DOT_SHUFFLE + "/libs/" + name + ".dll").c_str());
        if (!lib) continue;
        auto initialize = reinterpret_cast<initialize_t>(GetProcAddress(lib, "initialize"));

        if (initialize == nullptr) continue;

        initialize(L);
#elif __linux__
        void *lib = dlopen((DOT_SHUFFLE + "/libs/" + name + ".so").c_str(), RTLD_LAZY);
        auto initialize = (initialize_t) dlsym(lib, "initialize");

        initialize(L);
#endif
    }
}