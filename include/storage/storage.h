#ifndef STORAGE_H
#define STORAGE_H

#include <string>
#include <any>

#include "utils/shfljson.h"

using std::any, std::string;

any getData(const string&name);

void setData(const string&name, int value);

void setData(const string&name, double value);

void setData(const string&name, const string&value);

#endif //STORAGE_H
