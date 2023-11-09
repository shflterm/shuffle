#include "credit.h"

#include <string>
#include <sstream>
#include "version.h"
#include "utils.h"
#include "term.h"
#include "json/json.h"

using std::endl;

map<string, vector<string>> getCredits() {
    map<string, vector<string>> credits;
    string repo = "https://raw.githubusercontent.com/shflterm/shuffle/main/credits.json";

    Json::Value root;
    Json::Reader reader;
    reader.parse(readTextFromWeb(repo), root, false);

    credits["created_by"] = {};
    for (const auto &item: root["created_by"]) {
        credits["created_by"].push_back(item.asString());
    }
    credits["contributors"] = {};
    for (const auto &item: root["contributors"]) {
        credits["contributors"].push_back(item.asString());
    }
    credits["special_thanks"] = {};
    for (const auto &item: root["special_thanks"]) {
        credits["special_thanks"].push_back(item.asString());
    }
    credits["libraries"] = {};
    for (const auto &item: root["libraries"]) {
        credits["libraries"].push_back(item["name"].asString() + " by " + item["by"].asString());
    }

    return credits;
}

string createCreditText() {
    stringstream ss;
    ss << color(FOREGROUND, Yellow) << "Shuffle " << SHUFFLE_VERSION.str() << endl;
    ss << "Github Repository: https://github.com/shflterm/shuffle" << endl;
    ss << endl;

    auto credits = getCredits();

    ss << color(FOREGROUND, Yellow) << "Created by: " << endl;
    for (const auto &item: credits["created_by"])
        ss << resetColor << "  - " << replace(item, "@", "https://github.com/") << endl;

    ss << color(FOREGROUND, Green) << "Contributors: " << endl;
    for (const auto &item: credits["contributors"])
        ss << resetColor << "  - " << replace(item, "@", "https://github.com/") << endl;

    ss << color(FOREGROUND, Blue) << "Special Thanks: " << endl;
    for (const auto &item: credits["special_thanks"])
        ss << resetColor << "  - " << item << endl;

    ss << color(FOREGROUND, Cyan) << "Open Source Libraries: " << endl;
    for (const auto &item: credits["libraries"])
        ss << resetColor << "  - " << item << endl;

    ss << endl;

    ss << color(FOREGROUND_BRIGHT, Yellow) << "Thank you for using Shuffle!" << endl;
    ss << color(FOREGROUND_BRIGHT, Black)
       << "If you have any issues, please let us know here: https://github.com/shflterm/shuffle/issues/new" << endl;

    return ss.str();
}