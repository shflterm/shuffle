#include <iostream>
#include <filesystem>
#include <json/json.h>

#include "utils/utils.h"
#include "utils/console.h"
#include "appmgr/downloader.h"

using std::cout, std::endl, std::vector, std::string, std::filesystem::exists, std::filesystem::create_directories,
        std::filesystem::remove,
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
        string name = item.asString();
        if (!appmgr::installApp(name, false)) appInstalled = false;
    }
    return appInstalled;
}

int main(int argc, char** argv) {
    cout << SHUFFLE << endl << "Installing Shuffle..." << endl;
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
    cout << teleport(0, wherey() - 1) << erase_line << "Installed!" << endl;

    if (extractZip(temp, updatePath) == "") {
        error("Failed to extract Shuffle.");
        return 1;
    }
    cout << teleport(0, wherey() - 1) << erase_cursor_to_end << "Extracted!" << endl;

    cout << erase_line << "Install native apps.." << endl;
    const bool appInstalled = installNativeApps();
    if (!appInstalled) {
        cout << teleport(0, wherey() - 1) << erase_line
                << "Shuffle is installed, but the some default appmgr is not installed"
                << endl
                << "After running Shuffle, you may need to manually install the default apps.";
        return 1;
    }
    cout << teleport(0, wherey() - 1) << erase_line << "Shuffle has been successfully installed!";
    return 0;
}
