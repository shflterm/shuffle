#include <iostream>

#include "command.cpp"
#include "console.cpp"

using namespace std;

int main() {
  print(INFO, "Loading commands...");
  loadCommands();
  system("clear");
  print(INFO, "Welcome to SHUFFLE v1.0");
  print(INFO, "(C) 2023. Shuffle Team All rights reserved");
  white();
  print(INFO, "Type 'help' to get help!");
  while (true) {
    command();
  }
}