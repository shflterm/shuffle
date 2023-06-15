//
// Created by winch on 6/16/2023.
//

#ifndef SHFL_INCLUDE_UTILS_CREDIT_H_
#define SHFL_INCLUDE_UTILS_CREDIT_H_

#include <vector>
#include <string>
#include <algorithm>

using namespace std;

#define CREATED_BY vector<string>({"@ssyuk"})
#define CONTRIBUTORS vector<string>({"@devproje"})
#define SPECIAL_THANKS vector<string>({"@NY0510"})
#define LIBRARIES vector<pair<string,string>>({ \
  {"JsonCpp", "@open-source-parsers"},          \
  {"curl", "https://curl.se"},                  \
  {"zip", "@kuba--"}                            \
})


string createCreditText();

#endif //SHFL_INCLUDE_UTILS_CREDIT_H_
