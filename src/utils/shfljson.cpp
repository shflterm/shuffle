#include "utils/shfljson.h"

#include <filesystem>
#include <utils/utils.h>

using std::filesystem::exists, std::filesystem::path;

void initShflJson() {
    if (!exists(path(SHFL_JSON))) {
        writeFile(SHFL_JSON, R"({"apps": [], "libs": []})");
    }
}

Json::Value getShflJson(const string&part) {
    Json::Value root;
    Json::Reader reader;
    reader.parse(readFile(SHFL_JSON), root, false);
    return root[part];
}

void setShflJson(const string&part, Json::Value value) {
    Json::Value root;
    Json::Reader reader;
    reader.parse(readFile(SHFL_JSON), root, false);

    root[part] = std::move(value);

    writeFile(SHFL_JSON, root.toStyledString());
}