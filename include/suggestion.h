//
// Created by winch on 5/5/2023.
//

#ifndef SHUFFLE_INCLUDE_SUGGESTION_H_
#define SHUFFLE_INCLUDE_SUGGESTION_H_

#include <string>
#include <vector>

#include "commandmgr.h"

using namespace std;

vector<string> findSuggestion(Workspace ws,
                      const string &input,
                      Command *rootCommand,
                      const vector<unique_ptr<Command>> &DICTIONARY);

vector<string> createSuggestions(Workspace ws,
                                 const string &str,
                                 Command *rootCommand,
                                 const vector<unique_ptr<Command>> &DICTIONARY);

#endif //SHUFFLE_INCLUDE_SUGGESTION_H_
