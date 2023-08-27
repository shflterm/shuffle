#include "workspace/crashreport.h"

#include <string>
#include <ctime>
#include <fstream>

#include "utils.h"
#include "term.h"

#include "version.h"
#include "console.h"
#include "commandmgr.h"

using std::endl, std::to_string, std::ofstream;

#ifdef _WIN32

string genStackTrace(CONTEXT *context) {
    stringstream ss;

    const HANDLE hProcess = GetCurrentProcess();
    const HANDLE hThread = GetCurrentThread();

    STACKFRAME64 stackFrame;
    memset(&stackFrame, 0, sizeof(STACKFRAME64));
    DWORD machineType;

#ifdef _M_IX86
    machineType = IMAGE_FILE_MACHINE_I386;
    stackFrame.AddrPC.Offset = context->Eip;
    stackFrame.AddrFrame.Offset = context->Ebp;
    stackFrame.AddrStack.Offset = context->Esp;
#elif _M_X64
    machineType = IMAGE_FILE_MACHINE_AMD64;
    stackFrame.AddrPC.Offset = context->Rip;
    stackFrame.AddrFrame.Offset = context->Rsp;
    stackFrame.AddrStack.Offset = context->Rsp;
#elif _M_IA64
    machineType = IMAGE_FILE_MACHINE_IA64;
      stackFrame.AddrPC.Offset = context->StIIP;
      stackFrame.AddrFrame.Offset = context->IntSp;
      stackFrame.AddrBStore.Offset = context->RsBSP;
      stackFrame.AddrStack.Offset = context->IntSp;
#else
#error "Unsupported architecture"
#endif

    stackFrame.AddrPC.Mode = AddrModeFlat;
    stackFrame.AddrFrame.Mode = AddrModeFlat;
    stackFrame.AddrStack.Mode = AddrModeFlat;

    int frameCount = 0;

    while (StackWalk64(machineType, hProcess, hThread, &stackFrame, context, nullptr,
                       SymFunctionTableAccess64, SymGetModuleBase64, nullptr)) {
        // Print the frame number, module name, and function name
        ss << "    [" << std::dec << frameCount << "] ";

        DWORD64 moduleBase = SymGetModuleBase64(hProcess, stackFrame.AddrPC.Offset);
        char moduleName[MAX_PATH];
        if (GetModuleFileNameA(reinterpret_cast<HMODULE>(moduleBase), moduleName, MAX_PATH) != 0) {
            ss << moduleName;
        }

        ss << "! ";

        DWORD64 displacement = 0;
        const DWORD maxSymbolNameLen = 256;
        BYTE symbolBuffer[sizeof(IMAGEHLP_SYMBOL64) + maxSymbolNameLen];
        auto *symbol = reinterpret_cast<IMAGEHLP_SYMBOL64 *>(symbolBuffer);
        symbol->SizeOfStruct = sizeof(IMAGEHLP_SYMBOL64);
        symbol->MaxNameLength = maxSymbolNameLen;

        SymGetSymFromAddr64(hProcess, stackFrame.AddrPC.Offset, &displacement, symbol);
//    if (SymGetSymFromAddr64(hProcess, stackFrame.AddrPC.Offset, &displacement, symbol)) {
//      ss << symbol->Name;
//    } else {
//      ss << "Unknown Symbol";
//    }
        ss << symbol->Name;

        ss << endl;

        frameCount++;
    }

    return ss.str();
}

#elif defined(__linux__) || defined(__APPLE__)

string genStackTrace() {
    stringstream ss;

    void *array[10];
    size_t size;
    size = backtrace(array, 10);
    for (int i = 0; i < size; ++i) {
        ss << "  " << backtrace_symbols(array, size)[i] << endl;
    }

    return ss.str();
}

#endif

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
    ss << "Apps: " << endl;
    for (const auto &item: commands) {
        ss << "  " << item->getName() << endl;
    }
    ss << "========================" << endl;
    ss << "Workspaces: " << endl;
    for (auto &ws: wsMap) {
        ss << "  " << ws.first << (ws.first == currentWorkspace->getName() ? " (current)" : "") << endl;
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
    string filePath = DOT_SHUFFLE + "/crash-report-" + to_string(time(nullptr)) + ".txt";
    ofstream file(filePath);
    file << make();
    file.close();

    term << "Stack trace saved to \"" << filePath << "\"." << newLine;
}
