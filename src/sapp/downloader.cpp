#include "sapp/downloader.h"

#include <string>
#include <curl/curl.h>
#include <json/json.h>
#include <filesystem>
#include <kubazip/zip/zip.h>

#include "console.h"
#include "utils/utils.h"

using namespace std;
using namespace std::filesystem;

size_t writeText(void* ptr, size_t size, size_t nmemb, std::string* data) {
  data->append((char*)ptr, size * nmemb);
  return size * nmemb;
}

size_t writeFile(void *ptr, size_t size, size_t nmemb, FILE *stream) {
  size_t written = fwrite(ptr, size, nmemb, stream);
  return written;
}

Json::Value getRepo() {
  string repo = "https://raw.githubusercontent.com/shflterm/apps/main/repo.json";

  auto curl = curl_easy_init();
  string response;
  if (curl) {
    curl_easy_setopt(curl, CURLOPT_URL, repo.c_str());
    curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
    curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 50L);
    curl_easy_setopt(curl, CURLOPT_TCP_KEEPALIVE, 1L);

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeText);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    curl_easy_perform(curl);

    curl_easy_cleanup(curl);
    curl_global_cleanup();
    curl = nullptr;
  }

  Json::Value root;
  Json::Reader reader;
  reader.parse(response, root, false);

  return root;
}

void downloadFile(const string& url, const string& file) {
  auto curl = curl_easy_init();
  FILE *fp;
  CURLcode res;
  curl = curl_easy_init();
  if (curl) {
    fp = fopen(file.c_str(),"wb");
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, NULL);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
    res = curl_easy_perform(curl);

    curl_easy_cleanup(curl);
    fclose(fp);
  }
}

int onExtractEntry(const char *filename, void *arg) {
  info("Extracted: " + absolute(path(filename)).string();
  return 0;
}

void addSAPP(const string& name) {
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
    downloadFile(downloadFrom, downloadTo);
    info("Download Completed!");
  } else {
    error("Unknown repository version: " + to_string(ver));
    return;
  }

  info("Start Extracting..");

  int arg = 2;
  zip_extract(downloadTo.c_str(), (DOT_SHUFFLE + "/apps/" + name).c_str(), onExtractEntry, &arg);

  info("Extracted!");

  success("Done!");
}