//
// Created by winch on 5/6/2023.
//

#ifndef SHUFFLE_INCLUDE_SAPP_RUNNABLE_H_
#define SHUFFLE_INCLUDE_SAPP_RUNNABLE_H_

#include <vector>
#include <string>

#include "workspace.h"

using namespace std;

typedef void (*CommandExecutor)(Workspace workspace, const vector<string>& args);
void emptyExecutor(Workspace workspace, const vector<string>& args);

#endif //SHUFFLE_INCLUDE_SAPP_RUNNABLE_H_
