//
// Created by winch on 5/5/2023.
//

#ifndef SHUFFLE_INCLUDE_BUILTINCMD_H_
#define SHUFFLE_INCLUDE_BUILTINCMD_H_

#include <string>
#include <vector>

#include "workspace.h"

using namespace std;

void shflCmd(Workspace ws, vector<string> args);
void helpCmd(Workspace ws, vector<string> args); 

#endif //SHUFFLE_INCLUDE_BUILTINCMD_H_
