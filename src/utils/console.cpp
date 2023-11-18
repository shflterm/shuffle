#include "console.h"

#include <string>
#include "term.h"

#include "i18n.h"

bool debugMode = false;

Workspace* currentWorkspace;

void debug(const string&text, const initializer_list<string>&args) {
    if (debugMode) {
        term << "[90m" << translate(text, args) << resetColor << "\n";
    }
}

void info(const string&text, const initializer_list<string>&args) {
    term << resetColor << translate(text, args) << resetColor << "\n";
}

void success(const string&text, const initializer_list<string>&args) {
    term << color(FOREGROUND, Green) << translate(text, args) << resetColor << "\n";
}

void warning(const string&text, const initializer_list<string>&args) {
    term << color(FOREGROUND, Yellow) << translate(text, args) << resetColor << "\n";
}

void error(const string&text, const initializer_list<string>&args) {
    term << color(FOREGROUND, Red) << translate(text, args) << resetColor << "\n";
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
    while (!_kbhit()) {}
    return _getch();
#elif defined(__linux__) || defined(__APPLE__)
    return getwchar();
#endif
}

int wherex() {
#ifdef _WIN32
    CONSOLE_SCREEN_BUFFER_INFO buf;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &buf);
    return buf.dwCursorPosition.X;
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
    return buf.dwCursorPosition.Y;
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
