#include <iostream>

#include "console.h"
#include "i18n.h"
#include "commandmgr.h"

#include "version.h"

using namespace std;

int main() {
  cout << "Loading Shuffle...\n";
  loadLanguageFile("en_us");
  loadCommands();
  ios::sync_with_stdio(false); cin.tie(nullptr); cout.tie(nullptr);

  clear();

  info("Welcome to " + FG_YELLOW + "SHUFFLE " + SHUFFLE_VERSION + RESET);
  info("(C) 2023 Shuffle Team. All Rights Reserved.");
  white();
  info("system.help");

  Workspace workspace;
  while (true) {
    workspace.inputPrompt(true);
  }
}