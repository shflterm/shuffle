#define NOMINMAX

#include "utils/utils.h"

#include <iostream>
#include <vector>
#include <string>
#include <regex>
#include <algorithm>
#include <fstream>
#include <cstdlib>
#include <random>
#include <curl/curl.h>
#include <restclient-cpp/restclient.h>
#include <zip/zip.h>

#if defined(__linux__) || defined(__APPLE__)
#include <unistd.h>
#endif

#include "utils/console.h"
#include "version.h"

using std::cout, std::endl;

using std::ifstream, std::ostringstream, std::ofstream, std::sregex_iterator, std::smatch, std::to_string, std::string,
        std::filesystem::temp_directory_path, std::filesystem::path, std::filesystem::exists, std::filesystem::copy,
        std::filesystem::is_regular_file,
        std::filesystem::status, std::filesystem::perms, std::filesystem::absolute,
        std::filesystem::is_directory, std::filesystem::directory_iterator, std::filesystem::create_directories;

vector<string> splitBySpace(const string&input) {
    std::regex regex_pattern(R"((\S|^)\"[^"]*"|\([^)]*(\)*)|"[^"]*"|\S+)");
    std::vector<std::string> tokens;

    auto words_begin = std::sregex_iterator(input.begin(), input.end(), regex_pattern);
    auto words_end = std::sregex_iterator();

    for (std::sregex_iterator i = words_begin; i != words_end; ++i) {
        if (const std::smatch&match = *i;
            !tokens.empty() &&
            tokens.back()[tokens.back().size() - 1] == ')' && match.str()[0] == '!')
            tokens.back() += match.str();
        else tokens.push_back(match.str());
    }

    return tokens;
}

vector<string> split(const string&s, const regex&regex) {
    std::sregex_token_iterator iter(s.begin(), s.end(), regex, -1);
    std::sregex_token_iterator end;
    return {iter, end};
}

const string WHITESPACE = " \n\r\t\f\v";

string ltrim(const string&s) {
    const size_t start = s.find_first_not_of(WHITESPACE);
    return start == string::npos ? "" : s.substr(start);
}

string rtrim(const string&s) {
    const size_t end = s.find_last_not_of(WHITESPACE);
    return end == string::npos ? "" : s.substr(0, end + 1);
}

string trim(const string&s) { return rtrim(ltrim(s)); }

int levenshteinDist(const string&str1, const string&str2) {
    const int len1 = static_cast<int>(str1.length());
    const int len2 = static_cast<int>(str2.length());

    vector dp(len1 + 1, vector(len2 + 1, 0));

    for (int i = 0; i <= len1; ++i) {
        for (int j = 0; j <= len2; ++j) {
            if (i == 0) {
                dp[i][j] = j;
            }
            else if (j == 0) {
                dp[i][j] = i;
            }
            else if (str1[i - 1] == str2[j - 1]) {
                dp[i][j] = dp[i - 1][j - 1];
            }
            else {
                dp[i][j] = 1 + std::min({dp[i - 1][j], dp[i][j - 1], dp[i - 1][j - 1]});
            }
        }
    }

    return dp[len1][len2];
}

string replace(string str, const string&from, const string&to) {
    size_t start_pos = 0;
    while ((start_pos = str.find(from, start_pos)) != string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length();
    }
    return str;
}

string readFile(const path&path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << path << std::endl;
        return "";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();

    return buffer.str();
}

void writeFile(const path&path, const string&value) {
    ofstream file(path);
    file << value;
    file.close();
}

size_t writeText(void* ptr, const size_t size, const size_t nmemb, std::string* data) {
    data->append(static_cast<char *>(ptr), size * nmemb);
    return size * nmemb;
}


std::string readTextFromWeb(const std::string&url) {
    auto [code, body, headers] = RestClient::get(url);

    if (code == 200) {
        return body;
    }

    std::cerr << "Error: " << code << " - " << body << std::endl;
    return "";
}

struct ProgressData {
    double lastPercentage;
};

size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    FILE* fp = static_cast<FILE *>(userp);
    return fwrite(contents, size, nmemb, fp);
}

int ProgressCallback(void* clientp, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow) {
    auto* progressData = static_cast<ProgressData *>(clientp);

    if (dltotal > 0) {
        int progress = static_cast<int>(static_cast<float>(dlnow) / static_cast<float>(dltotal) * 100);

        if (progress - progressData->lastPercentage >= 1.0) {
            std::cout << "[";
            const int pos = progress * 20;
            for (int i = 0; i < 20; ++i) {
                if (i < pos) std::cout << "=";
                else if (i == pos) std::cout << ">";
                else std::cout << " ";
            }
            std::cout << "] " << static_cast<int>(progress * 100.0) << " %\r";
            progressData->lastPercentage = progress;
        }
    }

    return 0;
}

bool downloadFile(const string&url, const path&file) {
    ProgressData progress = {0.0};

    curl_global_init(CURL_GLOBAL_DEFAULT);

    if (CURL* curl = curl_easy_init()) {
        CURLcode res;

        if (FILE* fp = fopen(file.string().c_str(), "wb")) {
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, ProgressCallback);
            curl_easy_setopt(curl, CURLOPT_XFERINFODATA, &progress);
            curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0);

            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);

            curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);

            res = curl_easy_perform(curl);

            fclose(fp);
        }
        else {
            std::cerr << "Failed to open local file for writing." << std::endl;
            curl_easy_cleanup(curl);
            curl_global_cleanup();
            return false;
        }

        if (res != CURLE_OK) {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
            curl_easy_cleanup(curl);
            curl_global_cleanup();
            return false;
        }

        curl_easy_cleanup(curl);
        curl_global_cleanup();
        cout << "[" << string(20, '=') << "] DONE!" << endl;
        return true;
    }

    return false;
}

