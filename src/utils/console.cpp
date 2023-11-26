#include "utils/console.h"

#include <string>
#include <iostream>

#include "utils/i18n.h"

#ifdef _WIN32
#include <Windows.h>
#elif defined(__linux__) || defined(__APPLE__)
#include <unistd.h>
#endif

using std::cout, std::cerr, std::to_string;

bool debugMode = false;

Workspace* currentWorkspace;

bool isAnsiSupported() {
#ifdef _WIN32
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode;
    if (GetConsoleMode(hConsole, &mode)) {
        return (mode & ENABLE_VIRTUAL_TERMINAL_PROCESSING) != 0;
    }
    return false;
#elif defined(__linux__) || defined(__APPLE__)
    return isatty(STDOUT_FILENO) != 0;
#endif
}

void initAnsiCodes() {
    if (!isAnsiSupported()) return;
    reset = "\033[0m";
    fg_black = "\033[30m";
    fg_red = "\033[31m";
    fg_green = "\033[32m";
    fg_yellow = "\033[33m";
    fg_blue = "\033[34m";
    fg_magenta = "\033[35m";
    fg_cyan = "\033[36m";
    fg_white = "\033[37m";

    fgb_black = "\033[90m";
    fgb_red = "\033[91m";
    fgb_green = "\033[92m";
    fgb_yellow = "\033[93m";
    fgb_blue = "\033[94m";
    fgb_magenta = "\033[95m";
    fgb_cyan = "\033[96m";
    fgb_white = "\033[97m";

    bg_black = "\033[40m";
    bg_red = "\033[41m";
    bg_green = "\033[42m";
    bg_yellow = "\033[43m";
    bg_blue = "\033[44m";
    bg_magenta = "\033[45m";
    bg_cyan = "\033[46m";
    bg_white = "\033[47m";

    bgb_black = "\033[100m";
    bgb_red = "\033[101m";
    bgb_green = "\033[102m";
    bgb_yellow = "\033[103m";
    bgb_blue = "\033[104m";
    bgb_magenta = "\033[105m";
    bgb_cyan = "\033[106m";
    bgb_white = "\033[107m";

    erase_line = "\033[2K";
    erase_cursor_to_end = "\033[K";

    save_cursor_pos = "\033[s";
    restore_cursor_pos = "\033[u";
}

void debug(const string&text, const initializer_list<string>&args) {
    if (debugMode) {
        cout << fgb_black << translate(text, args) << reset << "\n";
        cout.flush();
    }
}

void info(const string&text, const initializer_list<string>&args) {
    cout << reset << translate(text, args) << reset << "\n";
    cout.flush();
}

void success(const string&text, const initializer_list<string>&args) {
    cout << fg_green << translate(text, args) << reset << "\n";
    cout.flush();
}

void warning(const string&text, const initializer_list<string>&args) {
    cout << fg_yellow << translate(text, args) << reset << "\n";
    cout.flush();
}

void error(const string&text, const initializer_list<string>&args) {
    cout << fg_red << translate(text, args) << reset << "\n";
    cout.flush();
}

void debug(const string&text) {
    debug(text, {});
}

void info(const string&text) {
    info(text, {});
}

void success(const string&text) {
    success(text, {});
}

void warning(const string&text) {
    warning(text, {});
}

void error(const string&text) {
    error(text, {});
}

#ifdef _WIN32
#define NOMINMAX 1
#define byte win_byte_override
#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <conio.h>

#elif defined(__linux__) || defined(__APPLE__)
#include <unistd.h>
#endif

int readChar() {
#ifdef _WIN32
    return _getch();
#elif defined(__linux__) || defined(__APPLE__)
    return getwchar();
#endif
}

int wherex() {
#ifdef _WIN32
    CONSOLE_SCREEN_BUFFER_INFO buf;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &buf);
    return buf.dwCursorPosition.X + 1;
#elif defined(__linux__) || defined(__APPLE__)
    printf("\033[6n");
    if (readChar() != '\x1B') return 0;
    if (readChar() != '\x5B') return 0;
    int in;
    int ly = 0;
    while ((in = readChar()) != ';')
        ly = ly * 10 + in - '0';
    int lx = 0;
    while ((in = readChar()) != 'R')
        lx = lx * 10 + in - '0';
    return lx;
#endif
}

int wherey() {
#ifdef _WIN32
    CONSOLE_SCREEN_BUFFER_INFO buf;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &buf);
    return buf.dwCursorPosition.Y + 1;
#elif defined(__linux__) || defined(__APPLE__)
    printf("\033[6n");
    if (readChar() != '\x1B') return 0;
    if (readChar() != '\x5B') return 0;
    int in;
    int ly = 0;
    while ((in = readChar()) != ';')
        ly = ly * 10 + in - '0';
    int lx = 0;
    while ((in = readChar()) != 'R')
        lx = lx * 10 + in - '0';
    return ly;
#endif
}

string teleport(const int x, const int y) {
    if (!isAnsiSupported()) return "";
    return "\033[" + to_string(y) + ";" + to_string(x) + "H";
}
