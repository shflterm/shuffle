//
// Created by winch on 6/8/2023.
//

#ifndef SHFL_INCLUDE_UTILS_CRASHREPORT_H_
#define SHFL_INCLUDE_UTILS_CRASHREPORT_H_

#include <string>

#include "workspace.h"

#ifdef _WIN32

#include "Windows.h"

#pragma comment(lib, "Dbghelp.lib")

string genStackTrace(CONTEXT *context);

#elif defined(__linux__) || defined(__APPLE__)

#include <execinfo.h>

string genStackTrace();

#endif

class CrashReport {
    vector<Workspace *> workspaces;
    string stackTrace;
    int sig = 0;

public:
    CrashReport setStackTrace(const string &stackTrace_);

    CrashReport setSignalNumber(int sig_);

    [[nodiscard]] string make() const;

    void save() const;
};

#endif //SHFL_INCLUDE_UTILS_CRASHREPORT_H_
