#ifndef CONSOLE
#define CONSOLE

#include <iostream>
#include <string>

#include "i18n.cpp"

using namespace std;

enum PrintLevel {
  INFO, WARNING, ERROR
};

void print(PrintLevel level, const string &text) {
  string ansi = "\033[0m";
  if (level == WARNING) ansi = "\033[33m";
  if (level == ERROR) ansi = "\033[31m";
  cout << ansi << text << "\033[0m" << "\n";
}

void info(const string &text, const initializer_list<string> &args) {
  cout << "\033[0m" << translate(text, args) << "\033[0m" << "\n";
  cout.flush();
}

void warning(const string &text, const initializer_list<string> &args) {
  cout << "\033[33m" << translate(text, args) << "\033[0m" << "\n";
  cout.flush();
}

void error(const string &text, const initializer_list<string> &args) {
  cout << "\033[31m" << translate(text, args) << "\033[0m" << "\n";
  cout.flush();
}

void info(const string &text) {
  info(text, {});
}

void warning(const string &text) {
  warning(text, {});
}

void error(const string &text) {
  error(text, {});
}

void white() { cout << "\n"; }

void too_many_arguments() { error("too_many_arguments"); }

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#elif __linux__
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#endif

int getch() {
#ifdef WIN32
  //TODO
#elif __linux__
  struct termios oldt{}, newt{};
  int ch;
  tcgetattr(STDIN_FILENO, &oldt);
  newt = oldt;
  newt.c_lflag &= ~ICANON;
  newt.c_lflag &= ~ECHO;
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);
  ch = getchar();
  tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
  return ch;
#endif
}

void gotoxy(int x, int y) {
#ifdef WIN32
  COORD pos;
  pos.X = x;
  pos.Y = y;
  SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
#elif __linux__
  printf("\033[%d;%df",y,x);
#endif
}

int wherex() {
#ifdef WIN32
  CONSOLE_SCREEN_BUFFER_INFO buf;
  GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &buf);
  return buf.dwCursorPosition.X;
#elif __linux__
  printf("\033[6n");
  if (getch() != '\x1B') return 0;
  if (getch() != '\x5B') return 0;
  int in;
  int ly = 0;
  while ((in = getch()) != ';')
    ly = ly * 10 + in - '0';
  int lx = 0;
  while ((in = getch()) != 'R')
    lx = lx * 10 + in - '0';
  return lx;
#endif
}

int wherey() {
#ifdef WIN32
  CONSOLE_SCREEN_BUFFER_INFO buf;
  GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &buf);
  return buf.dwCursorPosition.Y;
#elif __linux__
  printf("\033[6n");
  if (getch() != '\x1B') return 0;
  if (getch() != '\x5B') return 0;
  int in;
  int ly = 0;
  while ((in = getch()) != ';')
    ly = ly * 10 + in - '0';
  int lx = 0;
  while ((in = getch()) != 'R')
    lx = lx * 10 + in - '0';
  return ly;
#endif
}

#endif