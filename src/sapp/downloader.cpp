#include "sapp/downloader.h"

#include <string>
#include <cstdio>
#include <curl/curl.h>
#include <json/json.h>
#include <filesystem>
#include <kubazip/zip/zip.h>

#include "console.h"
#include "utils/utils.h"
#include "commandmgr.h"
#include "term.h"

using namespace std;
using namespace std::filesystem;

Json::Value getRepo() {
  string repo = "https://raw.githubusercontent.com/shflterm/apps/main/repo.json";

  Json::Value root;
  Json::Reader reader;
  reader.parse(readTextFromWeb(repo), root, false);

  return root;
}

int _onExtractEntry(const char *filename, void *arg) {
  info("Extracted: " + absolute(path(filename)).string());
  return 0;
}

void addSAPP(const string &name) {
  info("Downloading Repository information...");

  Json::Value repo = getRepo();

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
      term << eraseFromCursorToLineEnd;
      info("Download Completed!");
    } else {
      error("An error occurred while downloading the app. (Please double check the name of the app to be installed.)");
      return;
    }
  } else {
    error("Unknown repository version: " + to_string(ver));
    return;
  }

  info("Start Extracting..");

  int arg = 2;
  zip_extract(downloadTo.c_str(), (DOT_SHUFFLE + "/apps/" + name + ".app").c_str(), _onExtractEntry, &arg);

  info("Adding to config...");

  addRegisteredCommand({name, name});

  success("Done!");
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