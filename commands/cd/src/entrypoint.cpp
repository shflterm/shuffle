#include "libsapp.h"
#include "console.h"

extern "C" __declspec(dllexport) void entrypoint(Workspace workspace, const vector<string> &args) {
  if (args.empty()) {
    too_many_arguments();
    return;
  }
  path dir = workspace.dir;

  dir.append(args[1]);

  if (!is_directory(dir) || !exists(dir)) {
    error("cd.directory_not_found", {dir.string()});
    dir = dir.parent_path();
  }

  workspace.dir = dir;
}