//
// Created by winch on 5/5/2023.
//

#ifndef SHUFFLE_INCLUDE_COMPLETION_H_
#define SHUFFLE_INCLUDE_COMPLETION_H_

#include <string>
#include <vector>

#include "command.h"

using namespace std;

vector<string> createSuggestions(const string &str, const vector<Command>& DICTIONARY);

#endif //SHUFFLE_INCLUDE_COMPLETION_H_
