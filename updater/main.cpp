#include <kubazip/zip/zip.h>
#include <filesystem>

#include "utils.h"
#include "console.h"
#include "term.h"

using std::vector, std::string, std::filesystem::exists, std::filesystem::create_directories, std::filesystem::remove,
        std::filesystem::path, std::filesystem::temp_directory_path;

string SHUFFLE = "   _____ _    _ _    _ ______ ______ _      ______           __        ___  \n"
        "  / ____| |  | | |  | |  ____|  ____| |    |  ____|         /_ |      / _ \\ \n"
        " | (___ | |__| | |  | | |__  | |__  | |    | |__    __   __  | |     | | | |\n"
        "  \\___ \\|  __  | |  | |  __| |  __| | |    |  __|   \\ \\ / /  | |     | | | |\n"
        "  ____) | |  | | |__| | |    | |    | |____| |____   \\ V /   | |  _  | |_| |\n"
        " |_____/|_|  |_|\\____/|_|    |_|    |______|______|   \\_/    |_| (_)  \\___/ \n";

void deleteShuffleExecutable(const path&updatePath) {
    if (exists(updatePath.string() + "/shuffle.exe")) remove(updatePath.string() + "/shuffle.exe");
    if (exists(updatePath.string() + "/shuffle")) remove(updatePath.string() + "/shuffle");
}

bool installNativeApps() {
    vector<string> apps;
    const string repo = "https://raw.githubusercontent.com/shflterm/shuffle/main/nativeApps.json";

    Json::Value root;
    Json::Reader reader;
    reader.parse(readTextFromWeb(repo), root, false);

    bool appInstalled = true;
    for (const auto&item: root) {
#if defined(__linux__) || defined(__APPLE__)
        system(("chmod +x " + DOT_SHUFFLE.string() + "/bin/shuffle").c_str());
        system((DOT_SHUFFLE.string() + "/bin/shuffle appmgr add " + item.asString()).c_str());
#endif
        if (const int status = system(((DOT_SHUFFLE / "bin/shuffle").string() + " appmgr add " + item.asString()).c_str());
            status != 0)
            appInstalled = false;
    }
    return appInstalled;
}

int main() {
    cout << SHUFFLE << endl
            << "Installing Shuffle..." << endl;
    string latest = trim(readTextFromWeb("https://raw.githubusercontent.com/shflterm/shuffle/main/LATEST"));
#ifdef _WIN32
    string url = "https://github.com/shflterm/shuffle/releases/download/" + latest + "/bin-windows.zip";
#elif defined(__linux__)
    string url = "https://github.com/shflterm/shuffle/releases/download/" + latest + "/bin-linux.zip";
#elif defined(__APPLE__)
    string url = "https://github.com/shflterm/shuffle/releases/download/" + latest + "/bin-macos.zip";
#endif

    const path updatePath(DOT_SHUFFLE / "bin");
    deleteShuffleExecutable(updatePath);

    create_directories(updatePath);

    const path temp = temp_directory_path().append("shfl.zip");
    if (!downloadFile(url, temp.string())) {
        error("Failed to download Shuffle.");
        return 1;
    }
    cout << teleport(0, wherey() - 1) << ERASE_LINE << "Installed!" << endl;

    extractZip(temp, updatePath);
    cout << teleport(0, wherey() - 1) << ERASE_LINE << "Extracted!" << endl;

    cout << teleport(0, wherey() - 1) << ERASE_LINE << "Install native apps.." << endl;
    if (installNativeApps()) {
        cout << teleport(0, wherey() - 1) << ERASE_LINE << "Shuffle has been successfully installed!";
    }
    else {
        cout << teleport(0, wherey() - 1) << ERASE_LINE
                << "Shuffle is installed, but the some default app is not installed"
                << endl
                << "After running Shuffle, you may need to manually install the default apps.";
    }
}
