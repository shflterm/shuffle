//
// Created by 08sih on 6/25/2023.
//

#include "snippetmgr.h"

#include <utility>

vector<shared_ptr<Snippet>> snippets;

const string &Snippet::getName() const {
    return name;
}

const string &Snippet::getTarget() const {
    return target;
}

Snippet::Snippet(string name, string target) : name(std::move(name)), target(std::move(target)) {}
