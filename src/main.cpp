#include <iostream>

#include "command.cpp"
#include "console.cpp"

using namespace std;

int main() {
  info("loading");
  loadLanguageFile("ko_kr");
  loadCommands();
  system("clear");

  info("welcome", {"v1.0"});
  info("copyrights");
  white();
  info("help");

  while (true) {
    command();
  }
}