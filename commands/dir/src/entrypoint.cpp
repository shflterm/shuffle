#include "libsapp.h"
#include "console.h"

SAPPENTRYPOINT void entrypoint(Workspace &workspace, const vector<string> &args) {
  path dir = workspace.currentDirectory();
  info(dir.string());
}