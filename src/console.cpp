#ifndef CONSOLE
#define CONSOLE

#include <iostream>
#include <string>

using namespace std;

enum PrintLevel { INFO, WARNING, ERROR };

void print(PrintLevel level, string text) { cout << text << "\n"; }

void white() { cout << "\n"; }

#endif