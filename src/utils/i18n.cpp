#include "utils/i18n.h"

#include <string>
#include <initializer_list>

#include <json/json.h>
#include "utils/utils.h"
#include "utils/console.h"

using std::map, std::initializer_list, std::to_string, std::string;

string langJson;
map<string, string> colorMap;

string translate(const string&translate_code, const initializer_list<string>&args) {
    if (colorMap.empty()) {
        colorMap = {
            {"RESET", reset},
            //    {"BOLD", bold},
            //    {"ITALIC", italic},
            //    {"UNDERLINE", underline},
            {"FG_BLACK", fg_black},
            {"FG_RED", fg_red},
            {"FG_GREEN", fg_green},
            {"FG_YELLOW", fg_yellow},
            {"FG_BLUE", fg_blue},
            {"FG_MAGENTA", fg_magenta},
            {"FG_CYAN", fg_cyan},
            {"FG_WHITE", fg_white},
            {"BG_BLACK", bg_black},
            {"BG_RED", bg_red},
            {"BG_GREEN", bg_green},
            {"BG_YELLOW", bg_yellow},
            {"BG_BLUE", bg_blue},
            {"BG_MAGENTA", bg_magenta},
            {"BG_CYAN", bg_cyan},
            {"BG_WHITE", bg_white},
        };
    }

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
