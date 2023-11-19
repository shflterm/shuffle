#include "i18n.h"

#include <string>
#include <initializer_list>

#include "json/json.h"
#include "utils.h"
#include "console.h"

using std::map, std::initializer_list, std::to_string, std::string;

string langJson;
map<string, string> colorMap = {
    {"RESET", RESET},
    //    {"BOLD", bold},
    //    {"ITALIC", italic},
    //    {"UNDERLINE", underline},
    {"FG_BLACK", FG_BLACK},
    {"FG_RED", FG_RED},
    {"FG_GREEN", FG_GREEN},
    {"FG_YELLOW", FG_YELLOW},
    {"FG_BLUE", FG_BLUE},
    {"FG_MAGENTA", FG_MAGENTA},
    {"FG_CYAN", FG_CYAN},
    {"FG_WHITE", FG_WHITE},
    {"BG_BLACK", BG_BLACK},
    {"BG_RED", BG_RED},
    {"BG_GREEN", BG_GREEN},
    {"BG_YELLOW", BG_YELLOW},
    {"BG_BLUE", BG_BLUE},
    {"BG_MAGENTA", BG_MAGENTA},
    {"BG_CYAN", BG_CYAN},
    {"BG_WHITE", BG_WHITE},
};

string translate(const string&translate_code, const initializer_list<string>&args) {
    Json::Value root;
    Json::Reader reader;
    reader.parse(langJson, root, false);
    string str = root[translate_code].asString();
    if (str.empty()) str = translate_code;

    int i = 0;
    for (const string&elem: args) {
        str = replace(str, "$" + to_string(i), elem);
        i++;
    }

    for (const auto&[name, value]: colorMap) {
        str = replace(str, "{" + name + "}", value);
    }

    return str;
}

string translate(const string&translate_code) {
    return translate(translate_code, {});
}
