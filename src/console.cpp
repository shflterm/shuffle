#ifndef CONSOLE
#define CONSOLE

#include <iostream>
#include <string>

#include "i18n.cpp"

using namespace std;

enum PrintLevel {
  INFO, WARNING, ERROR
};

void print(PrintLevel level, const string& text) {
  string ansi = "\033[0m";
  if (level == WARNING) ansi = "\033[33m";
  if (level == ERROR) ansi = "\033[31m";
  cout << ansi << text << "\033[0m" << "\n";
}

void info(const string& text, const initializer_list<string> &args) {
  cout << "\033[0m" << translate(text, args) << "\033[0m" << "\n";
}

void warning(const string& text, const initializer_list<string> &args) {
  cout << "\033[33m" << translate(text, args) << "\033[0m" << "\n";
}

void error(const string& text, const initializer_list<string> &args) {
  cout << "\033[31m" << translate(text, args) << "\033[0m" << "\n";
}

void info(const string& text) {
  info(text, {});
}

void warning(const string& text) {
  warning(text, {});
}

void error(const string& text) {
  error(text, {});
}

void white() { cout << "\n"; }

void too_many_arguments() { error("too_many_arguments"); }

#endif