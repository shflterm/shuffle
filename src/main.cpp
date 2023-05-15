#include <iostream>

#include "console.h"
#include "i18n.h"
#include "commandmgr.h"

#include "version.h"

using namespace std;

int main() {
  cout << "Loading Shuffle...";
  loadLanguageFile("en_us");
  loadCommands();

  clear();

  info("Welcome to " + FG_YELLOW + "SHUFFLE " + SHUFFLE_VERSION + RESET);
  info("(C) 2023 Shuffle Team. All Rights Reserved.");
  white();
  info("system.help");

  Workspace workspace;
  while (true) {
    workspace.inputPrompt(false);
  }
}