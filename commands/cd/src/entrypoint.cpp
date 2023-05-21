#include "libsapp.h"
#include "console.h"

SAPPENTRYPOINT entrypoint(Workspace &workspace, const vector<string> &args) {
  if (args.empty()) {
    too_many_arguments();
    return;
  }
  path dir = workspace.currentDirectory();

  if (args[0] == "..") dir = dir.parent_path();
  else dir.append(args[0]);

  if (!is_directory(dir) || !exists(dir)) {
    error("Directory '$0' not found.", {dir.string()});
    dir = dir.parent_path();
  }

  workspace.moveDirectory(dir);
}