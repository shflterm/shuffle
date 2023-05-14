#include "libsapp.h"
#include "console.h"

extern "C" __declspec(dllexport) void entrypoint(Workspace &workspace, const vector<string> &args) {
  path dir = workspace.currentDirectory();
  info(dir.string());
}