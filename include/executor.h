//
// Created by winch on 5/5/2023.
//

#ifndef SHUFFLE_INCLUDE_EXECUTOR_H_
#define SHUFFLE_INCLUDE_EXECUTOR_H_

#include <string>
#include <filesystem>

using namespace std;
namespace fs = std::filesystem;
using namespace fs;

extern path dir;

void execute(const string& input);

#endif //SHUFFLE_INCLUDE_EXECUTOR_H_
