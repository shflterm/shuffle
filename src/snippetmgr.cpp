//
// Created by 08sih on 6/25/2023.
//

#include "snippetmgr.h"

#include <utility>
#include <json/json.h>

#include "utils/utils.h"

vector<shared_ptr<Snippet>> snippets;

const string &Snippet::getName() const {
    return name;
}

const string &Snippet::getTarget() const {
    return target;
}

Snippet::Snippet(string name, string target) : name(std::move(name)), target(std::move(target)) {}

vector<Snippet> getSnippets() {
    vector<Snippet> res;

    Json::Value commandList = getShflJson("snippets");
    for (auto command: commandList) {
        Snippet data = Snippet(command["name"].asString(), command["target"].asString());
        res.push_back(data);
    }

    return res;
}

bool addSnippet(const string &name, const string &target) {
    snippets.push_back(make_shared<Snippet>(Snippet(name, target)));

    Json::Value snippetsJson = getShflJson("snippets");
    Json::Value newSnippet;
    newSnippet["name"] = name;
    newSnippet["target"] = target;
    snippetsJson.append(newSnippet);
    setShflJson("snippets", snippetsJson);
    return true;
}

void loadSnippets() {
    snippets.clear();

    for (const Snippet &command: getSnippets()) {
        snippets.push_back(make_shared<Snippet>(command));
    }
}