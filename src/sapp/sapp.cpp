#include "sapp/sapp.h"

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
  if (type == NORMAL) {
#ifdef _WIN32
    HINSTANCE lib = LoadLibrary((DOT_SHUFFLE + "/apps/" + name + "/" + value).c_str());
    if (!lib) return;
    auto entrypoint = (entrypoint_t) GetProcAddress(lib, "entrypoint");

    if (entrypoint == nullptr) return;

    entrypoint(ws, args);
    ::FreeLibrary(lib);
#elif __linux__
    void *lib = dlopen((DOT_SHUFFLE + "/apps/" + name + "/" + value).c_str(), RTLD_LAZY);
    auto entrypoint = (entrypoint_t) dlsym(lib, "entrypoint");

    entrypoint(ws, args);

    dlclose(lib);
#endif
  } else { // SCRIPT
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
    string dir = lua_tostring(L, -1);

    ws.moveDirectory(absolute(path(dir)));
  }
}

void SAPPCommand::loadVersion2(Json::Value root, const string &name) {
  type = root["libpath"].isString() ? SCRIPT : NORMAL;
  if (type == NORMAL) {
    Json::Value libPath = root["libpath"];
#ifdef _WIN32
    Json::Value executable = libPath["windows"];
#elif __linux__
    Json::Value executable = libPath["linux"];
#elif __APPLE__
    Json::Value executable = libPath["macos"];
#endif

    value = executable.asString();
  } else { // SCRIPT
    string scriptPath = DOT_SHUFFLE + "/apps/" + name + "/" + root["libpath"].asString();
    L = luaL_newstate();
    luaL_openlibs(L);
    int err = luaL_loadfile(L, scriptPath.c_str());
    if (err) error("Error: " + string(lua_tostring(L, -1)));

    err = lua_pcall(L, 0, 0, 0);
    if (err) error("Error: " + string(lua_tostring(L, -1)));
  }
}

SAPPCommand::SAPPCommand(const string &name) : Command(name) {
  string runDotShfl = DOT_SHUFFLE + "/apps/" + name + "/run.shfl";

  Json::Value root;
  Json::Reader reader;
  reader.parse(readFile(runDotShfl), root, false);
  if (root["version"].asInt() == 2) {
    loadVersion2(root, name);
  } else {
    error("Error: Invalid version number in run.shfl");
  }
}