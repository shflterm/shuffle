#include <vector>
#include <iostream>

#include "executor.h"
#include "console.h"
#include "i18n.h"
#include "commandmgr.h"

using namespace std;

int main() {
  cout << "Loading Shuffle...";
  loadLanguageFile("en_us");
  loadCommands();

  clear();

  info("system.welcome", {"v1.0"});
  info("system.copyrights");
  white();
  info("system.help");

  while (true) {
    inputCommand(false);
  }
}