//
// Created by winch on 5/5/2023.
//

#ifndef SHUFFLE_INCLUDE_CONSOLE_H_
#define SHUFFLE_INCLUDE_CONSOLE_H_

#define RESET      "[0m"
#define BOLD       "[1m"
#define ITALIC     "[3m"
#define UNDERLINE  "[4m"

#define FG_BLACK   "[30m"
#define FG_RED     "[31m"
#define FG_GREEN   "[32m"
#define FG_YELLOW  "[33m"
#define FG_BLUE    "[34m"
#define FG_MAGENTA "[35m"
#define FG_CYAN    "[36m"
#define FG_WHITE   "[37m"

#define BG_BLACK   "[40m"
#define BG_RED     "[41m"
#define BG_GREEN   "[42m"
#define BG_YELLOW  "[43m"
#define BG_BLUE    "[44m"
#define BG_MAGENTA "[45m"
#define BG_CYAN    "[46m"
#define BG_WHITE   "[47m"

#include <string>
#include <initializer_list>

using namespace std;

void info(const string &text, const initializer_list<string> &args);
void warning(const string &text, const initializer_list<string> &args);
void error(const string &text, const initializer_list<string> &args);
void info(const string &text);
void warning(const string &text);
void error(const string &text);
void clear();
void white();
void too_many_arguments();
char readChar();
void gotoxy(int x, int y);
int wherex();
int wherey();

#endif //SHUFFLE_INCLUDE_CONSOLE_H_
