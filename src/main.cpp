#include <iostream>

#include "command.cpp"
#include "console.cpp"

using namespace std;

int main() {
  print(INFO, "Loading Shuffle...");
  loadLanguageFile("en_us");
  loadCommands();
  system("clear");

  info("system.welcome", {"v1.0"});
  info("system.copyrights");
  white();
  info("system.help");

  while (true) {
    command();
  }
}