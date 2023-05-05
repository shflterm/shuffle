#include <vector>

#include "executor.h"
#include "console.h"
#include "i18n.h"
#include "commandmgr.h"

using namespace std;

int main() {
  print(INFO, "Loading Shuffle...");
  loadLanguageFile("en_us");
  loadDefaultCommands();

  vector<CommandData> commands = getRegisteredCommands();
  for (const CommandData &command : commands) {
    loadCommand(command);
  }

//  clear();

  info("system.welcome", {"v1.0"});
  info("system.copyrights");
  white();
  info("system.help");

  while (true) {
    inputCommand();
  }
}