#include "sapp/sapp.h"
#include "json/value.h"
#include "json/reader.h"
#include "utils/utils.h"
#include "console.h"

#include <vector>
#include <string>

using namespace std;

void SAPPCommand::run(const vector<std::string> &args) const {
  system((DOT_SHUFFLE + "/apps/" + name + "/" + value).c_str());
}

SAPPCommand::SAPPCommand(const string &name) : Command(name) {
  string runDotShfl = DOT_SHUFFLE + "/apps/" + name + "/run.shfl";

  Json::Value root;
  Json::Reader reader;
  reader.parse(readFile(runDotShfl), root, false);

#ifdef _WIN32
  Json::Value executable = root["executable-WINDOWS"];
#elif __linux__
  Json::Value executable = root["executable-LINUX"];
#elif __APPLE__
  Json::Value executable = root["executable-MACOS"];
#endif

  value = executable.asString();
}
