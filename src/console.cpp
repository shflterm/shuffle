#include "console.h"

#include <iostream>
#include <string>

#include "i18n.h"

using namespace std;

bool debugMode = false;

void debug(const string &text, const initializer_list<string> &args) {
  if (debugMode) {
    cout << "[90m" << translate(text, args) << RESET << "\n";
    cout.flush();
  }
}

void info(const string &text, const initializer_list<string> &args) {
  cout << RESET << translate(text, args) << RESET << "\n";
  cout.flush();
}

void success(const string &text, const initializer_list<string> &args) {
  cout << FG_GREEN << translate(text, args) << RESET << "\n";
  cout.flush();
}

void warning(const string &text, const initializer_list<string> &args) {
  cout << FG_YELLOW << translate(text, args) << RESET << "\n";
  cout.flush();
}

void error(const string &text, const initializer_list<string> &args) {
  cout << FG_RED << translate(text, args) << RESET << "\n";
  cout.flush();
}

void debug(const string &text) {
  debug(text, {});
}

void info(const string &text) {
  info(text, {});
}

void success(const string &text) {
  success(text, {});
}

void warning(const string &text) {
  warning(text, {});
}

void error(const string &text) {
  error(text, {});
}

void clear() {
#ifdef _WIN32
  system("cls");
#elif __linux__ || __APPLE__
  system("clear");
#endif
}

void white() { cout << "\n"; }

void too_many_arguments() { error("There are too many or too few arguments."); }

#ifdef _WIN32
#define NOMINMAX 1
#define byte win_byte_override
#define WIN32_LEAN_AND_MEAN
#define _HAS_STD_BYTE 0

#include <Windows.h>
#include <conio.h>
#elif __linux__ || __APPLE__
#include <unistd.h>
#include <termios.h>
#endif

char readChar() {
#ifdef _WIN32
  while (!_kbhit()) {}
  return _getch();
#elif __linux__ || __APPLE__
  struct termios oldt, newt;
  int ch;
  tcgetattr(STDIN_FILENO, &oldt);
  newt = oldt;
  newt.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);
  ch = getchar();
  tcsetattr(STDIN_FILENO, TCSANOW, &oldt);

  return ch;
#endif
}

void gotoxy(int x, int y) {
#ifdef _WIN32
  COORD pos;
  pos.X = (short) x;
  pos.Y = (short) y;
  SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
#elif __linux__ || __APPLE__
  printf("\033[%d;%dH", y, x);
#endif
}

int wherex() {
#ifdef _WIN32
  CONSOLE_SCREEN_BUFFER_INFO buf;
  GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &buf);
  return buf.dwCursorPosition.X;
#elif __linux__ || __APPLE__
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
#elif __linux__ || __APPLE__
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