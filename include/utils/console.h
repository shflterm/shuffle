//
// Created by winch on 5/5/2023.
//

#ifndef SHUFFLE_INCLUDE_CONSOLE_H_
#define SHUFFLE_INCLUDE_CONSOLE_H_

#include <string>
#include <initializer_list>

#include "workspace/workspace.h"

using std::initializer_list;

inline string reset;
inline string fg_black;
inline string fg_red;
inline string fg_green;
inline string fg_yellow;
inline string fg_blue;
inline string fg_magenta;
inline string fg_cyan;
inline string fg_white;

inline string fgb_black;
inline string fgb_red;
inline string fgb_green;
inline string fgb_yellow;
inline string fgb_blue;
inline string fgb_magenta;
inline string fgb_cyan;
inline string fgb_white;

inline string bg_black;
inline string bg_red;
inline string bg_green;
inline string bg_yellow;
inline string bg_blue;
inline string bg_magenta;
inline string bg_cyan;
inline string bg_white;

inline string bgb_black;
inline string bgb_red;
inline string bgb_green;
inline string bgb_yellow;
inline string bgb_blue;
inline string bgb_magenta;
inline string bgb_cyan;
inline string bgb_white;

inline string erase_line;
inline string erase_cursor_to_end;

inline string save_cursor_pos;
inline string restore_cursor_pos;

bool isAnsiSupported();

void initAnsiCodes();

extern Workspace* currentWorkspace;

void debug(const string&text, const initializer_list<string>&args);

void info(const string&text, const initializer_list<string>&args);

void success(const string&text, const initializer_list<string>&args);

void warning(const string&text, const initializer_list<string>&args);

void error(const string&text, const initializer_list<string>&args);

void debug(const string&text);

void info(const string&text);

void success(const string&text);

void warning(const string&text);

void error(const string&text);

int readChar();

int wherex();

int wherey();

string teleport(int x, int y);

#endif //SHUFFLE_INCLUDE_CONSOLE_H_
