//
// Created by winch on 5/6/2023.
//

#ifndef SHUFFLE_INCLUDE_LOADER_H_
#define SHUFFLE_INCLUDE_LOADER_H_

#include "commandmgr.h"
#include "utils/cmdexecutor.h"

#include <vector>
#include <string>

using namespace std;

class SAPPCommand : public Command {
 public:
  explicit SAPPCommand(const string &name);
  void run(const vector<std::string> &args) const override;
};

#endif //SHUFFLE_INCLUDE_LOADER_H_
