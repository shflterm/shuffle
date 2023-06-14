#include "utils/crashreport.h"

#include <string>
#include <ctime>

#include "version.h"
#include "workspace.h"
#ifdef _WIN32
#elif __linux__ || __APPLE__
#include <execinfo.h>
#endif

using namespace std;

string printStackTrace() {
  stringstream ss;

#ifdef _WIN32
  ss << "Stack traces for Windows are in the works!" << endl;
#elif __linux__ || __APPLE__
  void *array[10];
  size_t size;
  size = backtrace(array, 10);
  for (int i = 0; i < size; ++i) {
    ss << backtrace_symbols(array, size)[i] << endl;
  }
#endif

  return ss.str();
}

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
  }
  ss << "========================" << endl;
  ss << "Stack Trace: " << endl;
  ss << printStackTrace();
  return ss.str();
}