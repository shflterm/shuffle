#include <string>
#include <initializer_list>
#include "term.h"

#include "json/json.h"
#include "utils.h"

using std::map, std::initializer_list, std::to_string, std::string;

string langJson;
map<string, string> colorMap = {
        {"RESET", resetColor},
//    {"BOLD", bold},
//    {"ITALIC", italic},
//    {"UNDERLINE", underline},
        {"FG_BLACK",   color(FOREGROUND, Black)},
        {"FG_RED",     color(FOREGROUND, Red)},
        {"FG_GREEN",   color(FOREGROUND, Green)},
        {"FG_YELLOW",  color(FOREGROUND, Yellow)},
        {"FG_BLUE",    color(FOREGROUND, Blue)},
        {"FG_MAGENTA", color(FOREGROUND, Magenta)},
        {"FG_CYAN",    color(FOREGROUND, Cyan)},
        {"FG_WHITE",   color(FOREGROUND, White)},
        {"BG_BLACK",   color(BACKGROUND, Black)},
        {"BG_RED",     color(BACKGROUND, Red)},
        {"BG_GREEN",   color(BACKGROUND, Green)},
        {"BG_YELLOW",  color(BACKGROUND, Yellow)},
        {"BG_BLUE",    color(BACKGROUND, Blue)},
        {"BG_MAGENTA", color(BACKGROUND, Magenta)},
        {"BG_CYAN",    color(BACKGROUND, Cyan)},
        {"BG_WHITE",   color(BACKGROUND, White)},
};

void loadLanguageFile(const string &region) {
    langJson = readFile(DOT_SHUFFLE + "/lang/" + region + ".json");
}

string translate(const string &translate_code, const initializer_list<string> &args) {
    Json::Value root;
    Json::Reader reader;
    reader.parse(langJson, root, false);
    string str = root[translate_code].asString();
    if (str.empty()) str = translate_code;

    int i = 0;
    for (const string &elem: args) {
        str = replace(str, "$" + to_string(i), elem);
        i++;
    }

    for (const auto &item: colorMap) {
        str = replace(str, "{" + item.first + "}", item.second);
    }

    return str;
}

string translate(const string &translate_code) {
    return translate(translate_code, {});
}