#include <term.h>
#include <filesystem>
#include <csignal>

#include "console.h"
#include "utils.h"
#include "crashreport.h"
#include "version.h"
#include "snippetmgr.h"
#include "builtincmd.h"

#ifdef _WIN32

// ReSharper disable once CppParameterMayBeConstPtrOrRef
LONG WINAPI ExceptionHandler(EXCEPTION_POINTERS* exceptionPointers) {
    term << newLine;
    error("Sorry. Something went wrong with Shuffle. Go to the URL below and report the problem.");
    error("https://github.com/shflterm/shuffle/issues/new?template=crash-report.yaml");
    term << newLine;
    const CrashReport report = CrashReport()
            .setStackTrace(genStackTrace(exceptionPointers->ContextRecord));
    error(report.make());
    report.save();
    exit(EXCEPTION_CONTINUE_SEARCH);
}

#elif defined(__linux__) || defined(__APPLE__)

#include <unistd.h>
#include <termios.h>

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

extern "C" void handleQuit(const int sig) {
    term << newLine << "Bye." << newLine;
    exit(sig);
}

[[noreturn]] int main(const int argc, char* argv[]) {
#ifdef _WIN32
    SymInitialize(GetCurrentProcess(), nullptr, TRUE);

    SetUnhandledExceptionFilter(ExceptionHandler);
#elif defined(__linux__) || defined(__APPLE__)
    signal(SIGSEGV, &handleCrash);
    signal(SIGABRT, &handleCrash);

    setlocale(LC_ALL, "");

    termios raw{};
    tcgetattr(STDIN_FILENO, &raw);
    raw.c_lflag &= ~(ECHO | ICANON);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
#endif
    signal(SIGINT, &handleQuit);

    if (getShflJson("repos").empty()) {
        Json::Value repos;
        repos.append("https://raw.githubusercontent.com/shflterm/apps/main/repo.json");
        setShflJson("repos", repos);
    }

    if (argc > 1) {
        if (const string arg = argv[1];
            arg == "--version" || arg == "-v") {
            term << "Shuffle " << SHUFFLE_VERSION.str() << newLine;
            return 0;
        }

        loadCommands();
        loadSnippets();
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

    term << "Type 'help' to get help!" << newLine;

    currentWorkspace = new Workspace("main");
    while (true) {
        currentWorkspace->inputPrompt();
    }
}
