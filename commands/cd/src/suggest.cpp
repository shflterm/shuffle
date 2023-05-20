#include "libsapp.h"
#include "console.h"
#include "utils/utils.h"

namespace fs = std::filesystem;

SAPPSUGGEST suggest(Workspace &workspace, const string &suggestId) {
  vector<string> res;
  if (suggestId == "path") {
    path dir = workspace.currentDirectory();
    for (const auto &entry : fs::directory_iterator(dir)) {
      if (!entry.is_directory()) continue;

      string name = replace(absolute(entry.path()).string(), absolute(dir).string(), "");
      if (name[0] == '\\' || name[0] == '/') name = name.substr(1);

      res.push_back(name);
    }
  }
  return res;
}