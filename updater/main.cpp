#include <kubazip/zip/zip.h>
#include <filesystem>

#include "utils/utils.h"
#include "console.h"
#include "term.h"

using namespace std;
using namespace std::filesystem;

int onExtractEntry(const char *filename, void *arg) {
    string name = path(filename).filename().string();
    if (!name.empty()) {
        term << teleport(0, wherey()) << eraseLine << "Extracting... (" << name << ")" << newLine;
    }
    return 0;
}

string SHUFFLE = "   _____ _    _ _    _ ______ ______ _      ______           __        ___  \n"
                 "  / ____| |  | | |  | |  ____|  ____| |    |  ____|         /_ |      / _ \\ \n"
                 " | (___ | |__| | |  | | |__  | |__  | |    | |__    __   __  | |     | | | |\n"
                 "  \\___ \\|  __  | |  | |  __| |  __| | |    |  __|   \\ \\ / /  | |     | | | |\n"
                 "  ____) | |  | | |__| | |    | |    | |____| |____   \\ V /   | |  _  | |_| |\n"
                 " |_____/|_|  |_|\\____/|_|    |_|    |______|______|   \\_/    |_| (_)  \\___/ \n";

int main(int argc, char *argv[]) {
    term << SHUFFLE << newLine
         << "Installing Shuffle..." << newLine;
    string latest = trim(readTextFromWeb("https://raw.githubusercontent.com/shflterm/shuffle/main/LATEST"));
//    string url = "https://github.com/shflterm/shuffle/releases/download/" + latest + "/bin.zip";
    string url = "https://github.com/shflterm/shuffle/releases/download/beta%2F4-hotfix/dotshuffle.zip";

    path updatePath(DOT_SHUFFLE + "/bin");
    if (exists(updatePath.string() + "/shuffle.exe")) {
        remove(updatePath.string() + "/shuffle.exe");
    }
    if (exists(updatePath.string() + "/shuffle")) {
        remove(updatePath.string() + "/shuffle");
    }
    create_directories(updatePath);

    path temp = temp_directory_path().append("shfl.zip");
    downloadFile(url, temp.string());
    term << teleport(0, wherey()) << eraseLine << "Installed!" << newLine;
    int arg = 0;
    zip_extract(temp.string().c_str(), updatePath.string().c_str(), onExtractEntry, &arg);
    term << teleport(0, wherey()) << eraseLine << "Extracted!" << newLine;

    term << teleport(0, wherey()) << eraseLine << "Install native apps.." << newLine;
#if defined(__linux__) || defined(__APPLE__)
    system(("chmod +x " + DOT_SHUFFLE + "/bin/shuffle").c_str());
#endif
    int status = system((DOT_SHUFFLE + "/bin/shuffle shfl apps add cd cp dir list mk mv rm").c_str());

    if (status == 0) {
        term << teleport(0, wherey()) << eraseLine << "Shuffle has been successfully installed!";
    } else {
        term << teleport(0, wherey()) << eraseLine << "Shuffle is installed, but the default app is not installed"
             << newLine
             << "After running Shuffle, you may need to manually install the default apps.";
    }
}