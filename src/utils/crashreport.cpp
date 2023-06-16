#include "utils/crashreport.h"

#include <string>
#include <ctime>
#include <fstream>

#include "utils/utils.h"
#include "term.h"

#include "version.h"
#ifdef _WIN32
#elif __linux__ || __APPLE__
#include <execinfo.h>
#endif

using namespace std;

string genStackTrace() {
  stringstream ss;

#ifdef _WIN32
  ss << "  Stack traces for Windows are in the works!" << endl;
#elif __linux__ || __APPLE__
  void *array[10];
  size_t size;
  size = backtrace(array, 10);
  for (int i = 0; i < size; ++i) {
    ss << "  " << backtrace_symbols(array, size)[i] << endl;
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

CrashReport CrashReport::setStackTrace(const string &stackTrace_) {
  stackTrace = stackTrace_;
  return *this;
}

CrashReport CrashReport::setSignalNumber(int sig_) {
  sig = sig_;
  return *this;
}

string CrashReport::make() {
  stringstream ss;
  ss << "[ Shuffle Crash Report ]" << endl;
  ss << "OS: " << getOSName() << endl;
  ss << "Version: " << SHUFFLE_VERSION.str() << endl;
  ss << "Time: " << time(nullptr) << endl;
  ss << "Signal Number: " << sig << endl;
  ss << "========================" << endl;
  ss << "Workspaces: " << endl;
  for (auto &ws : wsMap) {
    ss << "  " << ws.first << endl;
    ss << "    Current Directory: " << ws.second->currentDirectory().string() << endl;
    ss << "    History: " << endl;
    for (int i = 0; i < ws.second->getHistory().size(); ++i) {
      auto item = ws.second->getHistory()[i];
      ss << "      " << i << ". " << item << endl;
    }
    if (ws.second->getHistory().empty()) ss << "      (empty)" << endl;
  }
  ss << "========================" << endl;
  ss << "Stack Trace: " << endl;
  ss << stackTrace;
  return ss.str();
}

void CrashReport::save() {
  ofstream file(DOT_SHUFFLE + "/crash-report-" + to_string(time(nullptr)) + ".txt");
  file << make();
  file.close();

  term << "Stack trace saved to \"" << DOT_SHUFFLE << "/crash-report-" << to_string(time(nullptr)) << ".txt" << "\"."
       << newLine;
}
