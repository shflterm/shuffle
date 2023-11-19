//
// Created by winch on 5/5/2023.
//

#ifndef SHUFFLE_INCLUDE_CONSOLE_H_
#define SHUFFLE_INCLUDE_CONSOLE_H_

#define RESET "\033[0m"
#define FG_BLACK "\033[30m"
#define FG_RED "\033[31m"
#define FG_GREEN "\033[32m"
#define FG_YELLOW "\033[33m"
#define FG_BLUE "\033[34m"
#define FG_MAGENTA "\033[35m"
#define FG_CYAN "\033[36m"
#define FG_WHITE "\033[37m"

#define FGB_BLACK "\033[90m"
#define FGB_RED "\033[91m"
#define FGB_GREEN "\033[92m"
#define FGB_YELLOW "\033[93m"
#define FGB_BLUE "\033[94m"
#define FGB_MAGENTA "\033[95m"
#define FGB_CYAN "\033[96m"
#define FGB_WHITE "\033[97m"

#define BG_BLACK "\033[40m"
#define BG_RED "\033[41m"
#define BG_GREEN "\033[42m"
#define BG_YELLOW "\033[43m"
#define BG_BLUE "\033[44m"
#define BG_MAGENTA "\033[45m"
#define BG_CYAN "\033[46m"
#define BG_WHITE "\033[47m"

#define BGB_BLACK "\033[100m"
#define BGB_RED "\033[101m"
#define BGB_GREEN "\033[102m"
#define BGB_YELLOW "\033[103m"
#define BGB_BLUE "\033[104m"
#define BGB_MAGENTA "\033[105m"
#define BGB_CYAN "\033[106m"
#define BGB_WHITE "\033[107m"

#define ERASE_ALL "\033[2J"
#define ERASE_LINE "\033[2K"
#define ERASE_CURSOR_TO_END "\033[K"

#define SAVE_CURSOR_POS "\033[s"
#define RESTORE_CURSOR_POS "\033[u"

#include <string>
#include <initializer_list>

#include "workspace.h"

using std::initializer_list;

extern Workspace *currentWorkspace;

void debug(const string &text, const initializer_list<string> &args);

void info(const string &text, const initializer_list<string> &args);

void success(const string &text, const initializer_list<string> &args);

void warning(const string &text, const initializer_list<string> &args);

void error(const string &text, const initializer_list<string> &args);

void debug(const string &text);

void info(const string &text);

void success(const string &text);

void warning(const string &text);

void error(const string &text);

int readChar();

int wherex();

int wherey();

string teleport(int x, int y);

#endif //SHUFFLE_INCLUDE_CONSOLE_H_
