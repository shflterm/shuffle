#include "appmgr/downloader.h"

#include <iostream>
#include <string>
#include <json/json.h>
#include <filesystem>

#include "utils/console.h"
#include "utils/shfljson.h"
#include "utils/utils.h"
#include "appmgr/appmgr.h"

using std::cout, std::to_string, std::filesystem::path, std::filesystem::temp_directory_path, std::filesystem::exists,
        std::filesystem::remove_all, std::filesystem::copy_options, appmgr::addApp;

namespace appmgr {
    Json::Value getRepo(const string&repo) {
        Json::Value root;
        Json::Reader reader;
        reader.parse(readTextFromWeb(repo), root, false);

        return root;
    }

    vector<string> getRepos() {
        vector<string> result;
        Json::Value repos = getShflJson("repos");
        for (const auto&repo: repos) result.push_back(repo.asString());
        return result;
    }

    bool installApp(string&name, bool checkLocalApp) {
        bool installed = false;

        const path appPath = DOT_SHUFFLE / "apps" / (name + ".shflapp");

        if (checkLocalApp) {
            const path localApp = currentWorkspace->currentDirectory() / name;
            if (const path appShfl = localApp / "app.shfl";
                exists(localApp) && exists(appShfl)) {
                name = path(name).filename().string();
                info("Start downloading '" + name + "' from '" + absolute(localApp).parent_path().string() +
                     "'...");
                copy(absolute(localApp), absolute(appPath), copy_options::overwrite_existing | copy_options::recursive);
                success("Load Completed!");
                installed = true;
            }
        }

        if (!installed) {
            const string downloadTo = temp_directory_path().append("app.shflapp").string();

            info("Downloading Repository information...");
            for (const auto&repoUrl: getRepos()) {
                Json::Value repo = getRepo(repoUrl);

                string message = R"(Start downloading '{APP}' from '{REPO}'...)";
                message = replace(message, "{APP}", name);
                message = replace(message, "{REPO}", repo["repo"].asString());
                info(message);

                if (const int ver = repo["version"].asInt(); ver == 1) {
                    string downloadFrom = repo["download_at"].asString();
                    downloadFrom = replace(downloadFrom, "{APP}", name);
#ifdef _WIN32
                    downloadFrom = replace(downloadFrom, "{OS}", "windows");
#elif defined(__linux__)
                    downloadFrom = replace(downloadFrom, "{OS}", "linux");
#elif defined(__APPLE__)
                    downloadFrom = replace(downloadFrom, "{OS}", "macos");
#endif

                    if (!downloadFile(downloadFrom, downloadTo)) continue;
                    if (extractZip(downloadTo, appPath) == "") continue;
                    installed = true;
                    break;
                }
                else {
                    warning("Unknown repository version: " + to_string(ver));
                }
            }
        }

        if (!installed) {
            error("Failed to download '" + name + "'. (App not found.)");
            return false;
        }

        info("Adding to config...");

        if (!addApp(name)) {
            error("Failed to add appmgr. (The appmgr has already been added.)");
            return false;
        }

        success("Done!");
        return true;
    }

    bool removeApp(const string&name) {
        try {
            info("Deleting '" + name + "'...");

            if (const path appPath = DOT_SHUFFLE / "apps" / (name + ".shflapp"); exists(appPath)) {
                // TODO 지우기 전 라이브러리 언로드
                for (const auto&app: loadedApps)
                    if (app->getName() == name) app->unload();
                remove_all(appPath);

                Json::Value json = getShflJson("apps");
                bool removed = false;
                for (int i = 0; i < json.size(); ++i) {
                    if (json[i].asString() == name) {
                        removed = json.removeIndex(i, &json[i]);
                    }
                }
                if (!removed) {
                    error("Failed to remove app.");
                    return false;
                }
                setShflJson("apps", json);

                success("Done!");
                return true;
            }
            error("App not found!");
            return false;
        }
        catch (std::exception exception) {
            error("Failed to remove app. (Exception: " + string(exception.what()) + ")");
            return false;
        }
    }


    bool addRepo(const string&url) {
        Json::Value repos = getShflJson("repos");
        repos.append(url);
        setShflJson("repos", repos);
        info("Repository " + url + " added!");
        return true;
    }


    bool removeRepo(const string&url) {
        Json::Value repos = getShflJson("repos");
        for (int i = 0; i < repos.size(); ++i) {
            if (repos[i].asString() == url) {
                repos.removeIndex(i, &repos[i]);
            }
        }
        setShflJson("repos", repos);
        info("Repository " + url + " removed!");
        return true;
    }
}
