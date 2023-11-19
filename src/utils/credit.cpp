#include "credit.h"

#include <string>
#include <sstream>

#include "version.h"
#include "utils.h"
#include "json/json.h"
#include "console.h"

using std::endl;

map<string, vector<string>> getCredits() {
    map<string, vector<string>> credits;
    const string repo = "https://raw.githubusercontent.com/shflterm/shuffle/main/credits.json";

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
    ss << FG_YELLOW << "Shuffle " << SHUFFLE_VERSION.str() << endl;
    ss << "Github Repository: https://github.com/shflterm/shuffle" << endl;
    ss << endl;

    auto credits = getCredits();

    ss << FG_YELLOW << "Created by: " << endl;
    for (const auto &item: credits["created_by"])
        ss << RESET << "  - " << replace(item, "@", "https://github.com/") << endl;

    ss << FG_GREEN << "Contributors: " << endl;
    for (const auto &item: credits["contributors"])
        ss << RESET << "  - " << replace(item, "@", "https://github.com/") << endl;

    ss << FG_BLUE << "Special Thanks: " << endl;
    for (const auto &item: credits["special_thanks"])
        ss << RESET << "  - " << item << endl;

    ss << FG_CYAN << "Open Source Libraries: " << endl;
    for (const auto &item: credits["libraries"])
        ss << RESET << "  - " << item << endl;

    ss << endl;

    ss << FGB_YELLOW << "Thank you for using Shuffle!" << endl;
    ss << FGB_BLACK
       << "If you have any issues, please let us know here: https://github.com/shflterm/shuffle/issues/new" << endl;

    return ss.str();
}