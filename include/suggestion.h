//
// Created by winch on 5/5/2023.
//

#ifndef SHUFFLE_INCLUDE_SUGGESTION_H_
#define SHUFFLE_INCLUDE_SUGGESTION_H_

#include <string>
#include <vector>

#include "commandmgr.h"

using namespace std;

string findSuggestion(const string &input, const vector<Command> &DICTIONARY);
vector<string> createSuggestions(const string &str, const vector<Command>& DICTIONARY);

#endif //SHUFFLE_INCLUDE_SUGGESTION_H_
