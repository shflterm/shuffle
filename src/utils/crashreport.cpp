#include "utils/crashreport.h"

#include <string>
#include <ctime>

#include "version.h"
#include "workspace.h"

using namespace std;

string getOSName() {
#ifdef _WIN32
  return "Windows";
#elif __APPLE__
  return "macOS";
#elif __linux__
    return "Linux";
#elif __unix__
    return "Unix";
#else
    return "Unknown";
#endif
}

string generateCrashReport() {
  stringstream ss;
  ss << "[ Shuffle Crash Report ]" << endl;
  ss << "OS: " << getOSName() << endl;
  ss << "Version: " << SHUFFLE_VERSION.str() << endl;
  ss << "Time: " << time(nullptr) << endl;
  ss << "========================" << endl;
  ss << "Workspaces: " << endl;
  for (auto &ws : wsMap) {
    ss << "  " << ws.first << endl;
    ss << "    Current Directory: " << ws.second->currentDirectory().string() << endl;
    ss << "    History: " << endl;
    for (auto &h : ws.second->getHistory()) {
      ss << "      " << h << endl;
    }
  }
  ss << "========================" << endl;
  return ss.str();
}