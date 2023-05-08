#include "console.h"

#include <iostream>
#include <string>

#include "i18n.h"

using namespace std;

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
#elif __linux__
  system("clear");
#endif
}

void white() { cout << "\n"; }

void too_many_arguments() { error("system.too_many_arguments"); }

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <conio.h>
#elif __linux__
#include <unistd.h>
#include <termios.h>
#endif

char readChar() {
#ifdef _WIN32 // Windows 환경
  while (!_kbhit()) {}
  return _getch();
#else // Linux 환경
  char buf = 0;
    struct termios old = {0};
    fflush(stdout);
    if(tcgetattr(0, &old) < 0)
        perror("tcsetattr()");
    old.c_lflag &= ~ICANON;
    old.c_lflag &= ~ECHO;
    old.c_cc[VMIN] = 1;
    old.c_cc[VTIME] = 0;
    if(tcsetattr(0, TCSANOW, &old) < 0)
        perror("tcsetattr ICANON");
    if(read(0, &buf, 1) < 0)
        perror("read()");
    old.c_lflag |= ICANON;
    old.c_lflag |= ECHO;
    if(tcsetattr(0, TCSADRAIN, &old) < 0)
        perror("tcsetattr ~ICANON");
    return buf;
#endif
}

void gotoxy(int x, int y) {
#ifdef _WIN32
  COORD pos;
  pos.X = (short) x;
  pos.Y = (short) y;
  SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
#elif __linux__
  printf("\033[%d;%df",y,x);
#endif
}

int wherex() {
#ifdef _WIN32
  CONSOLE_SCREEN_BUFFER_INFO buf;
  GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &buf);
  return buf.dwCursorPosition.X;
#elif __linux__
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
#elif __linux__
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