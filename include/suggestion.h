//
// Created by winch on 5/5/2023.
//

#ifndef SHUFFLE_INCLUDE_SUGGESTION_H_
#define SHUFFLE_INCLUDE_SUGGESTION_H_

#include <string>
#include <vector>

#include "cmd/commandmgr.h"

using namespace std;

vector<string> findSuggestion(Workspace ws,
                              const string &input,
                              const vector<string> &DICTIONARY);

vector<string> createSuggestions(Workspace ws,
                                 const string &str,
                                 const vector<string> &DICTIONARY);

#endif //SHUFFLE_INCLUDE_SUGGESTION_H_
