//
// Created by 08sih on 2023-11-26.
//

#ifndef SHFLJSON_H
#define SHFLJSON_H

#include <json/json.h>
#include <string>

using std::string;

void initShflJson();

Json::Value getShflJson(const string&part);

void setShflJson(const string&part, Json::Value value);

#endif //SHFLJSON_H
