//
// Created by winch on 5/6/2023.
//

#ifndef SHUFFLE_INCLUDE_SAPP_RUNNABLE_H_
#define SHUFFLE_INCLUDE_SAPP_RUNNABLE_H_

#include <vector>
#include <string>

using namespace std;

typedef void (*CommandExecutor)(const vector<string>& args);
void emptyExecutor(const vector<string>& args);

#endif //SHUFFLE_INCLUDE_SAPP_RUNNABLE_H_
