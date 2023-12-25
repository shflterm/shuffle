#include "storage/storage.h"

#include <string>
#include <any>
#include <json/json.h>

#include "utils/shfljson.h"

using std::any, std::string;

any getData(const string&name) {
    Json::Value json = getShflJson("data");
    return json[name];
}

void setData(const string&name, int value) {
    Json::Value json = getShflJson("data");
    json[name] = value;
    setShflJson("data", json);
}

void setData(const string&name, const double value) {
    Json::Value json = getShflJson("data");
    json[name] = value;
    setShflJson("data", json);
}

void setData(const string&name, const string&value) {
    Json::Value json = getShflJson("data");
    json[name] = value;
    setShflJson("data", json);
}