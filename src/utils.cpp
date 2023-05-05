#include "utils.h"

#include <iostream>
#include <vector>
#include <string>
#include <regex>
#include <algorithm>
#include <fstream>
#include <sstream>

using namespace std;

vector<string> split(const string &s, const regex &delimiter_regex) {
  std::sregex_token_iterator iter(s.begin(), s.end(), delimiter_regex, -1);
  std::sregex_token_iterator end;
  return {iter, end};
}

const string WHITESPACE = " \n\r\t\f\v";

string ltrim(const string &s) {
  size_t start = s.find_first_not_of(WHITESPACE);
  return (start == string::npos) ? "" : s.substr(start);
}

string rtrim(const string &s) {
  size_t end = s.find_last_not_of(WHITESPACE);
  return (end == string::npos) ? "" : s.substr(0, end + 1);
}

string trim(const string &s) { return rtrim(ltrim(s)); }

int levenshteinDist(const string &str1, const string &str2) {
  int len1 = (int) str1.length();
  int len2 = (int) str2.length();

  vector<vector<int>> dp(len1 + 1, vector<int>(len2 + 1, 0));

  for (int i = 0; i <= len1; ++i) {
    for (int j = 0; j <= len2; ++j) {
      if (i == 0) {
        dp[i][j] = j;
      } else if (j == 0) {
        dp[i][j] = i;
      } else if (str1[i - 1] == str2[j - 1]) {
        dp[i][j] = dp[i - 1][j - 1];
      } else {
        dp[i][j] = 1 + std::min({dp[i - 1][j], dp[i][j - 1], dp[i - 1][j - 1]});
      }
    }
  }

  return dp[len1][len2];
}

string replace(string str, const string &from, const string &to) {
  size_t start_pos = 0;
  while ((start_pos = str.find(from, start_pos)) != string::npos) {
    str.replace(start_pos, from.length(), to);
    start_pos += to.length();
  }
  return str;
}

string readFile(const string &path) {
  ifstream file(path);
  ostringstream content_stream;
  content_stream << file.rdbuf();
  return content_stream.str();
}
