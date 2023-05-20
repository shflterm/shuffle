//
// Created by winch on 5/6/2023.
//

#ifndef SHUFFLE_INCLUDE_LOADER_H_
#define SHUFFLE_INCLUDE_LOADER_H_

#include "commandmgr.h"
#include "utils/cmdexecutor.h"
#include "lua.hpp"

#include <vector>
#include <string>

using namespace std;

enum SAPPType {
  NORMAL, SCRIPT
};

class SAPPCommand : public Command {
 protected:
  SAPPType type;
  lua_State *L;

 public:
  explicit SAPPCommand(const string &name);
  void run(Workspace &ws, const vector<string> &args) const override;
};

#endif //SHUFFLE_INCLUDE_LOADER_H_
