#include "sapp/sapp.h"

#include <vector>
#include <string>
#include <json/json.h>

#include "utils/utils.h"

#ifdef _WIN32
#define NOMINMAX 1
#define byte win_byte_override
#define WIN32_LEAN_AND_MEAN
#define _HAS_STD_BYTE 0

#include <Windows.h>
typedef void (*entrypoint_t)(Workspace &workspace, const vector<std::string> &args);
#elif __linux__
#include <dlfcn.h>
typedef void (*entrypoint_t)(Workspace &workspace, const vector<std::string> &args);
#endif

void SAPPCommand::run(Workspace &ws, const vector<std::string> &args) const {
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
}

void addChildren(const Json::Value& json, Command *command) {
  for (Json::Value item : json) {
    if (item["type"] == "option") {
      Command child = OptionSubCommand(item["name"].asString(), item["description"].asString());
      command->addChild(child);
    } else if (item["type"] == "subcommand") {
      Command child = Command(item["name"].asString(), item["description"].asString());
      if (item.isMember("children")) addChildren(item["children"], &child);

      command->addChild(child);
    }
  }
}

SAPPCommand::SAPPCommand(const string &name) : Command(name) {
  string runDotShfl = DOT_SHUFFLE + "/apps/" + name + "/run.shfl";

  Json::Value root;
  Json::Reader reader;
  reader.parse(readFile(runDotShfl), root, false);

#ifdef _WIN32
  Json::Value executable = root["windows"];
#elif __linux__
  Json::Value executable = root["linux"];
#elif __APPLE__
  Json::Value executable = root["macos"];
#endif

  value = executable.asString();

  //Read help.shfl
  string helpDotShfl = DOT_SHUFFLE + "/apps/" + name + "/help.shfl";
  Json::Value helpRoot;
  Json::Reader helpReader;
  helpReader.parse(readFile(helpDotShfl), helpRoot, false);
  Json::Value children = helpRoot["children"];
  addChildren(children, this);
}