int onExtractEntry(const char* filename, void* arg) {
    if (const string name = path(filename).filename().string(); !name.empty()) {
        cout << erase_cursor_to_end << "Extracting... (" << name << ")" << teleport(0, wherey() - 1) << endl;
    }
    return 0;
}

path extractZip(const path&zipFile, path extractPath) {
    if (!exists(zipFile)) {
        error("Zip file does not exist!");
        return "";
    }
    int arg = 0;
    if (zip_extract(zipFile.string().c_str(), extractPath.string().c_str(), onExtractEntry, &arg) == 0) {
        cout << erase_cursor_to_end << "Extracting... (Done!)" << endl;
        return extractPath;
    }
    error("Failed to extract zip file ($0)!", {absolute(zipFile).string()});
    return "";
}

void updateShuffle() {
    const path temp = temp_directory_path().append("shflupdater.zip");
#ifdef _WIN32
    string url = "https://github.com/shflterm/shuffle/releases/download/updater/bin-windows.zip";
#elif defined(__linux__)
    string url = "https://github.com/shflterm/shuffle/releases/download/updater/bin-linux.zip";
#elif defined(__APPLE__)
    string url = "https://github.com/shflterm/shuffle/releases/download/updater/bin-macos.zip";
#endif
    downloadFile(url, temp.string());
    path extractPath = temp_directory_path().append("shflupdater");
    extractZip(temp, extractPath);

#ifdef _WIN32
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));
    const string command = extractPath.append("updater.exe").string();

    CreateProcess(nullptr,
                  const_cast<LPSTR>(command.c_str()),
                  nullptr,
                  nullptr,
                  FALSE,
                  CREATE_NEW_CONSOLE,
                  nullptr,
                  nullptr,
                  &si,
                  &pi);
#elif defined(__linux__) || defined(__APPLE__)
    const string command = extractPath.append("updater").string();

    const pid_t pid = fork();

    if (pid == -1) {
        error("Failed to update Shuffle. (fork() failed.)");
        return;
    }
    if (pid == 0) {
        const vector<const char *> args{command.c_str(), nullptr};
        execvp(command.c_str(), const_cast<char * const*>(args.data()));

        // If execvp() returns, an error occurred.
        error("Failed to update Shuffle. (execvp() failed.)");
        exit(EXIT_FAILURE);
    }
#endif
    exit(0);
}

bool checkUpdate(const bool checkBackground) {
    if (const string latest = trim(readTextFromWeb(
        "https://raw.githubusercontent.com/shflterm/shuffle/main/LATEST")); latest != SHUFFLE_VERSION.str()) {
        cout << "New version available: " << SHUFFLE_VERSION.str() << " -> "
                << latest << endl;
        if (checkBackground) cout << "Type 'shfl upgrade' to get new version!" << endl;
        return true;
    }
    if (!checkBackground) cout << "You are using the latest version of Shuffle." << endl;
    return false;
}

std::string generateRandomString(const int length) {
    std::string characters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution<> distribution(0, characters.length() - 1);

    std::string randomString;
    for (int i = 0; i < length; ++i) {
        randomString += characters[distribution(generator)];
    }

    return randomString;
}

vector<path> getPathDirectories() {
    vector<path> directories;
#ifdef _WIN32
    string pathEnv = "Path", delimiter = ";";
#elif defined(__linux__) || defined(__APPLE__)
    string pathEnv = "PATH", delimiter = ":";
#endif
    if (const char* pathVariable = std::getenv(pathEnv.c_str()); pathVariable != nullptr) {
        for (const auto&directory: split(pathVariable, regex(delimiter))) directories.emplace_back(directory);
    }
    return directories;
}

bool isExecutableInPath(const path&currentDirectory, const string&executableName) {
    vector<path> directories = getPathDirectories();
    directories.emplace_back(currentDirectory);

    return std::any_of(directories.begin(), directories.end(), [&](const path&directory) {
        path fullPath = directory / executableName;
        bool result = exists(fullPath) && is_regular_file(fullPath) &&
                      (status(fullPath).permissions() & perms::owner_exec) != perms::none;
        if (result) return true;

#ifdef _WIN32
        fullPath = fullPath.string() + ".exe";
        return exists(fullPath) && is_regular_file(fullPath) &&
               (status(fullPath).permissions() & perms::owner_exec) != perms::none;
#else
        return false;
#endif
    });
}

vector<string> getExecutableFilesInPath(const vector<path>&directories) {
    vector<string> executables;
    for (const auto&directory: directories) {
        try {
            for (const auto&entry: directory_iterator(directory)) {
                if (is_regular_file(entry) && (status(entry).permissions() & perms::owner_exec) != perms::none) {
                    executables.push_back(entry.path().filename().string());
                }
            }
        }
        catch (const std::exception ignored) {
        }
    }
    return executables;
}

bool endsWith(const std::string&str, const std::string&suffix) {
    if (str.length() < suffix.length()) {
        return false;
    }
    return str.substr(str.length() - suffix.length()) == suffix;
}

#ifdef _WIN32
FILE* popen(const char* command, const char* modes) {
    return _popen(command, modes);
}

int pclose(FILE* pipe) {
    return _pclose(pipe);
}
#endif

bool execute_program(const string&command) {
    if (FILE* pipe = popen(command.c_str(), "r");
        pipe != nullptr) {
        char buffer[128];

        while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
            cout << buffer;
        }

        if (const int status = pclose(pipe); status == 0) {
            return true;
        }
    }
    return false;
}
