#include "executor.h"

#include <string>
#include <utility>
#include <vector>
#include <filesystem>
#include <sys/stat.h>
#include <json/json.h>

#include "console.h"
#include "utils.h"
#include "i18n.h"
#include "commandmgr.h"

using namespace std;
using std::filesystem::current_path;
namespace fs = std::filesystem;

#ifdef _WIN32
string path = current_path().string();
#elif __linux__
string path = current_path();
#endif

void execute(const string &input) {
  vector<string> args = split(input, regex(R"(\s+)"));
  if (args.empty()) return;

  bool isCommandFounded = false;
  for (const Command &command : commands) {
    if (command.name != args[0]) continue;
    isCommandFounded = true;

    if (command.type == CUSTOM) {
      if (args[0] == "shfl") {
        if (args.size() < 2) {
          too_many_arguments();
          return;
        }

        if (args[1] == "register") {
          if (args.size() != 4) {
            too_many_arguments();
            return;
          }

          CommandData newData;
          newData.name = args[2];
          newData.execute = args[3];
          newData.type = EXECUTABLE;
          addRegisteredCommand(newData);
        }
      } else if (args[0] == "exit") {
        info("exit.bye");
        exit(0);
      } else if (args[0] == "help") {
        info("system.wip");
      } else if (args[0] == "cd") {
        if (args.size() != 2) {
          too_many_arguments();
          return;
        }

        string newPath = args[1][0] == '/' || args[1][0] == '\\' ? args[1] : path.append("/").append(args[1]);

        struct stat sb{};
        if (stat(newPath.c_str(), &sb) == 0)
          path = newPath;
        else error("cd.directory_not_found", {newPath});
      } else if (args[0] == "list") {
        if (args.size() != 1) {
          too_many_arguments();
          return;
        }

        for (const auto &entry : fs::directory_iterator(path)) {
#ifdef _WIN32
          print(INFO, replace(entry.path().string(), path, ""));
#elif __linux__
          print(INFO, replace(entry.path(), path, ""));
#endif
        }
      } else if (args[0] == "lang") {
        if (args.size() != 2) {
          too_many_arguments();
          return;
        }

        loadLanguageFile(args[1]);
        info("lang.changed", {args[1]});
      }
    } else if (command.type == SAPP) {
      string runDotShfl = command.value + "/run.shfl";

      Json::Value root;
      Json::Reader reader;
      reader.parse(readFile(runDotShfl), root, false);

#ifdef _WIN32
      Json::Value executable = root["executable-WINDOWS"];
#elif __linux__
      Json::Value executable = root["executable-LINUX"];
#endif
      system((command.value + "/" + executable.asString()).c_str());
    } else if (command.type == EXECUTABLE) {
      string cmd = command.value;
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
    for (const Command &command : commands) {
      int dist = levenshteinDist(args[0], command.name);
      if (dist < similarWord.first) similarWord = {dist, command};
    }

    if (similarWord.first > 1) warning("system.check_command");
    else warning("system.similar_command", {similarWord.second.name});
  }
}
