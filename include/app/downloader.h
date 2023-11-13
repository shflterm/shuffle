//
// Created by winch on 5/14/2023.
//

#ifndef SHUFFLE_INCLUDE_SAPP_DOWNLOADER_H_
#define SHUFFLE_INCLUDE_SAPP_DOWNLOADER_H_

#include <string>

using std::string;

void installApp(string&name);

void removeApp(const string&name);

void addRepo(const string&url);

void removeRepo(const string&url);

#endif //SHUFFLE_INCLUDE_SAPP_DOWNLOADER_H_
