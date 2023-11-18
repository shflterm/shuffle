//
// Created by winch on 5/5/2023.
//

#ifndef SHUFFLE_INCLUDE_SUGGESTION_H_
#define SHUFFLE_INCLUDE_SUGGESTION_H_

#include <string>
#include <vector>
#include <workspace.h>

using std::string, std::vector;

vector<string> findSuggestion(const Workspace&ws,
                              const string&input,
                              const vector<string>&DICTIONARY);

vector<string> createSuggestions(const Workspace&ws,
                                 const string&str,
                                 const vector<string>&DICTIONARY);

#endif //SHUFFLE_INCLUDE_SUGGESTION_H_
