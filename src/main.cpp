#include <iostream>
#include <term.h>

#include "console.h"
#include "commandmgr.h"
#include "utils/utils.h"
#include "version.h"

using namespace std;

int main(int argc, char *argv[]) {
  loadCommands();
  ios::sync_with_stdio(false);
  cin.tie(nullptr);
  cout.tie(nullptr);

  if (argc > 1) {
    string arg = argv[1];
    if (arg == "--version" || arg == "-v") {
      term << "Shuffle " << SHUFFLE_VERSION.str() << newLine;
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

  initShflJson();
  
  term << eraseAll
       << "Welcome to" << color(FOREGROUND, Yellow) << " SHUFFLE " << SHUFFLE_VERSION.str() << resetColor << "!"
       << newLine
       << "(C) 2023 Shuffle Team. All Rights Reserved." << newLine << newLine
       << "Type 'help' to get help!" << newLine;

  currentWorkspace = new Workspace("main");
  while (true) {
    currentWorkspace->inputPrompt(false);
  }
}