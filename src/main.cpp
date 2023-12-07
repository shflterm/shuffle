#include <iostream>
#include <filesystem>
#include <csignal>
#include <ai/genllama.h>

#include "console.h"
#include "utils.h"
#include "crashreport.h"
#include "version.h"
#include "snippetmgr.h"
#include "builtincmd.h"
#include "shfljson.h"

using std::filesystem::create_directories, std::filesystem::exists, std::cout, std::cerr, std::endl;
#define PYBIND11_DETAILED_ERROR_MESSAGES

#ifdef _WIN32
#include <DbgHelp.h>

// ReSharper disable once CppParameterMayBeConstPtrOrRef
LONG WINAPI ExceptionHandler(EXCEPTION_POINTERS* exceptionPointers) {
    cout << endl;
    error("Sorry. Something went wrong with Shuffle. Go to the URL below and report the problem.");
    error("https://github.com/shflterm/shuffle/issues/new?template=crash-report.yaml");
    cout << endl;
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
    cout << endl;
    error("Sorry. Something went wrong with Shuffle. Go to the URL below and report the problem.");
    error("https://github.com/shflterm/shuffle/issues/new?template=crash-report.yaml");
    cout << endl;
    CrashReport report = CrashReport()
            .setStackTrace(genStackTrace())
            .setSignalNumber(sig);
    error(report.make());
    report.save();
    exit(1);
}

#endif

extern "C" void handleQuit(const int sig) {
    cout << endl << "Bye." << endl;
    exit(sig);
}

int main(const int argc, char* argv[]) {
    info("Loading AI model...");
    loadAiModel(R"(../../llama-2-7b-chat.Q4_K_M.gguf)");
    success("Done.");
#ifdef _WIN32
    system("cls");
#elif defined(__linux__) || defined(__APPLE__)
    system("clear");
#endif

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

    initAnsiCodes();

    if (getShflJson("repos").empty()) {
        Json::Value repos;
        repos.append("https://raw.githubusercontent.com/shflterm/apps/main/repo.json");
        setShflJson("repos", repos);
    }

    if (argc > 1) {
        if (const string arg = argv[1];
            arg == "--version" || arg == "-v") {
            cout << "Shuffle " << SHUFFLE_VERSION.str() << endl;
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
        workspace.createJob(cmd)->start(&workspace);
        return 0;
    }

    if (!exists(DOT_SHUFFLE)) create_directories(DOT_SHUFFLE);
    initShflJson();

    cout << "Welcome to" << fg_blue << " Shuffle " << SHUFFLE_VERSION.str() << reset << "!"
            << endl
            << "(C) 2023 Kim Sihu. All Rights Reserved." << endl << endl;

    if (!isAnsiSupported()) {
        error("ANSI escape codes are not supported on this terminal.");
        error("Shuffle may not work properly.");
        error("");
    }

    if (checkUpdate()) cout << endl;

    loadCommands();
    loadSnippets();

    cout << "Type 'help' to get help!" << endl;

    currentWorkspace = new Workspace("main");
    // ReSharper disable once CppDFAEndlessLoop
    while (true) {
        currentWorkspace->inputPrompt();
    }
}
