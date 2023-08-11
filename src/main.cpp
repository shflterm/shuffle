#include <term.h>
#include <filesystem>
#include <csignal>

#include "console.h"
#include "utils.h"
#include "crashreport.h"
#include "version.h"
#include "snippetmgr.h"
#include "builtincmd.h"
#include "automation.h"

#ifdef _WIN32

LONG WINAPI ExceptionHandler(EXCEPTION_POINTERS *exceptionPointers) {
    term << newLine;
    error("Sorry. Something went wrong with Shuffle. Go to the URL below and report the problem.");
    error("https://github.com/shflterm/shuffle/issues/new?template=crash-report.yaml");
    term << newLine;
    CrashReport report = CrashReport()
            .setStackTrace(genStackTrace(exceptionPointers->ContextRecord));
    error(report.make());
    report.save();
    exit(EXCEPTION_CONTINUE_SEARCH);
}

#elif defined(__linux__) || defined(__APPLE__)

extern "C" void handleCrash(int sig) {
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

#endif

extern "C" void handleQuit(int sig) {
    term << newLine << "Bye." << newLine;
    exit(sig);
}

int main(int argc, char *argv[]) {
#ifdef _WIN32
    SymInitialize(GetCurrentProcess(), nullptr, TRUE);

    SetUnhandledExceptionFilter(ExceptionHandler);
#elif defined(__linux__) || defined(__APPLE__)
    signal(SIGSEGV, &handleCrash);
    signal(SIGABRT, &handleCrash);
#endif
    signal(SIGINT, &handleQuit);

    if (argc > 1) {
        string arg = argv[1];
        if (arg == "--version" || arg == "-v") {
            term << "Shuffle " << SHUFFLE_VERSION.str() << newLine;
            return 0;
        }

        loadCommands();
        loadSnippets();
        loadAutomations();
        Workspace workspace;
        string cmd;
        for (int i = 1; i < argc; ++i) {
            cmd += argv[i];
            cmd += " ";
        }
        workspace.execute(cmd);
        return 0;
    }

    if (!exists(path(DOT_SHUFFLE))) create_directories(path(DOT_SHUFFLE));
    initShflJson();

    term << "Welcome to" << color(FOREGROUND, Blue) << " Shuffle " << SHUFFLE_VERSION.str() << resetColor << "!"
         << newLine
         << "(C) 2023 Kim Sihu. All Rights Reserved." << newLine << newLine;

    if (checkUpdate()) term << newLine;

    loadCommands();
    loadSnippets();
    loadAutomations();

    term << "Type 'help' to get help!" << newLine;

    currentWorkspace = new Workspace("main");
    while (true) {
        currentWorkspace->inputPrompt(true);
    }
}