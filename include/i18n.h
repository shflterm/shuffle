//
// Created by winch on 5/5/2023.
//

#ifndef SHUFFLE_INCLUDE_I18N_H_
#define SHUFFLE_INCLUDE_I18N_H_

#include <string>
#include <initializer_list>

using namespace std;

void loadLanguageFile(const string &region);

string translate(const string &translate_code, const initializer_list<string> &args);

string translate(const string &translate_code);

#endif //SHUFFLE_INCLUDE_I18N_H_
