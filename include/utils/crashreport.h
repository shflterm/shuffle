//
// Created by winch on 6/8/2023.
//

#ifndef SHFL_INCLUDE_UTILS_CRASHREPORT_H_
#define SHFL_INCLUDE_UTILS_CRASHREPORT_H_

#include <string>

#include "workspace.h"

using namespace std;

string genStackTrace();

class CrashReport {
  vector<Workspace *> workspaces;
  string stackTrace;
  int sig;

 public:
  CrashReport setStackTrace(const string &stackTrace_);
  CrashReport setSignalNumber(int sig);

  string make();
  void save();
};

#endif //SHFL_INCLUDE_UTILS_CRASHREPORT_H_
