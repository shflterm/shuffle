#include "command.h"
#include "console.h"
#include "i18n.h"

using namespace std;

int main() {
  print(INFO, "Loading Shuffle...");
  loadLanguageFile("en_us");
  loadCommands();
#ifdef _WIN32
  system("cls");
#elif __linux__
  system("clear");
#endif

  info("system.welcome", {"v1.0"});
  info("system.copyrights");
  white();
  info("system.help");

  while (true) {
    command();
  }
}