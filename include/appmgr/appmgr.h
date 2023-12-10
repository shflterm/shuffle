//
// Created by 08sih on 2023-11-11.
//

#ifndef APPMGR_H
#define APPMGR_H

#include "utils/utils.h"

#include <vector>
#include <string>
#include <memory>

using std::vector, std::string, std::shared_ptr;

namespace appmgr {
    class App {
    protected:
        App(const App&new_parent, const string&name);

        int apiVersion = -1;

    public:
        string name, description, author;
        string version;

        explicit App(const string&name);

        [[nodiscard]] string getName() const {
            return name;
        }

        [[nodiscard]] string getDescription() const {
            return description;
        }

        [[nodiscard]] string getAuthor() const {
            return author;
        }

        [[nodiscard]] string getVersion() const {
            return version;
        }

        [[nodiscard]] int getApiVersion() const {
            return apiVersion;
        }
    };

    extern vector<shared_ptr<App>> loadedApps;

    void loadApp(const string&name);

    bool addApp(const string&name);

    void unloadAllApps();

    vector<string> getApps();
}

#endif //APPMGR_H
