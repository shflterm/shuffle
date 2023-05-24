#include <iostream>
#include <kubazip/zip/zip.h>
#include <filesystem>

#include "utils/utils.h"
#include "console.h"
#include "version.h"

using namespace std;
using namespace std::filesystem;

int onExtractEntry(const char *filename, void *arg) {
  string name = path(filename).filename().string();
  if (!name.empty()) {
    eraseLine();
    info("Extracting... (" + name + ")");
    gotoxy(wherex(), wherey() - 1);
  }
  return 0;
}

int main(int argc, char *argv[]) {
  string latest = trim(readTextFromWeb("https://raw.githubusercontent.com/shflterm/shuffle/main/LATEST"));
  if (latest != SHUFFLE_VERSION.to_string()) {
    cout << "New version available: " << SHUFFLE_VERSION.to_string() << " -> " << latest << endl;
    cout << "Updating..." << endl;
    string url = "https://github.com/shflterm/shuffle/releases/download/" + latest + "/bin.zip";

    path updatePath(DOT_SHUFFLE + "/bin");
    remove(updatePath.string() + "/shuffle.exe");
    create_directories(updatePath);

    path temp = temp_directory_path().append("shfl.zip");
    downloadFile(url, temp.string());
    int arg = 0;
    zip_extract(temp.string().c_str(), updatePath.string().c_str(), onExtractEntry, &arg);

    eraseLine();
    cout << "Completed! Press Any Key To Exit." << endl;
    readChar();
  } else {
    cout << "You are using the latest version of Shuffle." << endl;
  }
}