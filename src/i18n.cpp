#include <string>
#include <initializer_list>

#include "json/json.h"
#include "utils.cpp"

using namespace std;

string langJson;

void loadLanguageFile() {
  langJson = read_file("../lang/en_us.json");
}

string translate(const string &translate_code, const initializer_list<string> &args) {
  Json::Value root;
  Json::Reader reader;
  reader.parse(langJson, root, false);
  string str = root[translate_code].asString();

  int i = 0;
  for (const string& elem : args) {
    str = replace(str, "$" + to_string(i), elem);
    i++;
  }

  return str;
}

string translate(const string &translate_code) {
  return translate(translate_code, {});
}