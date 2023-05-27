#include <iostream>

#include "console.h"
#include "commandmgr.h"

#include "version.h"

using namespace std;

int main(int argc, char *argv[]) {
  loadCommands();
  ios::sync_with_stdio(false); cin.tie(nullptr); cout.tie(nullptr);

  if (argc > 1) {
    string arg = argv[1];
    if (arg == "--version" || arg == "-v") {
      cout << "Shuffle " << SHUFFLE_VERSION.to_string() << endl;
      return 0;
    }

    Workspace workspace;
    string cmd;
    for (int i = 1; i < argc; ++i) {
      cmd += argv[i];
      cmd += " ";
    }
    workspace.execute(cmd);
    return 0;
  }

  clear();
  info("Welcome to " + FG_YELLOW + "SHUFFLE " + SHUFFLE_VERSION.to_string() + RESET);
  info("(C) 2023 Shuffle Team. All Rights Reserved.");
  white();
  info("Type 'help' to get help!");

  while (true) {
    currentWorkspace.inputPrompt(true);
  }
}