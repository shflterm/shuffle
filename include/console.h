//
// Created by winch on 5/5/2023.
//

#ifndef SHUFFLE_INCLUDE_CONSOLE_H_
#define SHUFFLE_INCLUDE_CONSOLE_H_

#include <string>
#include <initializer_list>

using namespace std;

enum PrintLevel {
  INFO, WARNING, ERROR
};

void print(PrintLevel level, const string &text);
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
