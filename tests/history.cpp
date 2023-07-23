#include "suggestion.h"
#include "utils/utils.h"
#include "term.h"
#include "workspace.h"

#include <iostream>

using namespace std;

vector<string> makeDictionary_(const vector<shared_ptr<Command>> &cmds) {
    vector<string> dictionary;
    dictionary.reserve(cmds.size());
    for (const auto &item: cmds) {
        dictionary.push_back(item->getName());
    }
    return dictionary;
}

int main() {
    Workspace ws;
    ws.addHistory("a");
    ws.addHistory("b");
    ws.addHistory("c");

    cout << ws.historyUp() << endl;
    cout << ws.historyDown() << endl;
    cout << ws.historyUp() << endl;
    ws.addHistory("d");
    cout << ws.historyDown() << endl;
    return 0;
}