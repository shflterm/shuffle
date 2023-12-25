//
// Created by 08sih on 2023-11-11.
//

#ifndef APPMGR_H
#define APPMGR_H

#include <vector>
#include <string>
#include <memory>

#include "cmd/commandmgr.h"

using std::vector, std::string, std::shared_ptr, cmd::Command;

namespace appmgr {
    class App {
    protected:
        App(const App&new_parent, const string&name);

        int apiVersion = -1;

    public:
        string name, description, author;
        string version;
        vector<shared_ptr<Command>> commands;
        vector<std::function<void()>> onUnload;

        explicit App(const string&name);

        App(string name, string description, string author, string version, vector<shared_ptr<Command>> commands);

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

        void unload() const;
    };

    extern vector<shared_ptr<App>> loadedApps;

    vector<shared_ptr<Command>> getCommands();

    void loadApp(const shared_ptr<App>& app);

    bool addApp(const string&name);

    void unloadAllApps();

    vector<string> getApps();
}

#endif //APPMGR_H
