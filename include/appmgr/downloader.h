//
// Created by winch on 5/14/2023.
//

#ifndef SHUFFLE_INCLUDE_SAPP_DOWNLOADER_H_
#define SHUFFLE_INCLUDE_SAPP_DOWNLOADER_H_

#include <string>

using std::string;

namespace appmgr {
    bool installApp(string&name);

    bool removeApp(const string&name);

    bool addRepo(const string&url);

    bool removeRepo(const string&url);
}

#endif //SHUFFLE_INCLUDE_SAPP_DOWNLOADER_H_
