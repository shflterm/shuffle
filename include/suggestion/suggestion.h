//
// Created by winch on 5/5/2023.
//

#ifndef SHUFFLE_INCLUDE_SUGGESTION_H_
#define SHUFFLE_INCLUDE_SUGGESTION_H_

#include <string>
#include <vector>
#include <workspace.h>

using std::string, std::vector;

namespace suggestion {
    string getSuggestion(Workspace&ws, const string&input);

    vector<string> findSuggestion(const Workspace&ws,
                                  const string&input,
                                  const vector<string>&DICTIONARY);

    string getHint(Workspace ws, const string&input);
}

#endif //SHUFFLE_INCLUDE_SUGGESTION_H_
