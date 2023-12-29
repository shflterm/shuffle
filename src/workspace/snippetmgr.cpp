#include "workspace/snippetmgr.h"

#include <utility>
#include "json/json.h"

#include "utils/shfljson.h"

using std::vector, std::make_shared;

vector<shared_ptr<Snippet>> snippets;

const string& Snippet::getName() const {
    return name;
}

const string& Snippet::getTarget() const {
    return target;
}

Snippet::Snippet(string name, string target) : name(std::move(name)), target(std::move(target)) {
}

vector<Snippet> getSnippets() {
    vector<Snippet> res;

    Json::Value commandList = getShflJson("snippets");
    for (auto command: commandList) {
        auto data = Snippet(command["name"].asString(), command["target"].asString());
        res.push_back(data);
    }

    return res;
}

bool addSnippet(const string&name, const string&target) {
    snippets.push_back(make_shared<Snippet>(Snippet(name, target)));

    Json::Value snippetsJson = getShflJson("snippets");

    for (auto snippet: snippetsJson) {
        if (snippet["name"].asString() == name) return false;
    }

    Json::Value newSnippet;
    newSnippet["name"] = name;
    newSnippet["target"] = target;
    snippetsJson.append(newSnippet);
    setShflJson("snippets", snippetsJson);
    return true;
}

bool removeSnippet(const string&name) {
    snippets.erase(std::remove_if(snippets.begin(), snippets.end(), [name](const shared_ptr<Snippet>&t) {
        return t->getName() == name;
    }), snippets.end());

    Json::Value snippetsJson = getShflJson("snippets");

    for (int i = 0; i < snippetsJson.size(); ++i) {
        if (auto snippet = snippetsJson[i];
            snippet["name"].asString() == name) {
            return snippetsJson.removeIndex(i, &snippet);
        }
    }

    return false;
}

void loadSnippets() {
    snippets.clear();

    for (const Snippet&command: getSnippets()) {
        snippets.push_back(make_shared<Snippet>(command));
    }
}
