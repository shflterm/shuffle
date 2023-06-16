#include <iostream>
#include <term.h>
#include <csignal>

#include "console.h"
#include "commandmgr.h"
#include "utils/utils.h"
#include "utils/crashreport.h"
#include "version.h"

using namespace std;

extern "C" void handleAborts(int sig) {
  term << newLine;
  error("Sorry. Something went wrong with Shuffle. Go to the URL below and report the problem.");
  error("https://github.com/shflterm/shuffle/issues/new?template=crash-report.yaml");
  term << newLine;
  CrashReport report = CrashReport()
      .setStackTrace(genStackTrace())
      .setSignalNumber(sig);
  error(report.make());
  report.save();
  exit(1);
}

int main(int argc, char *argv[]) {
//  ios::sync_with_stdio(false);
//  cin.tie(nullptr);
//  cout.tie(nullptr);

  signal(SIGSEGV, &handleAborts);
  signal(SIGABRT, &handleAborts);

  loadCommands();
  if (argc > 1) {
    string arg = argv[1];
    if (arg == "--version" || arg == "-v") {
      term << "Shuffle " << SHUFFLE_VERSION.str() << newLine;
      return 0;
    }

    loadCommands();
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
       << "(C) 2023 Shuffle Team. All Rights Reserved." << newLine << newLine;

  loadCommands();

  term << "Type 'help' to get help!" << newLine;

  currentWorkspace = new Workspace("main");
  while (true) {
    currentWorkspace->inputPrompt(true);
  }
}