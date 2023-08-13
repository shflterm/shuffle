#include <kubazip/zip/zip.h>
#include <filesystem>

#include "utils.h"
#include "console.h"
#include "term.h"

using std::vector, std::string, std::filesystem::exists, std::filesystem::create_directories, std::filesystem::remove, std::filesystem::path, std::filesystem::temp_directory_path;

string SHUFFLE = "   _____ _    _ _    _ ______ ______ _      ______           __        ___  \n"
                 "  / ____| |  | | |  | |  ____|  ____| |    |  ____|         /_ |      / _ \\ \n"
                 " | (___ | |__| | |  | | |__  | |__  | |    | |__    __   __  | |     | | | |\n"
                 "  \\___ \\|  __  | |  | |  __| |  __| | |    |  __|   \\ \\ / /  | |     | | | |\n"
                 "  ____) | |  | | |__| | |    | |    | |____| |____   \\ V /   | |  _  | |_| |\n"
                 " |_____/|_|  |_|\\____/|_|    |_|    |______|______|   \\_/    |_| (_)  \\___/ \n";

void deleteShuffleExecutable(path updatePath) {
    if (exists(updatePath.string() + "/shuffle.exe")) remove(updatePath.string() + "/shuffle.exe");
    if (exists(updatePath.string() + "/shuffle")) remove(updatePath.string() + "/shuffle");
}

bool installNativeApps() {
    vector<string> apps;
    string repo = "https://raw.githubusercontent.com/shflterm/shuffle/main/nativeApps.json";

    Json::Value root;
    Json::Reader reader;
    reader.parse(readTextFromWeb(repo), root, false);

    bool appInstalled = true;
    for (const auto &item: root) {
#if defined(__linux__) || defined(__APPLE__)
        system(("chmod +x " + DOT_SHUFFLE + "/bin/shuffle").c_str());
        system((DOT_SHUFFLE + "/bin/shuffle appmgr -add " + item.asString()).c_str());
#endif
        int status = system((DOT_SHUFFLE + "/bin/shuffle appmgr -add " + item.asString()).c_str());
        if (status != 0) appInstalled = false;
    }
    return appInstalled;
}

int main(int argc, char *argv[]) {
    term << SHUFFLE << newLine
         << "Installing Shuffle..." << newLine;
    string latest = trim(readTextFromWeb("https://raw.githubusercontent.com/shflterm/shuffle/main/LATEST"));
#ifdef _WIN32
    string url = "https://github.com/shflterm/shuffle/releases/download/" + latest + "/bin-windows.zip";
#elif defined(__linux__)
    string url = "https://github.com/shflterm/shuffle/releases/download/" + latest + "/bin-linux.zip";
#elif defined(__APPLE__)
    string url = "https://github.com/shflterm/shuffle/releases/download/" + latest + "/bin-macos.zip";
#endif

    path updatePath(DOT_SHUFFLE + "/bin");
    deleteShuffleExecutable(updatePath);

    create_directories(updatePath);

    path temp = temp_directory_path().append("shfl.zip");
    if (!downloadFile(url, temp.string())) {
        error("Failed to download Shuffle.");
        return 1;
    }
    term << teleport(0, wherey() - 1) << eraseLine << "Installed!" << newLine;

    extractZip(temp, updatePath);
    term << teleport(0, wherey() - 1) << eraseLine << "Extracted!" << newLine;

    term << teleport(0, wherey() - 1) << eraseLine << "Install native apps.." << newLine;
    if (installNativeApps()) {
        term << teleport(0, wherey() - 1) << eraseLine << "Shuffle has been successfully installed!";
    } else {
        term << teleport(0, wherey() - 1) << eraseLine
             << "Shuffle is installed, but the some default app is not installed"
             << newLine
             << "After running Shuffle, you may need to manually install the default apps.";
    }
}
