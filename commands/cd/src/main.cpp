#include "libsapp.h"
#include "commandmgr.h"
#include "console.h"

ENTRYPOINT(Workspace workspace, const vector<string> &args) {
  if (args.size() != 2) {
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