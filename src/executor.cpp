#include "executor.h"

#include <string>
#include <utility>
#include <vector>
#include <filesystem>
#include <sys/stat.h>
#include <json/json.h>
#include <memory>

#include "console.h"
#include "utils/utils.h"
#include "i18n.h"
#include "commandmgr.h"
#include "sapp/sapp.h"

using namespace std;
using std::filesystem::current_path;

#ifdef _WIN32
path dir = current_path();
#elif __linux__
path dir = current_path();
#endif

void execute(const string &input) {
  vector<string> args = split(input, regex(R"(\s+)"));
  if (args.empty()) return;

  bool isCommandFounded = false;
  for (const auto &item : commands) {
    Command &command = *item;
    if (command.getName() != args[0]) continue;
    isCommandFounded = true;

    if (command.getType() == CUSTOM) {
      command.run(args);
    } else if (command.getType() == SAPP) {
      dynamic_cast<SAPPCommand &>(command).run(args);
    } else if (command.getType() == EXECUTABLE) {
      string cmd = command.getValue();
      for (int i = 1; i < args.size(); ++i) {
        cmd.append(" ").append(args[i]);
      }
      system(cmd.c_str());
    }
    break;
  }

  if (!isCommandFounded) {
    error("system.command_not_found", {args[0]});
    pair<int, Command> similarWord = {1000000000, {"", CUSTOM, ""}};
    for (const auto &item : commands) {
      Command &command = *item;
      int dist = levenshteinDist(args[0], command.getName());
      if (dist < similarWord.first) similarWord = {dist, command};
    }

    if (similarWord.first > 1) warning("system.check_command");
    else warning("system.similar_command", {similarWord.second.getName()});
  }
}
