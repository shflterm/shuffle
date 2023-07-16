#include "sapp/downloader.h"

#include <string>
#include <json/json.h>
#include <filesystem>
#include <kubazip/zip/zip.h>

#include "console.h"
#include "utils/utils.h"
#include "cmd/commandmgr.h"
#include "term.h"

using std::to_string, std::filesystem::temp_directory_path, std::filesystem::exists, std::filesystem::remove_all;

Json::Value getRepo() {
    string repo = "https://raw.githubusercontent.com/shflterm/apps/main/repo.json";

    Json::Value root;
    Json::Reader reader;
    reader.parse(readTextFromWeb(repo), root, false);

    return root;
}

int _onExtractEntry(const char *filename, void *arg) {
    string name = path(filename).filename().string();
    if (!name.empty()) {
        term << teleport(0, wherey()) << eraseLine << "Extracting... (" << name << ")";
    }
    return 0;
}

void addSAPP(const string &name) {
    info("Downloading Repository information...");

    Json::Value repo = getRepo();

    term << teleport(0, wherey() - 1) << eraseLine;
    string message = R"(Start downloading '{APP}' from '{REPO}'...)";
    message = replace(message, "{APP}", name);
    message = replace(message, "{REPO}", repo["repo"].asString());
    info(message);

    int ver = repo["version"].asInt();
    string downloadTo = temp_directory_path().append("app.sapp").string();
    if (ver == 1) {
        string downloadFrom = replace(repo["download_at"].asString(), "{APP}", name);
        bool res = downloadFile(downloadFrom, downloadTo);
        if (res) {
            term << teleport(0, wherey() - 1) << eraseLine;
            success("Download Completed!");
        } else {
            term << teleport(0, wherey() - 1) << eraseLine;
            error("The app could not be found in the repository.");
            return;
        }
    } else {
        term << teleport(0, wherey() - 1) << eraseLine;
        error("Unknown repository version: " + to_string(ver));
        return;
    }

    term << teleport(0, wherey() - 1) << eraseLine;
    int arg = 2;
    zip_extract(downloadTo.c_str(), (DOT_SHUFFLE + "/apps/" + name + ".app").c_str(), _onExtractEntry, &arg);
    term << teleport(0, wherey()) << eraseLine;
    success("Extracted!");

    term << teleport(0, wherey() - 1) << eraseLine;
    info("Adding to config...");

    if (!addRegisteredCommand({name})) {
        term << teleport(0, wherey() - 1) << eraseLine;
        error("Failed to add app. (The app has already been added.)");
    } else {
        term << teleport(0, wherey() - 1) << eraseLine;
        success("Done!");
    }
}

void removeSAPP(const string &name) {
    info("Deleting '" + name + "'...");

    string path = DOT_SHUFFLE + "/apps/" + name + ".app";
    if (exists(path)) {
        remove_all(path);

        Json::Value json = getShflJson("apps");
        for (int i = 0; i < json.size(); ++i) {
            if (json[i]["name"] == name) {
                json.removeIndex(i, &json[i]);
            }
        }
        setShflJson("apps", json);

        success("Done!");
    } else {
        error("App not found!");
    }
}