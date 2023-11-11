//
// Created by winch on 5/5/2023.
//

#ifndef SHUFFLE_INCLUDE_CONSOLE_H_
#define SHUFFLE_INCLUDE_CONSOLE_H_

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

#endif //SHUFFLE_INCLUDE_CONSOLE_H_
