//
// Created by 08sih on 6/25/2023.
//

#ifndef SHFL_SNIPPETMGR_H
#define SHFL_SNIPPETMGR_H

#include <string>
#include <vector>
#include <memory>

using std::string, std::vector, std::shared_ptr;

class Snippet {
protected:
    string name;
    string target;

public:
    [[nodiscard]] const string &getName() const;

    [[nodiscard]] const string &getTarget() const;

    Snippet(string name, string target);
};

extern vector <shared_ptr<Snippet>> snippets;

vector <Snippet> getSnippets();

bool addSnippet(const string &name, const string &target);

void loadSnippets();

#endif //SHFL_SNIPPETMGR_H
