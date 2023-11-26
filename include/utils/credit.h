//
// Created by winch on 6/16/2023.
//

#ifndef SHFL_INCLUDE_UTILS_CREDIT_H_
#define SHFL_INCLUDE_UTILS_CREDIT_H_

#include <vector>
#include <map>
#include <string>

using std::string, std::vector, std::map;

map<string, vector<string>> getCredits();

string createCreditText();

#endif //SHFL_INCLUDE_UTILS_CREDIT_H_
