#include "downloader.h"

#include <string>
#include <json/json.h>
#include <filesystem>

#include "console.h"
#include "utils.h"
#include "appmgr.h"
#include "term.h"

using std::to_string, std::filesystem::temp_directory_path, std::filesystem::exists, std::filesystem::remove_all;

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

void installApp(const string&name) {
    info("Downloading Repository information...");

    bool appFounded = false;
    const string downloadTo = temp_directory_path().append("app.shflapp").string();

    for (const auto&repoUrl: getRepos()) {
        Json::Value repo = getRepo(repoUrl);

        term << teleport(0, wherey() - 1) << eraseLine;
        string message = R"(Start downloading '{APP}' from '{REPO}'...)";
        message = replace(message, "{APP}", name);
        message = replace(message, "{REPO}", repo["repo"].asString());
        info(message);

        if (const int ver = repo["version"].asInt(); ver == 1) {
            if (const string downloadFrom = replace(repo["download_at"].asString(), "{APP}", name); downloadFile(
                downloadFrom, downloadTo)) {
                term << teleport(0, wherey() - 1) << eraseLine;
                success("Download Completed!");
                appFounded = true;
                break;
            }
        }
        else {
            term << teleport(0, wherey() - 1) << eraseLine;
            error("Unknown repository version: " + to_string(ver));
        }
    }

    if (!appFounded) {
        term << teleport(0, wherey() - 1) << eraseLine;
        error("The app could not be found in the repository.");
        return;
    }

    term << teleport(0, wherey() - 1) << eraseLine;
    extractZip(downloadTo, DOT_SHUFFLE + "/apps/" + name + ".shflapp");
    term << teleport(0, wherey()) << eraseLine;
    success("Extracted!");

    term << teleport(0, wherey() - 1) << eraseLine;
    info("Adding to config...");

    if (!addApp(name)) {
        term << teleport(0, wherey() - 1) << eraseLine;
        error("Failed to add app. (The app has already been added.)");
    }
    else {
        term << teleport(0, wherey() - 1) << eraseLine;
        success("Done!");
    }
}

void removeApp(const string&name) {
    info("Deleting '" + name + "'...");

    if (const string path = DOT_SHUFFLE + "/apps/" + name + ".shflapp"; exists(path)) {
        remove_all(path);

        Json::Value json = getShflJson("apps");
        for (int i = 0; i < json.size(); ++i) {
            if (json[i]["name"] == name) {
                json.removeIndex(i, &json[i]);
            }
        }
        setShflJson("apps", json);

        success("Done!");
    }
    else {
        error("App not found!");
    }
}


void addRepo(const string&url) {
    Json::Value repos = getShflJson("repos");
    repos.append(url);
    setShflJson("repos", repos);
    info("Repository " + url + " added!");
}


void removeRepo(const string&url) {
    Json::Value repos = getShflJson("repos");
    repos.removeMember(url);
    setShflJson("repos", repos);
    info("Repository " + url + " removed!");
}
