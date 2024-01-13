//
// Created by winch on 5/5/2023.
//

#ifndef SHUFFLE_INCLUDE_UTILS_H_
#define SHUFFLE_INCLUDE_UTILS_H_

#ifdef _WIN32
#define HOME path(getenv("APPDATA"))
#elif defined(__linux__) || defined(__APPLE__)
#define HOME std::filesystem::path(getenv("HOME"))
#endif
#define DOT_SHUFFLE (HOME / ".shuffle")
#define SHFL_JSON (DOT_SHUFFLE / "shfl.json")
#define PY_PKGS (DOT_SHUFFLE / "py_pkgs")

#include <string>
#include <vector>
#include <regex>
#include <filesystem>

using std::regex, std::vector, std::string, std::filesystem::path;

extern string pythonPlatform;

std::vector<std::string> splitBySpace(const std::string&input);

vector<string> split(const string&s, const regex&regex);

string trim(const string&s);

int levenshteinDist(const string&str1, const string&str2);

string replace(string str, const string&from, const string&to);

string readFile(const path&path);

void writeFile(const path&path, const string&value);

string readTextFromWeb(const string&url);

bool downloadFile(const string&url, const path&file);

path extractZip(const path&zipFile, path extractPath);

void updateShuffle();

bool checkUpdate(bool checkBackground = true);

std::string generateRandomString(int length);

bool isExecutableInPath(const path&currentDirectory, const string& executableName);

bool endsWith(const std::string& str, const std::string& suffix);

bool execute_program(const string&command);

#endif //SHUFFLE_INCLUDE_UTILS_H_
